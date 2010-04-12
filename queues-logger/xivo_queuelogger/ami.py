# vim: set expandtab ts=4 sw=4 sts=4 fileencoding=utf-8:

class ami:
# {
    @staticmethod
    def forge_command(action, cmd):
    # {
        str = "Action: " + action + "\r\n"
        for i, v in cmd.iteritems():
            str += i + ": " + v + "\r\n"
        str += "\r\n"

        return str
    # }
# }
