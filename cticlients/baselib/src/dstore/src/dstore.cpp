/* XiVO Client
 * Copyright (C) 2007-2010, Proformatique
 *
 * This file is part of XiVO Client.
 *
 * XiVO Client is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, with a Section 7 Additional
 * Permission as follows:
 *   This notice constitutes a grant of such permission as is necessary
 *   to combine or link this software, or a modified version of it, with
 *   the OpenSSL project's "OpenSSL" library, or a derivative work of it,
 *   and to copy, modify, and distribute the resulting work. This is an
 *   extension of the special permission given by Trolltech to link the
 *   Qt code with the OpenSSL library (see
 *   <http://doc.trolltech.com/4.4/gpl.html>). The OpenSSL library is
 *   licensed under a dual license: the OpenSSL License and the original
 *   SSLeay license.
 *
 * XiVO Client is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XiVO Client.  If not, see <http://www.gnu.org/licenses/>.
 */

/* $Revision$
 * $Date$
 */

#include "dstore_private.h"

static QRegExp finalSlash, leadingSlash;

#define NOOP               0
#define IS_EQUAL        0x01
#define IS_DIFFERENT    0x02
#define IS_INFERIOR     0x10
#define IS_SUPERIOR     0x20
#define ORDER_KIND      (IS_INFERIOR|IS_SUPERIOR)

DStore::DStore()
    : m_4genUid(0),
      m_path2uid(QHash<QString, qlonglong>()),
      m_uid2node(QHash<qlonglong, DStoreNode*>()),
      m_blockSignal(0),
      m_root(new VMapNode("", NULL, this))
{ 
    leadingSlash = QRegExp("^/*");
    finalSlash = QRegExp("/*$");
}

int DStore::getUid()
{
    while (m_uid2node.contains(++m_4genUid));
    return m_4genUid;
}

QString DStore::sanitize(const QString &path)
{
    return QString(path).remove(leadingSlash)
                        .remove(finalSlash);
}

void DStore::populate(const QString &path, const QVariant &value)
{
    QStringList traverseList = sanitize(path).split("/");
    QString baseName = traverseList.takeLast();
    QString dirName = traverseList.join("/");
    VMapNode *dadNode = mkPath(dirName);
  
    if (value.type() == QVariant::Map) {
        VMapNode *node = new VMapNode(baseName, dadNode, this);
        node->populate(value.toMap(), this);
    } else {
        new VNode(baseName, dadNode, this, value);
    }

    if (!m_blockSignal) {
        dynamicInvocation(path, NODE_POPULATED);
    }
}

VMapNode* DStore::mkPath(const QString &path, VMapNode *from)
{
    QStringList traverseList = sanitize(path).split("/");
    VMapNode *traverse = from?from:m_root;

    int i, e;
    for (i=0, e=traverseList.count();i<e;i++) {
        VMapNode *map = VMapNode::getNode(traverseList[i], traverse, this);
        traverse = map;
    }

    if (!m_blockSignal) {
        dynamicInvocation(traverse->path(), NODE_POPULATED);
    }

    return traverse;
}

DStoreNode* DStore::getNode(qlonglong uid)
{
    if (m_uid2node.contains(uid)) {
        return m_uid2node.value(uid);
    } else {
        return NULL;
    }
}

DStoreNode* DStore::getNode(const QString &path)
{
    if (m_path2uid.contains(path)) {
        return getNode(m_path2uid.value(path));
    } else {
        return NULL;
    }
}

void DStore::registerNode(DStoreNode *node)
{
    m_path2uid.insert(node->path(), node->uid());
    m_uid2node.insert(node->uid(), node);
}

void DStore::unregisterNode(DStoreNode *node)
{
    m_path2uid.remove(node->path());
    m_uid2node.remove(node->uid());
}

DStore::~DStore()
{
    root()->destroy(this, 1);
    QHashIterator<QString, DStoreCallback*> i(m_callbackList);

    while (i.hasNext()) {
        i.next();
        delete i.value();
    }

    m_callbackList.clear();
}

