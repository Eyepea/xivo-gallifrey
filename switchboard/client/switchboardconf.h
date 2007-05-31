#ifndef __SWITCHBOARDCONF_H__
#define __SWITCHBOARDCONF_H__

#include <QDialog>

class SwitchBoardEngine;
class SwitchBoardWindow;
class QLineEdit;
class QSpinBox;
class QCheckBox;
class QComboBox;

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
	QLineEdit * m_serverhost;
	QLineEdit * m_sbport;
	QLineEdit * m_loginport;
	QSpinBox * m_widthsb;
	QCheckBox * m_autoconnect;
	QLineEdit * m_asterisk;
	QComboBox * m_protocombo;
	QLineEdit * m_ext;
	QLineEdit * m_passwd;
	//	QLineEdit * m_context;
	QLineEdit * m_availstate;
};

#endif

