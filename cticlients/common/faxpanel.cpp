/* XiVO Client
 * Copyright (C) 2007-2009, Proformatique
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

#include <QDebug>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QVBoxLayout>

#include "baseengine.h"
#include "dirdialog.h"
#include "extendedlineedit.h"
#include "faxpanel.h"

FaxPanel::FaxPanel(BaseEngine * engine,
                   QWidget * parent)
    : QWidget(parent), m_mainwindow(parent), m_engine(engine)
{
    // qDebug() << "FaxPanel::FaxPanel()" << parent;
    Qt::CheckState previous_hide = (Qt::CheckState) m_engine->getSettings()->value("faxhistory/hidenumber", 0).toInt();

    QVBoxLayout * vlayout = new QVBoxLayout(this);

    //
    QGroupBox * groupBox1 = new QGroupBox( tr("1. Choose Destination Number") );
    groupBox1->setAlignment( Qt::AlignLeft );
    QHBoxLayout * hbox1 = new QHBoxLayout( groupBox1 );

    QLabel * lblfax = new QLabel(tr("Fax Number"), this);
    m_destination = new QLineEdit(this);
    QPushButton * directory = new QPushButton( tr("Directory"), this);
    connect(directory, SIGNAL(clicked()),
            this, SLOT(dirLookup()));
    connect(m_destination, SIGNAL(textChanged(const QString &)),
            this, SLOT(destNumberChanged(const QString &)));
    connect(m_destination, SIGNAL(selectionChanged()),
            this, SLOT(destSelectionChanged()));
    hbox1->addWidget(lblfax);
    hbox1->addWidget(m_destination);
    hbox1->addWidget(directory);
    lblfax->setObjectName("fax");
        
    //
    QGroupBox * groupBox2 = new QGroupBox( tr("2. Choose File to Send") );
    groupBox2->setAlignment( Qt::AlignLeft );
    QHBoxLayout * hbox2 = new QHBoxLayout( groupBox2 );
    m_openFileNameLabel = new FileNameLineEdit(this);
    connect(m_openFileNameLabel, SIGNAL(textChanged(const QString &)),
            this, SLOT(fileNameChanged(const QString &)));
    QPushButton * openFileNamesButton = new QPushButton( tr("Browse"), this);
    connect(openFileNamesButton, SIGNAL(clicked()),
            this, SLOT(setOpenFileName()));
    hbox2->addWidget(m_openFileNameLabel);
    hbox2->addWidget(openFileNamesButton);
        
    //
    QGroupBox * groupBox3 = new QGroupBox( tr("3. Hide the Number ?") );
    groupBox3->setAlignment( Qt::AlignLeft );
    QHBoxLayout * hbox3 = new QHBoxLayout( groupBox3 );

    m_maskornot = new QCheckBox(tr("Hide Number"), this);
    m_maskornot->setCheckState(previous_hide);
    hbox3->addStretch(1);
    hbox3->addWidget(m_maskornot);
    hbox3->addStretch(1);

    //
    QGroupBox * groupBox4 = new QGroupBox( tr("4. Send your File") );
    groupBox4->setAlignment( Qt::AlignLeft );
    QHBoxLayout * hbox4 = new QHBoxLayout( groupBox4 );

    m_sendButton = new QPushButton( tr("Send"), this);
    m_sendButton->setEnabled(false);
    connect(m_sendButton, SIGNAL(clicked()),
            this, SLOT(sendFax()));
    hbox4->addStretch(1);
    hbox4->addWidget(m_sendButton);
    hbox4->addStretch(1);

    vlayout->addWidget(groupBox1);
    vlayout->addWidget(groupBox2);
    vlayout->addWidget(groupBox3);
    vlayout->addWidget(groupBox4);
    vlayout->addStretch(1);

}


FaxPanel::~FaxPanel()
{
    // qDebug() << "FaxPanel::~FaxPanel()";
}

void FaxPanel::destSelectionChanged()
{
    if(m_destination->selectedText() == m_destination->text()) {
        QString dt = m_destination->text();
        dt.remove('.').remove(' ');
        if(m_destination->text() != dt)
            m_destination->setText(dt);
    }
}

void FaxPanel::destNumberChanged(const QString &/* ext*/)
{
    // qDebug() << "FaxPanel::destNumberChanged()" << ext;
    if ((! m_openFileNameLabel->text().isEmpty()) && (! m_destination->text().isEmpty())) {
        m_sendButton->setEnabled(true);
    } else {
        m_sendButton->setEnabled(false);
    }
}