void DStore::rmPath(const QString &path)
{
    DStoreNode *node = getNode(path);
    QStringList traverseList = sanitize(path).split("/");
    QString baseName = traverseList.takeLast();

    if (!m_blockSignal) {
        dynamicInvocation(path, NODE_REMOVED);
    }

    if (node) {
        if (node->parent()) {
            node->parent()->remove(baseName);
        }
        if (node->type() == INNER) {
            static_cast<VMapNode*>(node)->destroy(this, 1);
        } else {
            static_cast<VNode*>(node)->destroy(this, 1);
        }
    }
}

void DStore::filter(int op, const QString &filter, const QVariantList &with)
{
    VMapNode *r = root();

    if (op&IS_INFERIOR) {
        op = IS_INFERIOR | ((op&IS_EQUAL)?0:IS_EQUAL);
    }

    foreach(QString nodeName, r->nodeNames()) {
        DStoreNode *dnode = r->node(nodeName);
        if (dnode->type() == INNER) {
            QVariant value = static_cast<VMapNode *>(dnode)->variant(filter);
            if (!(op&ORDER_KIND)) {
                if ((with.count(value)) ^ (op == IS_EQUAL)) {
                    rmPath(nodeName);
                }
            }
        }
    }
}

void DStore::filter(int op, const QString &filter, const QVariant &with)
{
    VMapNode *r = root();

    if (op&IS_INFERIOR) {
        op = IS_INFERIOR | ((op&IS_EQUAL)?0:IS_EQUAL);
    }

    foreach(QString nodeName, r->nodeNames()) {
        DStoreNode *dnode = r->node(nodeName);
        if (dnode->type() == INNER) {
            QVariant value = static_cast<VMapNode *>(dnode)->variant(filter);
            if (!(op&ORDER_KIND)) {
                if ((value == with) ^ (op == IS_EQUAL)) {
                    rmPath(nodeName);
                }
            } else {
                if (op&IS_EQUAL) {
                    if ((value.toInt() < with.toInt()) ^ ((op&IS_INFERIOR)?1:0)) {
                        rmPath(nodeName);
                    }
                } else {
                    if ((value.toInt() <= with.toInt()) ^ ((op&IS_INFERIOR)?1:0) ) {
                        rmPath(nodeName);
                    }
                }
            }
        }
    }
}

// path      = nodepath, { "/*[", filter, "]"}? ;
// filter    = nodepath, test, ( nodepath | value ) ;
// test      = ("=" | "~" | "<" | ">") ;
// value     = "@", [^]]*;
// quote     = "'" ;
// nodepath  = nodename, { "/", nodename } ;
// nodename  = [^/*<>=~]+ ;


int readTest(const QString &s, int *off)
{
    int ret = NOOP;
    int i;
    QChar c;

    if (s.size() < 2) {
        return NOOP;
    }

    for (i=0;i<2;i++) {
        c = s[i]; *off += 1;
        if (c == QChar('=')) {
            return ret | IS_EQUAL;
        } else if (c == QChar('~')) {
            return IS_DIFFERENT;
        } else if (c == QChar('<')) {
            ret |= IS_INFERIOR;
        } else if (c == QChar('>')) {
            ret |= IS_SUPERIOR;
        } else {
            *off -= 1;
            break;
        }
    }
    return ret;
}


QString readNodePath(const QString &s)
{
    QString nodePath = s;
    int i, e;
    for (i=0,e=s.size();i<e;i++) {
        if ((s[i] == QChar('*')) ||
            (s[i] == QChar('=')) ||
            (s[i] == QChar('<')) ||
            (s[i] == QChar('>')) ||
            (s[i] == QChar('~')))
            break;
    }
    if (i < e) {
        if ((i)&&(s[i-1]==QChar('/'))) {
            nodePath = nodePath.left(i-1);
        } else {
            nodePath = nodePath.left(i);
        }
    }
    return nodePath;
}

DStore* DStore::extract(const QString &path)
{
    QString cleanPath = sanitize(path);
    QString baseNodePath = readNodePath(cleanPath);
    DStore *tree = new DStore();

    DStoreNode *node = getNode(baseNodePath);

    if (node != NULL) {
        node->clone(tree, tree->root(), 1);
    }

    if (cleanPath.indexOf("[") != -1) {
        QString filter = path.right(cleanPath.size() -
                                    baseNodePath.size() - 3);
        QString path = readNodePath(filter);
        QVariant with;

        int test, testOffset = 0;
        test = readTest(filter.right(filter.size() - path.size()), &testOffset);

        if (filter[path.size() + testOffset] == QChar('@')) {
            with = QVariant(filter.mid(path.size() + 1 + testOffset,
                            filter.size() - path.size() -  2 - testOffset));

        } else { 
            with = root()->variant(filter.mid(path.size() + testOffset,
                                   filter.size() - path.size() -  1 - testOffset ));
        }

        tree->filter(test, path, with);
    }
    return tree;
}

