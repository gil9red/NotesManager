/*************************************************************************
**
** Copyright (C) 2013-2014 by Ilya Petrash
** All rights reserved.
** Contact: gil9red@gmail.com, ip1992@inbox.ru
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

#include "page_notes.h"
#include "ui_page_notes.h"

#include <QDir>
#include <QDateTime>
#include <QMessageBox>
#include <QButtonGroup>
#include <QDockWidget>

Page_Notes::Page_Notes( QWidget * parent, QAbstractItemModel * m ) :
    QMainWindow( parent ),
    ui( new Ui::Page_notes ),
    model ( m ),
    settings( 0 )
{
    ui->setupUi( this );
    ui->tableView->setModel( model );
    ui->tableView->horizontalHeader()->setMovable( false );

    selectionModel = new QItemSelectionModel( model );
    ui->tableView->setSelectionModel( selectionModel );

    connect( selectionModel, SIGNAL( selectionChanged(QItemSelection,QItemSelection) ), SLOT( selectionChanged(QItemSelection,QItemSelection) ) );
}
Page_Notes::~Page_Notes()
{
    delete ui;
}

void Page_Notes::sort( int column, Qt::SortOrder sort )
{
    ui->tableView->sortByColumn( column, sort );
}
int Page_Notes::sortColumn()
{
    return ui->tableView->horizontalHeader()->sortIndicatorSection();
}
Qt::SortOrder Page_Notes::sortOrder()
{
    return ui->tableView->horizontalHeader()->sortIndicatorOrder();
}
QModelIndex Page_Notes::currentIndex() const
{
    return selectionModel->currentIndex();
}
int Page_Notes::currentRow()
{
    return selectionModel->currentIndex().row();
}
bool Page_Notes::hasSelection()
{
    return selectionModel->hasSelection();
}
void Page_Notes::setSettings( QSettings * s )
{
    settings = s;
}
QAbstractItemModel * Page_Notes::getModel()
{
    return model;
}

void Page_Notes::readSettings()
{
    if ( !settings )
        return;

    settings->beginGroup( "Page_Notes" );
    ui->tableView->horizontalHeader()->restoreState( settings->value( "HorizontalHeader" ).toByteArray() );
    ui->splitter->restoreState( settings->value( "Splitter_Main" ).toByteArray() );
    settings->endGroup();
}
void Page_Notes::writeSettings()
{
    if ( !settings )
        return;

    settings->beginGroup( "Page_Notes" );
    settings->setValue( "HorizontalHeader", ui->tableView->horizontalHeader()->saveState() );
    settings->setValue( "Splitter_Main", ui->splitter->saveState() );
    settings->endGroup();
    settings->sync();
}
void Page_Notes::selectionChanged(QItemSelection,QItemSelection)
{
    emit changeSelection();
}
