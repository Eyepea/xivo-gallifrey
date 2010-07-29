#include "confchamber.h"

ConfChamberModel::ConfChamberModel(QObject *parent, const QString &id)
    : QAbstractTableModel(parent), m_admin(0), m_id(id), m_view(NULL)
{
    b_engine->tree()->onChange(QString("confrooms/%0").arg(id), this,
        SLOT(confRoomChange(const QString &, DStoreEvent)));
    extractRow2IdMap();
    startTimer(1000);
    timerEvent(NULL);
}

void ConfChamberModel::timerEvent(QTimerEvent *)
{
    QString req = QString("confrooms/%0/in[user-id=@%1]").arg(m_id)
                                                         .arg(b_engine->xivoUserId());
    m_admin = b_engine->eV(req).toMap()["admin"].toBool();
    updateView();
    reset();
}

void ConfChamberModel::setView(ConfChamberView *v)
{
    m_view = v;

    updateView();
}

void ConfChamberModel::updateView()
{
    if (m_view) {
        if (!m_admin) {
            m_view->hideColumn(ACTION_KICK);
            m_view->hideColumn(ACTION_ALLOW_IN);
            m_view->hideColumn(ACTION_TALK_TO);
        } else {
            m_view->showColumn(ACTION_KICK);
            m_view->showColumn(ACTION_ALLOW_IN);
            m_view->showColumn(ACTION_TALK_TO);
        }
    }
}


void ConfChamberModel::confRoomChange(const QString &path, DStoreEvent event)
{
    extractRow2IdMap();
}

void ConfChamberModel::extractRow2IdMap()
{
    QVariantMap roomInList = b_engine->eVM(QString("confrooms/%0/in").arg(m_id));

    int row = 0;
    if (roomInList.size() != m_row2id.size()) {
        foreach(QString roomId, roomInList.keys()) {
            m_row2id.insert(row++, roomId);
        }
    }
    reset();
}

void ConfChamberModel::sort(int column, Qt::SortOrder order)
{
    struct {
        static bool ascending(const QPair<int, QString> &a,
                              const QPair<int, QString> &b) {
            return QString::localeAwareCompare(a.second, b.second) < 0 ?
                                               true : false;
        }
        static bool descending(const QPair<int, QString> &a,
                               const QPair<int, QString> &b) {
            return QString::localeAwareCompare(a.second, b.second) < 0 ?
                                               false : true;
        }
    } sFun;

    QList<QPair<int, QString> > toSort;

    int i, e;
    for (i=0,e=rowCount(QModelIndex());i<e;i++) {
        toSort.append(QPair<int, QString>(index(i, ID).data().toInt(),
                                          index(i, column).data().toString()));
    }

    qSort(toSort.begin(), toSort.end(), (order == Qt::AscendingOrder) ? 
                                         sFun.ascending :
                                         sFun.descending);

    for (i=0;i<e;i++) {
        m_row2id.insert(i, QString::number(toSort[i].first));
    }
    reset();
}


int ConfChamberModel::rowCount(const QModelIndex&) const
{
    return b_engine->eVM(QString("confrooms/%0/in").arg(m_id)).size();
}

int ConfChamberModel::columnCount(const QModelIndex&) const
{
    return NB_COL;
}

