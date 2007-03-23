#ifndef __SWITCHBOARDCONF_H__
#define __SWITCHBOARDCONF_H__

#include <QDialog>

class SwitchBoardEngine;
class SwitchBoardWindow;
class QLineEdit;
class QSpinBox;

class SwitchBoardConfDialog : public QDialog
{
	Q_OBJECT
public:
	SwitchBoardConfDialog(SwitchBoardEngine * engine,
	                      SwitchBoardWindow * window, QWidget *parent = 0);
private slots:
	void saveAndClose();
private:
	SwitchBoardEngine * m_engine;
	SwitchBoardWindow * m_window;
	QLineEdit * m_host;
	QLineEdit * m_port;
	QSpinBox * m_widthsb;
};

#endif

