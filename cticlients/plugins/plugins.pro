# build projects contained in subdirectories
TEMPLATE = subdirs
SUBDIRS  = xletweb \
           videoxlet \
           xletnull \
           conference \
           history \
           switchboard \
           identity \
           datetime \
           features \
           queues \
           agents \
           agentsnext \
           agentdetails \
           queuedetails \
           queueentrydetails \
           operator
win32:SUBDIRS += outlook