QVariant
ConfChamberModel::data(const QModelIndex &index,
                       int role) const
{
    int row = index.row(), col = index.column();
    QString rowId;

    rowId = m_row2id[row];
    QString in = QString("confrooms/%0/in/%1/").arg(m_id).arg(rowId);

    if (role != Qt::DisplayRole) {
        if (role == Qt::TextAlignmentRole) {
            return Qt::AlignCenter;
        } else if (role == Qt::DecorationRole) {
            if (col == ACTION_KICK) {
                return QPixmap(":images/cancel.png").scaledToHeight(16,
                               Qt::SmoothTransformation);
            } else if (col == ACTION_ALLOW_IN) {
                if (!b_engine->eV(in + "authed").toBool()) {
                    return QPixmap(":images/add.png").scaledToHeight(16,
                                   Qt::SmoothTransformation);
                } else {
                    return QVariant();
                }
            } else if (col == ACTION_TALK_TO) {
                return QPixmap(":in/speak.png").scaledToHeight(16,
                               Qt::SmoothTransformation);
            } else if (col == ACTION_MUTE) {
                if (m_admin) {
                    return QPixmap(":in/mute.png").scaledToHeight(16, Qt::SmoothTransformation);
                } else if (b_engine->eV(in + "user-id").toString() == b_engine->xivoUserId()) {
                    return QPixmap(":in/mute.png").scaledToHeight(16, Qt::SmoothTransformation);
                } else {
                    return QVariant();
                }
            }
        } else if (role == Qt::ToolTipRole) {
            if (col == ACTION_KICK) {
                return tr("Kick");
            } else if (col == ACTION_ALLOW_IN) {
                return tr("Allow in");
            } else if (col == ACTION_TALK_TO) {
                return tr("Talk to");
            } else if (col == ACTION_MUTE) {
                return tr("Mute");
            }
        }
        return QVariant();
    }

    switch (col) {
        case ID:
            return b_engine->eV(in + "id");
        case NUMBER:
            return b_engine->eV(in + "phonenum");
        case ADMIN:
            return (b_engine->eV(in + "admin").toBool()) ? tr("yes") : tr("no");
        case NAME:
        {
            QString name = b_engine->eV(QString("users/*[id=%0user-id]").arg(in))
                                       .toMap()["fullname"].toString();
            if (name.isEmpty()) {
                return tr("nobody");
            }
            return name;
        }
        case ACTION_ALLOW_IN:
            if (b_engine->eV(in + "authed").toBool()) {
                return QString::fromUtf8("✓");
            }
            break;
        case SINCE:
            return QDateTime::fromTime_t(QDateTime::currentDateTime().toTime_t() -
                                         b_engine->eV(in + "time-start").toDouble() - 
                                         b_engine->timeDeltaServerClient()).toUTC() 
                                         .toString("hh:mm:ss");
        default:
            break;
    }
    return QVariant();
}

QVariant
ConfChamberModel::headerData(int section,
                             Qt::Orientation orientation,
                             int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    
    if (orientation == Qt::Horizontal) {
        if (section == ID) {
            return QVariant(tr("ID"));
        } else if (section == NUMBER) {
            return QVariant(tr("Number"));
        } else if (section == NAME) {
            return QVariant(tr("Name"));
        } else if (section == SINCE) {
            return QVariant(tr("Since"));
        } else if (section == ADMIN) {
            return QVariant(tr("Admin"));
        } else if (section == ACTION_KICK) {
            return "K";
        } else if (section == ACTION_ALLOW_IN) {
            return "A";
        } else if (section == ACTION_TALK_TO) {
            return "T";
        } else if (section == ACTION_MUTE) {
            return "M";
        }
    }

    return QVariant();
}

Qt::ItemFlags ConfChamberModel::flags(const QModelIndex &index) const
{
    int row = index.row(), col = index.column();

    QString rowId;
    rowId = m_row2id[row];
    QString in = QString("confrooms/%0/in/%1/").arg(m_id).arg(rowId);

    if (m_admin) {
        if (col == ACTION_KICK) {
            return Qt::ItemIsEnabled;
        }
        if (((col == ACTION_ALLOW_IN) || (col == ACTION_TALK_TO))
              && (!b_engine->eV(in + "authed").toBool())) {
            return Qt::ItemIsEnabled;
        }
        if ( (col == ACTION_MUTE) && (b_engine->eV(in + "mute").toBool())) {
            return Qt::ItemIsEnabled;
        }
    } else {
        if (b_engine->eV(in + "user-id").toString() == b_engine->xivoUserId()) {
            if (col == ACTION_MUTE) {
                if (b_engine->eV(in + "mute").toBool()) {
                    return Qt::ItemIsEnabled;
                }
            } 
        }
    }

    return Qt::NoItemFlags;
}

QString ConfChamberModel::row2participantId(int row) const
{
    return m_row2id[row];
}

QString ConfChamberModel::id() const
{
    return m_id;
}


