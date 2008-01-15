/* XIVO CTI clients
Copyright (C) 2007, 2008  Proformatique

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/

/* $Revision$
 * $Date$
 */

#include <QComboBox>
#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QRegExp>
#include <QScrollArea>
#include <QToolButton>

#include "dialpanel.h"
#include "xivoconsts.h"

/*! \brief Constructor
 */
DialPanel::DialPanel(QWidget * parent)
        : QWidget(parent)
{
	QHBoxLayout * vlayout = new QHBoxLayout(this);
	vlayout->setMargin(0);
        m_lbl = new QLabel( tr("Enter &Number :"), this );
	m_input = new QComboBox( this );
	m_lbl->setBuddy(m_input);
	m_input->setStatusTip( tr("Input here the phone number to dial") );
	m_input->setEditable( true );
        m_input->setDuplicatesEnabled( false );
        m_input->setInsertPolicy( QComboBox::InsertAlphabetically );
	m_input->setMinimumContentsLength( 15 );
	//m_input->setSizeAdjustPolicy( QComboBox::AdjustToContents );
	m_input->setInsertPolicy( QComboBox::NoInsert );
	connect( m_input->lineEdit(), SIGNAL(returnPressed()),
	         this, SLOT(inputValidated()) );
	connect( m_input, SIGNAL(editTextChanged(const QString &)),
	         this, SIGNAL(textEdited(const QString &)) );
        QPixmap pm = QPixmap(":/images/sipphone.png");
        QToolButton * dialButton = new QToolButton(this);
        dialButton->setIcon(pm);
        dialButton->setIconSize(pm.size());

	connect( dialButton, SIGNAL(clicked()),
	         this, SLOT(inputValidated()) );

        setAcceptDrops(true);

        vlayout->addStretch(1);
	vlayout->addWidget( m_lbl, 0, Qt::AlignCenter );
	vlayout->addWidget( m_input, 0, Qt::AlignCenter );
	vlayout->addWidget( dialButton, 0, Qt::AlignCenter );
        vlayout->addStretch(1);
}

void DialPanel::setNumberToDial(const QString & text)
{
        // qDebug() << "DialPanel::setNumberToDial()" << text;
        // adds the item to the list
        if (m_input->findText(text) == -1)
                m_input->insertItem(0, text);
        // displays it
        m_input->lineEdit()->setText(text);
}

void DialPanel::dragEnterEvent(QDragEnterEvent * event)
{
        // qDebug() << "DialPanel::dragEnterEvent()" << event;
	if(event->mimeData()->hasFormat(PEER_MIMETYPE))
                event->acceptProposedAction();
}

void DialPanel::dropEvent(QDropEvent * event)
{
	QString ext;
	QString originator = event->mimeData()->text();
	if(m_input->lineEdit()) {
                qDebug() << "DialPanel::dropEvent()" << event << originator << m_input->lineEdit()->text();
                ext = m_input->lineEdit()->text();
                ext.remove(QRegExp("[\\s\\.]")); // remove spaces and full stop characters
                if(ext.length() == 0)	// do nothing if the string is empty
                        return;
                originateCall(originator, m_input->lineEdit()->text());
                m_input->insertItem(0, ext); // ajout à l'historique
                // supprimer les occurences les plus anciennes du meme numero
                for(int i=1; i<m_input->count(); ) {
                        if(ext == m_input->itemText(i)) {
                                m_input->removeItem(i);
                        }
                        else
                                i++;
                }
                m_input->clearEditText();
        }
}

// void DialPanel::mouseMoveEvent(QMouseEvent *event)
// {
// 	qDebug() << "DialPanel::mouseMoveEvent()" << event;
// }

// void DialPanel::mousePressEvent(QMouseEvent *event)
// {
// 	qDebug() << "DialPanel::mousePressEvent()" << event;
// }

/*! \brief the input was validated
 *
 * check the input and call emitDial() if ok.
 */
void DialPanel::inputValidated()
{
	QString ext;
	if(m_input->lineEdit())
	{
		ext = m_input->lineEdit()->text();
                ext.remove(QRegExp("[\\s\\.]")); // remove spaces and full stop characters
                if(ext.length() == 0)	// do nothing if the string is empty
                        return;
                emitDial( ext );
                m_input->insertItem(0, ext); // ajout à l'historique
                // supprimer les occurences les plus anciennes du meme numero
                for(int i=1; i<m_input->count(); ) {
                        if(ext == m_input->itemText(i)) {
                                m_input->removeItem(i);
                        }
                        else
                                i++;
                }
                m_input->clearEditText();
        }
}

