#include "conference.h"

Q_EXPORT_PLUGIN2(xletconferenceplugin, XLetConferencePlugin);

ConfTab::ConfTab(QWidget *parent)
    : QTabWidget(parent)
{
}

void ConfTab::closeTab(const QString &id)
{
    int index;

    if (id.isNull()) {
        QWidget *tab;
        tab = sender()->property("index").value<QWidget*>();
        index = indexOf(tab);
        removeTab(index);
    }
}

int ConfTab::addClosableTab(QWidget *w, const QString &title)
{
    int index = addTab(w, title);

    QPushButton *p = new QPushButton("X");
    p->setFlat(true);
    p->setMaximumSize(12, 20);
    p->setFocusPolicy(Qt::NoFocus);
    p->setProperty("index", qVariantFromValue(w));
    connect(p, SIGNAL(clicked()), this, SLOT(closeTab()));
    tabBar()->setTabButton(index, QTabBar::RightSide, p);
    return index;
}

void ConfTab::showConfRoom(const QString &id, bool force)
{
    int index;
    if ((index = indexOf(id)) == -1) {
        if ((!force) && 
            (b_engine->eVM(QString("confrooms/%0/in").arg(id)).size() == 0)) {
            return ;
        }

        QString roomName = \
            b_engine->eV(QString("confrooms/%0/name").arg(id)).toString();
        QString roomNumber = \
            b_engine->eV(QString("confrooms/%0/number").arg(id)).toString();

        index = addClosableTab(new ConfChamber(this, id),
                               QString("%0 (%1)").arg(roomName).arg(roomNumber));
    }
    setCurrentIndex(index);
}

int ConfTab::indexOf(const QString &id)
{
    int i, e;
    for(i=1,
        e=count();i<e;i++) {
        if (widget(i)->property("id").toString() == id) {
            return i;
        }
    }
    return -1;
}

XLet* XLetConferencePlugin::newXLetInstance(QWidget *parent)
{
    b_engine->registerTranslation(":/conference_%1");
    return new XletConference(parent);
}

XletConference::XletConference(QWidget *parent)
    : XLet(parent)
{
    setTitle(tr("Conference"));

    QVBoxLayout *vLayout = new QVBoxLayout();
    setLayout(vLayout);

    m_tab = new ConfTab(this);
    m_tab->addTab(new ConfList(this), tr("Conference room list"));
    vLayout->addWidget(m_tab);
}

void XletConference::openConfRoom(const QString &id, bool force)
{
    m_tab->showConfRoom(id, force);
}
