# -*- coding: UTF-8 -*-

from __future__ import with_statement

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2011  Proformatique <technique@proformatique.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

import contextlib
import errno
import gzip
import itertools
import logging
import math
import os
import StringIO
import urllib2
from xivo import anysql
from xivo.BackSQL import backmysql
from xivo.BackSQL import backsqlite
try:
    import json
except ImportError:
    try:
        import simplejson as json
    except ImportError:
        json = None
        import cjson

LAST_TIMESTAMP_FILE = 'qlog-timestamp'
IGNORED_LINES = 'qlog-ignored-lines'

logger = logging.getLogger(__name__)


def _new_opener(base_uri, username, password):
    pwd_manager = urllib2.HTTPPasswordMgrWithDefaultRealm()
    pwd_manager.add_password(None, base_uri, username, password)
    basic_auth_handler = urllib2.HTTPBasicAuthHandler(pwd_manager)
    return urllib2.build_opener(basic_auth_handler)


def _gzip_content(content):
    buf = StringIO.StringIO()
    fobj = gzip.GzipFile(mode='wb', fileobj=buf)
    try:
        fobj.write(content)
    finally:
        fobj.close()
    return buf.getvalue()


def _send_content_to_server(server_uri, username, password, content, headers, compress):
    # headers should be a dictionary that may be modified by this function
    if compress:
        content = _gzip_content(content)
        headers['Content-Type'] = 'application/gzip'
    opener = _new_opener(server_uri, username, password)
    request = urllib2.Request(server_uri, content, headers)
    logger.debug('Sending %s bytes to %s', len(content), server_uri)
    fobj = opener.open(request)
    try:
        fobj.read()
    finally:
        fobj.close()


def _open_qlog_file(file):
    if file.endswith('.gz'):
        return contextlib.closing(gzip.open(file))
    else:
        return open(file)


def _get_qlog_lines_from_file(qlog_file, ref_timestamp, ignored_lines):
    # Return an iterator that yields (timestamp, line) tuples for every line
    # in qlog_file such that timestamp >= ref_timestamp.
    #
    # qlog_file can be either a plain text file or a gzipped file
    with _open_qlog_file(qlog_file) as fobj:
        for line_no, line in enumerate(fobj):
            raw_timestamp = line.split('|', 1)[0]
            try:
                timestamp = int(raw_timestamp)
            except ValueError, e:
                logger.error('Error while parsing timestamp in %s:%s: %s',
                             qlog_file, line_no + 1, e)
            else:
                if timestamp > ref_timestamp:
                    yield timestamp, line.rstrip()
                elif timestamp == ref_timestamp:
                    line = line.rstrip()
                    if line not in ignored_lines:
                        yield timestamp, line.rstrip()


def _get_qlog_files(qlog_basepath):
    # Return an iterator that yields queuelog filenames from a basepath.
    #
    # qlog_basepath is something like "/var/log/queuelog"
    directory, basename = os.path.split(qlog_basepath)
    for file in os.listdir(directory):
        if file.startswith(basename):
            yield os.path.join(directory, file)


class _QlogsReader(object):
    """Iterator that yields queuelog lines for every queuelog file matching
    qlog_basepath such that the timestamp of every yielded line is >= than
    ref_timestamp.
    
    Instances of this class have the following attributes: 
      last_timestamp -- the "latest" timestamp seen while reading the queuelogs.
        This field is dynamically updated, i.e. it may take new values as you
        consume the iterator.
      last_ignored_lines -- a list of lines for which the timestamp is equal to the
        latest timestamp
    
    """
    
    def __init__(self, qlog_basepath, ref_timestamp, ignored_lines, use_mtime):
        self.last_timestamp = ref_timestamp
        self.last_ignored_lines = list(ignored_lines)
        self._iter = self._new_iterator(qlog_basepath, ref_timestamp, ignored_lines, use_mtime)
        
    def _new_iterator(self, qlog_basepath, ref_timestamp, ignored_lines, use_mtime):
        # XXX use local variables for last_timestamp and last_ignored_lines and tells
        #     they are only available once the iterator is consumed completely ?
        total_line_no = 0
        for file in _get_qlog_files(qlog_basepath):
            logger.debug('Checking file %s', file)
            if use_mtime:
                mtime = math.ceil(os.path.getmtime(file))
                if mtime < ref_timestamp:
                    logger.debug('Skipping file %s (mtime check)', file)
                    continue
            
            line_no = 0
            for timestamp, line in _get_qlog_lines_from_file(file, ref_timestamp, ignored_lines):
                if timestamp > self.last_timestamp:
                    self.last_timestamp = timestamp
                    self.last_ignored_lines = [line]
                elif timestamp == self.last_timestamp:
                    self.last_ignored_lines.append(line)
                line_no += 1
                yield line
            total_line_no += line_no
            logger.info('Found %s new lines in file %s', line_no, file)
        logger.info('Found a total of %s new lines', total_line_no)
    
    def __iter__(self):
        return self
    
    def next(self):
        return self._iter.next()


