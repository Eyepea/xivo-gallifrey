"""XiVO phones provisioning util functions

Copyright (C) 2010  Proformatique
"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2010  Proformatique

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

from xivo.xivo_helpers import speed_dial_key_extension, fkey_extension

UF_TABLE            = 'userfeatures'
SIP_TABLE           = 'usersip'
FK_TABLE            = 'phonefunckey'
XNUM_TABLE          = 'extenumbers'

def name_from_first_last(first, last):
    "Construct full name from first and last."
    if first and last:
        return first + ' ' + last
    if first:
        return first
    if last:
        return last
    return ''

def get_extensions(cursor):
    extensions = {'callgroup':          None,
                  'callqueue':          None,
                  'calluser':           None,
                  'enablednd':          None,
                  'fwdunc':             None,
                  'parkext':            None,
                  'pickup':             None,
                  'pickupexten':        None,
                  'phoneprogfunckey':   None,
                  'vmusermsg':          None}

    cursor.query(("SELECT ${columns} FROM %s "
        "WHERE type IN ('extenfeatures', 'featuremap', 'generalfeatures') "
        "AND typeval IN (" + ", ".join(["%%s"] * len(extensions)) + ")")
        % (XNUM_TABLE,),
        ('typeval', 'exten'),
        extensions.keys())
    res = cursor.fetchall()
    if res:
        extensions.update(dict([x['typeval'], x['exten']] for x in res))

    return extensions

def get_funckeys(cursor, iduserfeatures, number):
    extensions = get_extensions(cursor)

    cursor.query(("SELECT ${columns} "
        "FROM %s "
        "LEFT OUTER JOIN %s AS extenumleft "
        "ON  %s.typeextenumbers = extenumleft.type "
        "AND %s.typevalextenumbers = extenumleft.typeval "
        "LEFT OUTER JOIN %s AS extenumright "
        "ON  %s.typeextenumbersright = extenumright.type "
        "AND %s.typevalextenumbersright = extenumright.typeval "
        "LEFT OUTER JOIN %s "
        "ON %s.typevalextenumbersright = %s.id "
        "WHERE iduserfeatures = %%s")
        % (FK_TABLE,
           XNUM_TABLE,
           FK_TABLE,
           FK_TABLE,
           XNUM_TABLE,
           FK_TABLE,
           FK_TABLE,
           UF_TABLE,
           FK_TABLE,
           UF_TABLE),
        [FK_TABLE+x for x in ('.fknum',
                              '.exten',
                              '.supervision',
                              '.progfunckey',
                              '.iduserfeatures',
                              '.label',
                              '.typeextenumbersright',
                              '.typevalextenumbersright')]
        + ['extenumleft.exten', 'extenumright.exten',
           'extenumleft.type', 'extenumleft.typeval',
           'extenumright.type', 'extenumright.typeval']
        + [UF_TABLE+x for x in ('.firstname', '.lastname', '.id')],
        (iduserfeatures,))

    funckey = {}
    for fk in cursor.fetchall():
        isbsfilter = False
        fknum = fk[FK_TABLE+'.fknum']
        label = fk[FK_TABLE+'.label']
        exten = fk[FK_TABLE+'.exten']

        fkey = {'exten':        None,
                'label':        label,
                'supervision':  bool(int(fk[FK_TABLE+'.supervision']))}

        # SPECIAL CASES:
        if fk['extenumleft.type'] is None:
            # group, queue or user without number
            if fk['extenumright.type'] is None \
                and fk[FK_TABLE+'.typeextenumbersright'] in ('group',
                                                             'queue',
                                                             'user'):
                fkey['exten'] = \
                    fkey_extension(
                        extensions.get('call' + fk[FK_TABLE+'.typeextenumbersright']),
                        (fk[FK_TABLE+'.typevalextenumbersright'],))
                funckey[fknum] = fkey
                continue

        elif fk['extenumleft.type'] == 'extenfeatures':
            if fk['extenumleft.typeval'] == 'bsfilter':
                isbsfilter = True
            elif bool(fk[FK_TABLE+'.progfunckey']):
                if exten is None and fk[FK_TABLE+'.typevalextenumbersright'] is not None:
                    exten = '*' + fk[FK_TABLE+'.typevalextenumbersright']

                fkey['exten'] = \
                    fkey_extension(
                        extensions.get('phoneprogfunckey'),
                        (fk[FK_TABLE+'.iduserfeatures'],
                         fk['extenumleft.exten'],
                         exten))

                funckey[fknum] = fkey
                continue

        if label is None \
           and fk[FK_TABLE+'.typeextenumbersright'] == 'user' \
           and fk[UF_TABLE+'.id']:
               label = name_from_first_last(fk[UF_TABLE+'.firstname'],
                                            fk[UF_TABLE+'.lastname'])

               if isbsfilter:
                   fkey['label'] = "BS:%s" % label
               else:
                   fkey['label'] = label

        fkey['exten'] = \
            speed_dial_key_extension(
                fk['extenumleft.exten'],
                fk['extenumright.exten'],
                exten,
                number,
                isbsfilter)

        fkey['altlabel'] = fk['extenumleft.typeval']
        funckey[fknum] = fkey

    return funckey
