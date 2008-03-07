__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2007, 2008, Proformatique'
__author__    = 'Corentin Le Gall'


class Users:
        def __init__(self):
                self.list = {}
                self.byast = {}
                self.commandclass = None
                return

        def setcommandclass(self, commandclass):
                self.commandclass = commandclass
                return

        def set_ast_list(self, astid):
                self.byast[astid] = Users_byast(astid)
                return

        def adduser(self, astid, userparams):
                self.byast[astid].adduser(userparams)
                return

        def finduser(self, username):
                uinfo = None
                for astid, user_byast in self.byast.iteritems():
                        uinfo = user_byast.finduser(username)
                        if uinfo is not None:
                                break
                return uinfo

        def disconnect_user(self, userinfo):
                astid = userinfo.get('astid')
                username = userinfo.get('user')
                self.byast[astid].disconnect_user(username)
                return

        def update(self, astid):
##                nlist = self.commandclass.getuserlist_ng(asterisklist[0])
##                self.list = nlist
                userl = self.commandclass.getuserlist()
                for ul, vv in userl.iteritems():
                        self.adduser(astid, vv)
                return


## \class Users_byast
# \brief Properties of Users, sorted by Asterisk id
class Users_byast:
        def __init__(self, iastid):
                self.astid = iastid
                self.list = {}
                return

        def adduser(self, inparams):
                [user, agentnum, options] = inparams
                if self.list.has_key(user):
                        # updates
                        self.list[user]['agentnum'] = agentnum
                else:
                        self.list[user] = {'astid'    : self.astid,
                                           'user'     : user,
                                           'agentnum' : agentnum,
                                           'cbstatus' : 'undefined',
                                           'login'    : {},
                                           'calls'    : {},
                                           'queuelist': {},
                                           'options'  : options}

        def deluser(self, user):
                if self.list.has_key(user):
                        self.list.pop(user)

        def finduser(self, user):
                return self.list.get(user)

        def disconnect_user(self, user):
                userinfo = self.list[user]
                if 'connection' in userinfo.get('login'):
                        del userinfo.get('login')['connection']

        def listusers(self):
                lst = {}
                for user,info in self.list.iteritems():
                        lst[user] = info
                return lst

        def listconnected(self):
                lst = {}
                for user,info in self.list.iteritems():
                        if 'logintimestamp' in info:
                                lst[user] = info
                return lst

