#!/usr/bin/python

import UpCollections
from RestHTTPConnector import *

a = RestHTTPRegistrar('localhost', 8080)
a._RestHTTPRegistrar__dispatcher = 42
a.start_listener()