def _read_last_timestamp(state_dir):
    # Return the timestamp stored in the timestamp file or 0 if the file is
    # missing
    abs_file = os.path.join(state_dir, LAST_TIMESTAMP_FILE)
    try:
        fobj = open(abs_file)
    except IOError, e:
        if e.errno == errno.ENOENT:
            logger.info('No timestamp file at %s, using 0', abs_file)
            return 0
        else:
            raise
    else:
        try:
            return int(fobj.read())
        finally:
            fobj.close()


def _write_last_timestamp(state_dir, timestamp):
    abs_file = os.path.join(state_dir, LAST_TIMESTAMP_FILE)
    with open(abs_file, 'w') as fobj:
        fobj.write(str(timestamp))


def _read_ignored_lines(state_dir):
    # Return the set of stored ignored lines or an empty set if the file is
    # missing
    abs_file = os.path.join(state_dir, IGNORED_LINES)
    try:
        fobj = open(abs_file)
    except IOError, e:
        if e.errno == errno.ENOENT:
            logger.info('No ignored lines file at %s, using empty set', abs_file)
            return set()
        else:
            raise
    else:
        try:
            return set(line.rstrip() for line in fobj)
        finally:
            fobj.close()


def _write_ignored_lines(state_dir, ignored_lines):
    abs_file = os.path.join(state_dir, IGNORED_LINES)
    with open(abs_file, 'w') as fobj:
        fobj.write('\n'.join(ignored_lines))


def send_qlog(server_uri, username, password, state_dir, qlog_basepath, use_mtime=True,
              maxlines=-1, compress=False, dry_run=False):
    logger.info('Sending new queuelog lines...')
    old_timestamp = _read_last_timestamp(state_dir)
    logger.debug('Old timestamp is %s', old_timestamp)
    old_ignored_lines = _read_ignored_lines(state_dir)
    logger.debug('%d old ignored lines', len(old_ignored_lines))
    
    qlogs_reader = _QlogsReader(qlog_basepath, old_timestamp, old_ignored_lines, use_mtime)    
    while True:
        if maxlines <= 0:
            cur_lines = qlogs_reader
        else:
            cur_lines = itertools.islice(qlogs_reader, maxlines)
        content = '\n'.join(cur_lines)
        if not content:
            logger.info('No more lines to transmit')
            break
        
        if dry_run:
            logger.info('Not transmitting qlog to server (dry-run).')
        else:
            logger.info('Transmitting qlog to server...')
            headers = {'Content-Type': 'text/plain'}
            _send_content_to_server(server_uri, username, password, content, headers, compress)
        
    new_timestamp = qlogs_reader.last_timestamp
    new_ignored_lines = qlogs_reader.last_ignored_lines
    logger.debug('New timestamp is %s', new_timestamp)
    logger.debug('%d new ignored lines', len(new_ignored_lines))
    if not dry_run:
        _write_ignored_lines(state_dir, new_ignored_lines)
        _write_last_timestamp(state_dir, new_timestamp)


def _get_agent_infos(cursor):
    cursor.query("SELECT * FROM agentfeatures")
    return cursor.fetchall()


def _serialize_agent_infos_to_json(agent_infos):
    # Handle the fact that we only know which json module is available at
    # runtime...
    if json:
        return json.dumps(agent_infos, separators=(',', ':'))
    else:
        return cjson.encode(agent_infos)


def send_agent_infos(server_uri, username, password, ast_db_uri,
                     compress=False, dry_run=False):
    logger.info('Sending agent infos...')
    connection = anysql.connect_by_uri(ast_db_uri)
    try:
        cursor = connection.cursor()
        agent_infos = _get_agent_infos(cursor)
    finally:
        connection.close()
    
    content = _serialize_agent_infos_to_json(agent_infos)
    if dry_run:
        logger.info('Not transmitting agent infos to server (dry-run).')
    else:
        logger.info('Transmitting agent infos to server...')
        headers = {'Content-Type': 'application/json'}
        _send_content_to_server(server_uri, username, password, content, headers, compress)
