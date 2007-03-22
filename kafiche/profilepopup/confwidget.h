#ifndef __CONFWIDGET_H__
#define __CONFWIDGET_H__

#include <QWidget>
#include <QDialog>
#include <QLineEdit>
#include "engine.h"

class QSpinBox;
class QCheckBox;
class QComboBox;
class MainWidget;

/*! \brief Configuration Window
 *
 * This Widget enables the user to edit the connection
 * parameters to the identification server */
/* could be a QDialog instead of QWidget */
//class ConfWidget: public QWidget
class ConfWidget: public QDialog
{
	Q_OBJECT
public:
	/*! \brief Constructor
	 *
	 * Construct the widget and its layout.
	 * Fill widgets with values got from the Engine object.
	 * Once constructed, the Widget is ready to be shown.
	 * \param engine	related Engine object where parameters will be modified
	 * \param parent	parent QWidget
	 */
	ConfWidget(Engine *engine, MainWidget *parent);
	//ConfWidget(Engine *engine, QWidget *parent = 0);
private slots:
	//! Save the configuration to the Engine object and close
	void saveAndClose();
private:
	QLineEdit *m_lineip;		//!< IP/hostname of the server
	QLineEdit *m_lineport;		//!< port of the server
	QComboBox *m_protocombo;	//!< Protocol SIP/IAX
	QLineEdit *m_linelogin;		//!< user login
	QLineEdit *m_linepasswd;	//!< user password
	QCheckBox *m_autoconnect;	//!< Auto connect checkbox
	QSpinBox *m_kainterval_sbox;	//!< Keep alive interval
	QCheckBox *m_trytoreconnect;	//!< "Try to reconnect" Checkbox
	QSpinBox *m_tryinterval_sbox;	//!< "Try to reconnect" interval
	QSpinBox *m_tablimit_sbox;	//!< Maximum number of tabs
	Engine *m_engine;			//!< Engine object parameters are commited to
	MainWidget *m_mainwidget;	//!< MainWidget where some parameters are commited to
};

#endif

