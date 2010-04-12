# vim: set expandtab ts=4 sw=4 sts=4 fileencoding=utf-8:

from xivo import anysql
from xivo.BackSQL import backmysql
from xivo.BackSQL import backsqlite3

import sys
import time
import select


class ami_logger:
# {
    clients = []
    conn = None
    cur = None
    last_transaction = None

    @staticmethod
    def add_client(client):
        ami_logger.clients.append(client)

    @staticmethod
    def client_awaiting_to_send():
    # {
        need_to_send = []
        for client in ami_logger.clients:
            len(client.to_send) and need_to_send.append(client)

        return need_to_send
    # }

    @staticmethod
    def loop_step():
    # {
        rlist,\
        wlist,\
        elist = select.select(ami_logger.clients,
                              ami_logger.client_awaiting_to_send(),
                              [],
                              0.5)

        for client in rlist:
            client.recv()
        for client in wlist:
            client.send()
        for client in ami_logger.clients:
            if len(client.error):
                ami_logger.clients.remove(client)
                ool = client.error
                del client
                return ool

        return 0
    # }

    @staticmethod
    def init(options):
    # {
        ami_logger.conn = anysql.connect_by_uri(options.anysql_uri)
        ami_logger.cur = ami_logger.conn.cursor()
        ami_logger.last_transaction = time.time()
    # }

    @staticmethod
    def store_in_db():
    # {
        transaction = []
        for client in ami_logger.clients:
            transaction += client.sqltransaction
            client.sqltransaction = []

        # transaction aren't supported by anysql wrapper so expect
        # performance issues.
        for query in transaction:
            #print "<<" + query + ">>\n"
            sys.stderr.write(".")
            ami_logger.cur.query(query)
            ami_logger.conn.commit()
    # }

    @staticmethod
    def loop(options):
    # {
        ami_logger.init(options)
        while len(ami_logger.clients):
            ret = ami_logger.loop_step()
            if ret:
                return ret

            if time.time() > ami_logger.last_transaction + 1:
                ami_logger.store_in_db()

        return ("empty", -3)
    # }
# }
