# vim: set expandtab ts=4 sw=4 sts=4 fileencoding=utf-8:

import time


class log_event:
# {
    def __init__(self, ev, cache):
        self.sql = getattr(self, ev["Event"], getattr(self, "ignored_event"))(ev, cache)

    def ignored_event(self, ev, cache):
        return ""

    def trace_event(self, ev, cache):
    # {
        if not cache.has_key(ev['Queue']):
            cache[ev['Queue']] = {}

        if not cache[ev['Queue']].has_key(ev['Uniqueid']):
            cache[ev['Queue']][ev['Uniqueid']] = ev
        else:
            cache[ev['Queue']][ev['Uniqueid']] = \
                dict(cache[ev['Queue']][ev['Uniqueid']].items() + ev.items())
    # }

    def is_traced_event(self, ev, cache):
        return cache.has_key(ev['Queue']) and  cache[ev['Queue']].has_key(ev['Uniqueid'])

    def Join(self, ev, cache):
    # {
        ev['call_time_t'] = time.time()

        sql = '''INSERT INTO "queue_info" ("call_time_t", "queue_name", ''' \
                          '''"caller", "caller_uniqueid") ''' \
              '''VALUES (%d, "%s", "%s", "%s");''' % \
              (ev['call_time_t'], ev['Queue'], ev['CallerID'], ev['Uniqueid'])

        self.trace_event(ev, cache)
        return sql
    # }

    def AgentConnect(self, ev, cache):
    # {
        if not self.is_traced_event(ev, cache):
            return ""

        ct = cache[ev['Queue']][ev['Uniqueid']]['call_time_t']
        
        sql = '''UPDATE "queue_info" '''\
              '''SET "call_picker" = "%s", "hold_time" = %s '''\
              '''WHERE "call_time_t" = %d and "caller_uniqueid" = "%s"; ''' %\
              (ev["Member"], ev["Holdtime"], ct, ev["Uniqueid"]);

        self.trace_event(ev, cache)
        return sql
    # }


    def AgentComplete(self, ev, cache):
    # {
        if not self.is_traced_event(ev, cache):
            return ""

        ct = cache[ev['Queue']][ev['Uniqueid']]['call_time_t']

        sql = '''UPDATE "queue_info" ''' \
              '''SET "talk_time" = %s ''' \
              '''WHERE "call_time_t" = %d and "caller_uniqueid" = "%s"; ''' % \
              (ev['TalkTime'], ct, ev['Uniqueid'])

        del cache[ev['Queue']][ev['Uniqueid']]

        return sql
    # }

    def Leave(self, ev, cache):
    # {
        if not self.is_traced_event(ev, cache):
            return ""

        hold_time = time.time() - cache[ev['Queue']][ev['Uniqueid']]['call_time_t']
        ct = cache[ev['Queue']][ev['Uniqueid']]['call_time_t']

        sql = '''UPDATE "queue_info" ''' \
              '''SET "hold_time" = %d ''' \
              '''WHERE "call_time_t" = %d and "caller_uniqueid" = "%s"; ''' % \
              (hold_time, ct, ev['Uniqueid'])

        if ev['Reason'] == "0":
            del cache[ev['Queue']][ev['Uniqueid']]

        return sql
    # }
# }
