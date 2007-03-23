#ifndef __SWITCHBOARDCONF_H__
#define __SWITCHBOARDCONF_H__

#include <QDialog>

class SwitchBoardEngine;
class QLineEdit;

class SwitchBoardConfDialog : public QDialog
{
	Q_OBJECT
public:
	SwitchBoardConfDialog(SwitchBoardEngine * engine, QWidget *parent = 0);
private slots:
	void saveAndClose();
private:
	SwitchBoardEngine * m_engine;
	QLineEdit * m_host;
	QLineEdit * m_port;
};

#endif

