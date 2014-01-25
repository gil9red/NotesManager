/*************************************************************************
**
** Copyright (C) 2013-2014 by Ilya Petrash
** All rights reserved.
** Contact: gil9red@gmail.com
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the
** Free Software Foundation, Inc.,
** 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
**
**************************************************************************/


#include "page_about.h"
#include "ui_page_about.h"

#include "version.h"
#include <QFile>
#include <QTextStream>
#include <QDesktopServices>

Page_About::Page_About(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Page_About)
{
    ui->setupUi( this );

    ui->labelName->setText( ui->labelName->text().arg( App::name ) );
    ui->labelVersion->setText( ui->labelVersion->text().arg( App::version ) );
    ui->labelDescription->setText( ui->labelDescription->text().arg( tr( "The program creates notes" ) ) );
    ui->labelCopyright->setText( ui->labelCopyright->text().arg( tr( "All rights reserved." ) ) );
    ui->labelTextVersion->setText( ui->labelTextVersion->text().arg( tr( "Version" ) ) );
    ui->labelTextAuthor->setText( ui->labelTextAuthor->text().arg( tr( "Author" ) ) );
    ui->labelTextMail->setText( ui->labelTextMail->text().arg( tr( "Mail" ) ) );
    ui->labelSourceCode->setText( ui->labelSourceCode->text().arg( tr( "Source code" ) ) );

    QFile file;
    QTextStream in( &file );
    in.setCodec( "utf8" );

    file.setFileName( ":/license" );
    if ( !file.open( QIODevice::ReadOnly ) )
        return;
    ui->licenseText->setHtml( in.readAll() );
    file.close();
    in.flush();

    file.setFileName( ":/history" );
    if ( !file.open( QIODevice::ReadOnly ) )
        return;
    ui->history->setHtml( in.readAll() );
    file.close();

    QUrl urlDoc( QUrl::fromLocalFile( qApp->applicationDirPath() + "/doc/ru/html/index.html" ) );
    ui->webView->load( urlDoc );
}

Page_About::~Page_About()
{
    delete ui;
}

void Page_About::on_tButtonShowDocInBrowser_clicked()
{
    QDesktopServices::openUrl( ui->webView->url() );
}

void Page_About::setCurrentTab( int index )
{
    ui->tabWidget->setCurrentIndex( index );
}
int Page_About::currentTab()
{
    return ui->tabWidget->currentIndex();
}