QVariantMap DStore::extractVMap(const QString &path)
{
    DStore *tree = extractb(path);
    QVariantMap ret = tree->root()->variantMap();
    delete tree;
    return ret;
}

QVariant DStore::extractVariant(const QString &path)
{
    DStore *tree = extractb(path);
    QVariantMap map = tree->root()->variantMap();
    delete tree;

    foreach(QString key, map.keys()) {
        return map[key];
    }

    return QVariant();
}

void DStore::onChange(const QString &path, QObject *target, const char *slot)
{
    QString cb = QString(slot).remove(QRegExp("\\(.*$"));
    m_callbackList.insert(sanitize(path),
                          new DStoreCallback(target, cb.toAscii().constData()));
}

void DStore::unregisterAllCb(QObject *on)
{
    int i, e;
    QList<QPair<QString, DStoreCallback*> > listToRemove;

    foreach (QString cb, m_callbackList.keys()) {
        QList<DStoreCallback*> list = m_callbackList.values(cb);
        for (i=0,e=list.size();i<e;i++) {
            if (on == list[i]->on()) {
                listToRemove.append(QPair<QString, DStoreCallback*>(cb, list[i]));
            }
        }
    }

    for (i=0, e=listToRemove.size();i<e;i++) {
        m_callbackList.remove(listToRemove[i].first, listToRemove[i].second);
    }
}

void DStore::dynamicInvocation(const QString &path, DStoreEvent event)
{
    QString triggerPath = sanitize(path);
    QStringList traverseList = triggerPath.split("/");


    do {
        if (m_callbackList.contains(triggerPath)) {
            QList<DStoreCallback*> cbList = m_callbackList.values(triggerPath);
            int i, e;
            for (i=0, e=cbList.size();i<e;++i) {
                cbList.at(i)->call(path, event);
            }
        }

        traverseList.removeLast();
        triggerPath = traverseList.join("/");

    } while (traverseList.count());
}

#include "grammar.h"
#define ParseARG_PDECL ParserRet*
#define ParseTOKENTYPE int
void* ParseAlloc(void* (*)(size_t )); 
void ParseFree(void *, void (*)(void *));                  
void Parse(void *, int, ParseTOKENTYPE, ParseARG_PDECL);

DStore* DStore::extractb(const QString &path)
{
    ParserRet list;
    list.origin = this;
    list.ret = NULL;
    list.req = path;
    list.abort = 0;
    
    void *pParser = ParseAlloc(malloc);        
    
    int i, e;
    char c;
    for (i=0, e=path.size();(i<e)&&(list.abort==0);i++) {
        c = path[i].toAscii();

        if (c == '\\') {
            i += 1;
            if (i>e) {
                break;
            }
            c = path[i].toAscii();
            Parse(pParser, CHARACTER, c, &list);                
        } else if ((c == '>')||(c== '<')) {
            int op = (c == '<') ? IS_INFERIOR : IS_SUPERIOR;
            if (i+1>e) {
                break;
            }
            if (path[i+1].toAscii() == '=') {
                op |= IS_EQUAL;
                i += 1;
            }
            Parse(pParser, TEST, op, &list);
        } else if (c == '/') {
            Parse(pParser, SLASH, 0, &list);                
        } else if (c == '@') {
            Parse(pParser, AT, 0, &list);                
        } else if (c == '=') {
            Parse(pParser, TEST, IS_EQUAL, &list);                
        } else if (c == '~') {
            Parse(pParser, TEST, IS_DIFFERENT, &list);                
        } else if (c == '[') {
            Parse(pParser, LC, 0, &list);                
        } else if (c == ']') {
            Parse(pParser, RC, 0, &list);                
        } else {
            Parse(pParser, CHARACTER, c, &list);                
        }
    }
    Parse(pParser, 0, 0, &list);
    ParseFree(pParser, free);                  

    return list.ret;
}