ConfChamberView::ConfChamberView(QWidget *parent, ConfChamberModel *model)
    : QTableView(parent)
{
    setSortingEnabled(true);
    setModel(model);
    setShowGrid(0);
    verticalHeader()->hide();
    horizontalHeader()->setMovable(true);
    horizontalHeader()->setStretchLastSection(true);


    int ActionCol[] = { ConfChamberModel::ACTION_MUTE,
                        ConfChamberModel::ACTION_TALK_TO,
                        ConfChamberModel::ACTION_ALLOW_IN,
                        ConfChamberModel::ACTION_KICK };
    int i;
    for(i=0;i<(int)(sizeof(ActionCol)/sizeof(ActionCol[0]));i++) {
        setColumnWidth(ActionCol[i], 18);
        horizontalHeader()->setResizeMode(ActionCol[i], QHeaderView::Fixed);
    }

    setColumnWidth(ConfChamberModel::ADMIN, 60);
    horizontalHeader()->setResizeMode(ConfChamberModel::ADMIN, QHeaderView::Fixed);
    setStyleSheet("ConfListView {"
                      "border: none;"
                      "background:transparent;"
                      "color:black;"
                  "}");
    hideColumn(0);

    connect(this, SIGNAL(clicked(const QModelIndex &)),
            this, SLOT(onViewClick(const QModelIndex &)));
}

void ConfChamberView::onViewClick(const QModelIndex &index)
{
    int row = index.row(), col = index.column();

    QString roomId = static_cast<ConfChamberModel*>(model())->id();
    QString castId = model()->index(row, ConfChamberModel::ID).data().toString();

    QString in = QString("confrooms/%0/in/%1/").arg(roomId).arg(castId);

    if (!(static_cast<ConfChamberModel*>(model())->isAdmin() ||
          b_engine->eV(in + "user-id").toString() == b_engine->xivoUserId())) {
        return;
    }

    switch (col) {
        case ConfChamberModel::ACTION_MUTE:
            if (b_engine->eV(in + "mute").toBool()) {
                b_engine->meetmeAction("unmute", castId + " " + roomId);
            } else {
                b_engine->meetmeAction("mute", castId + " " + roomId);
            }
            break;
        case ConfChamberModel::ACTION_KICK:
            if (!b_engine->eV(in + "authed").toBool()) {
                b_engine->meetmeAction("MeetmeKick", castId + " " + roomId);
            } else {
                b_engine->meetmeAction("kick", castId + " " + roomId);
            }
            break;
        case ConfChamberModel::ACTION_TALK_TO:
            b_engine->meetmeAction("MeetmeTalk", castId + " " + roomId);
            break;
        case ConfChamberModel::ACTION_ALLOW_IN:
            b_engine->meetmeAction("MeetmeAccept", castId + " " + roomId);
            break;
        default:
            break;
    }
}

void ConfChamberView::mousePressEvent(QMouseEvent *event)
{
    lastPressed = event->button();
    QTableView::mousePressEvent(event);
}


ConfChamber::ConfChamber(const QString &id)
    : QWidget(), m_id(id)
{
    QVBoxLayout *vBox = new QVBoxLayout(this);
    setLayout(vBox);
    QHBoxLayout *hBox = new QHBoxLayout();
    ConfChamberModel *model = new ConfChamberModel(this, id);
    QPushButton *roomPause = new QPushButton(tr("&pause the conference"), this);
    QLabel *redondant = new QLabel(
        tr(" Conference room ") +
        b_engine->eV(QString("confrooms/%0/name").arg(id)).toString() + " (" +
        b_engine->eV(QString("confrooms/%0/number").arg(id)).toString() + ") "
        );



    roomPause->setProperty("state", true);
    hBox->addStretch(1);
    hBox->addWidget(redondant,6);
    hBox->addWidget(roomPause,2);
    hBox->addStretch(1);
    if (!model->isAdmin()) {
        roomPause->hide();
        hBox->setStretch(1, 8);
    }
    vBox->addLayout(hBox);

    hBox = new QHBoxLayout();
    connect(roomPause, SIGNAL(clicked()), this, SLOT(pauseConf()));

    ConfChamberView *view = new ConfChamberView(this, model);
    model->setView(view);


    view->setStyleSheet("ConfChamberView {"
                            "border: none;"
                            "background:transparent;"
                            "color:black;"
                        "}");

    view->verticalHeader()->hide();

    hBox->addStretch(1);
    hBox->addWidget(view, 8);
    hBox->addStretch(1);

    vBox->addLayout(hBox);

}

void ConfChamber::pauseConf()
{
    QPushButton *button = static_cast<QPushButton*>(sender());
    bool confPaused = button->property("state").toBool();

    if (confPaused) {
        button->setText(tr("&restart the conference"));
    } else {
        button->setText(tr("&pause the conference"));
    }
    button->setProperty("state", !confPaused);
    b_engine->meetmeAction("MeetmePause", m_id + " " + (confPaused? "on" : "off"));
}
