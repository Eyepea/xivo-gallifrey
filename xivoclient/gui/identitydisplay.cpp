/*
XIVO switchboard : 
Copyright (C) 2007  Proformatique

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

/* $Id$
   $Date$
*/

#include <QComboBox>
#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QSizePolicy>
#include <QPushButton>
#include <QRegExp>
#include <QScrollArea>
#include "identitydisplay.h"
#include "xivoconsts.h"

/*! \brief Constructor
 */
IdentityDisplay::IdentityDisplay(QWidget * parent)
        : QWidget(parent)
{
	QGridLayout * glayout = new QGridLayout(this);
        this->setStyleSheet("* {background : #fff0e0}");
	// glayout->setMargin(0);
        //        m_lbl = new QLabel( tr("Logged in User :"), this );
	m_user = new XivoLabel( "", this );
        m_user->setStyleSheet("* {border: 3px solid #ffe0b0; border-radius: 5px; font-size: 20px}");
        m_user->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        m_user->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        qDebug() << "IdentityDisplay::IdentityDisplay() : label" << m_user->geometry() << m_user->sizeHint();

        //	glayout->addWidget( m_lbl,  0, 0, Qt::AlignCenter );
	glayout->addWidget( m_user, 0, 0, Qt::AlignCenter );
	glayout->setColumnStretch( 0, 1 );
	glayout->setRowStretch( 0, 1 );
        qDebug() << "layout" << glayout->geometry();
}


/*! \brief the input was validated
 *
 * check the input and call emitDial() if ok.
 */
void IdentityDisplay::setUser(const QString & user)
{
        //        m_user->setText(user);
        m_user->setText(user);
        qDebug() << "IdentityDisplay::IdentityDisplay() : label" << m_user->geometry() << m_user->sizeHint();
}




XivoLabel::XivoLabel(const QString &text, QWidget *parent)
        : QLabel(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setText(text);
}

QSize XivoLabel::sizeHint() const
{
        QSize size = QLabel::sizeHint();
        size.rheight() = 40;
        size.rwidth() = 3000;
        return size;
}
