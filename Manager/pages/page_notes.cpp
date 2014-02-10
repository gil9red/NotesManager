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

#include "page_notes.h"
#include "ui_page_notes.h"

#include <QDir>
#include <QDateTime>
#include <QMessageBox>
#include <QButtonGroup>
#include <QDockWidget>

//#include "Manager/Manager.h"


Page_Notes::Page_Notes( QWidget * parent, QAbstractItemModel * m ) :
    QMainWindow( parent ),
    ui( new Ui::Page_notes ),
    model ( m ),
    settings( 0 )
{
    ui->setupUi( this );
    ui->tableView->setModel( model );
    ui->tableView->horizontalHeader()->setMovable( false );
//    connect( ui->tableView, SIGNAL( clicked(QModelIndex) ), SLOT( clicked(QModelIndex) ) );

    selectionModel = new QItemSelectionModel( model );
    ui->tableView->setSelectionModel( selectionModel );

    ui->frameCurrentNote->hide();
    ui->attach->hide();

    connect( selectionModel, SIGNAL( selectionChanged(QItemSelection,QItemSelection) ), SLOT( selectionChanged(QItemSelection,QItemSelection) ) );
//    updateStates();
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

//void Page_Notes::displayContentNote( int row )
//{
//    if ( row < 0 )
//        return;

//    RichTextNote * note = toNote( model->index( row, 0 ) );
//    if ( !note )
//        return;

//    clearContentNote();

//    connect( note, SIGNAL( changed(int) ), SLOT( noteChange(int) ) );
//    ui->title->setText( note->title() );
//    ui->title->setFont( note->titleFont() );
//    ui->title->setCursorPosition( 0 );
//    ui->editor->setDocument( note->document() );

//    ui->attach->setViewTo( note );
//}
//void Page_Notes::clearContentNote()
//{
//    ui->title->clear();
//    ui->editor->setDocument( 0 );
//    ui->attach->setViewTo( 0 );
//}
void Page_Notes::setSettings( QSettings * s )
{
    settings = s;
}
//QTextEdit * Page_Notes::editor()
//{
//    return ui->editor;
//}
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
    ui->splitterMain->restoreState( settings->value( "Splitter_Main" ).toByteArray() );
//    ui->splitterPageNote_Right->restoreState( settings->value( "SplitterPageNote_Right" ).toByteArray() );
    settings->endGroup();
}
void Page_Notes::writeSettings()
{
    if ( !settings )
        return;

    settings->beginGroup( "Page_Notes" );
    settings->setValue( "HorizontalHeader", ui->tableView->horizontalHeader()->saveState() );
    settings->setValue( "Splitter_Main", ui->splitterMain->saveState() );
//    settings->setValue( "SplitterPageNote_Right", ui->splitterPageNote_Right->saveState() );
    settings->endGroup();
    settings->sync();
}

//void Page_Notes::clicked( const QModelIndex & index )
//{
//    displayContentNote( index.row() );
//}
//void Page_Notes::updateStates()
//{
//    // сымитируем изменение выделения
//    selectionChanged( QItemSelection(), QItemSelection() );
//}
void Page_Notes::selectionChanged(QItemSelection,QItemSelection)
{
//    bool itemHasSelection = hasSelection();

//    ui->frameCurrentNote->setVisible( itemHasSelection );
//    ui->attach->setVisible( itemHasSelection );

    emit changeSelection();
}

//void Page_Notes::noteChange( int index )
//{
//    RichTextNote * note = qobject_cast < RichTextNote * > ( sender() );

//    if ( !note )
//        return;

//    switch ( index )
//    {
//    case EventsNote::ChangeTitle:
//        ui->title->setText( note->title() );
//        break;

//    case EventsNote::ChangeFontTitle:
//        ui->title->setFont( note->titleFont() );
//        break;
//    }
//}