void FaxPanel::fileNameChanged(const QString &)
{
    // qDebug() << "FaxPanel::fileNameChanged()" << ext;
    if ((! m_openFileNameLabel->text().isEmpty()) && (! m_destination->text().isEmpty())) {
        m_sendButton->setEnabled(true);
    } else {
        m_sendButton->setEnabled(false);
    }
}

void FaxPanel::setOpenFileName()
{
    // There has been an option set as QFileDialog::DontUseNativeDialog
    // previously, probably because of a too quick copy/paste from an example.
    // While on Linux platforms, it makes no difference, on MacOS it is worth
    // not setting it, in order for special places like "Volumes" to be seen.
    QString selectedFilter;
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Fax File"),
                                                    m_openFileNameLabel->text(),
                                                    tr("PDF Files (*.pdf);;All Files (*)"),
                                                    &selectedFilter);
    if (!fileName.isEmpty())
        m_openFileNameLabel->setText(fileName);
}


void FaxPanel::sendFax()
{
    m_engine->getSettings()->setValue("faxhistory/hidenumber", m_maskornot->checkState());
    if ((! m_openFileNameLabel->text().isEmpty()) && (! m_destination->text().isEmpty())) {
        // qDebug() << "FaxPanel::sendFax()"
        // << m_openFileNameLabel->text()
        // << m_destination->text()
        // << m_maskornot->checkState();
        // m_sendButton->setEnabled(false);
        m_dest_string = m_destination->text();
        m_file_string = m_openFileNameLabel->text();
        m_destination->setText("");
        m_openFileNameLabel->setText("");
        faxSend(m_file_string,
                m_dest_string,
                m_maskornot->checkState());
    }
}

void FaxPanel::dirLookup()
{
    // qDebug() << "FaxPanel::dirLookup()";
    m_dirw = new DirDialog(m_engine, m_mainwindow);
    m_dirw->exec();
    // qDebug() << "FaxPanel::dirLookup() DirDialog exec'ed";
    QString retstr = m_dirw->faxnumber();
    if(retstr.size() > 0)
        m_destination->setText(retstr);
    delete m_dirw;
}

void FaxPanel::popupMsg(const QString & status, const QString & reason)
{
    static QMessageBox msgbox;
    QMessageBox::Icon icon;
    QString text;
        
    // qDebug() << "FaxPanel::popupMsg()" << status << reason;
        
    if(status == "ok") {
        icon = QMessageBox::Information;
        text = tr("Your Fax (file %1)\n"
                  "was successfully sent to %2.").arg(m_file_string, m_dest_string);
    } else if (status == "queued") {
        icon = QMessageBox::Information;
        text = tr("Your Fax (file %1)\n"
                  "is being processed and will be sent soon.").arg(m_file_string);
    } else {
        QString faxreason;
        if(reason == "orig")
            faxreason = tr("Problem when dialing the number");
        else if(reason == "convert-pdftif")
            faxreason = tr("Unable to convert your PDF to TIFF");
        else if(reason == "filetype")
            faxreason = tr("Document not a PDF");
        else if(reason == "AMI")
            faxreason = tr("Dialog between CTI server and IPBX");
        else if(reason == "exists-pathspool")
            faxreason = tr("(Server) missing directory");
        else if(reason == "unknown")
            faxreason = tr("Unknown");
        icon = QMessageBox::Critical;
        text = tr("Your Fax (file %1)\n"
                  "was NOT sent to %2.\n"
                  "Reason given : %3.").arg(m_file_string, m_dest_string, faxreason);
        m_destination->setText(m_dest_string);
        m_openFileNameLabel->setText(m_file_string);
    }
    
    msgbox.setWindowTitle("XIVO CTI (Fax)");
    msgbox.setIcon(icon);
    msgbox.setText(text);
    msgbox.show();
}
