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

#include "Manager.h"
#include "ui_Manager.h"

#include <QStandardItem>
#include <QFileInfo>
#include <QClipboard>
#include <QDesktopWidget>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>

Manager * Manager::self = 0;

#include "FormattingToolbar/FormattingToolbar.h"
#include "utils/func.h"
#include "ImageCropper/fullscreenshotcropper.h"

Manager::Manager( QWidget * parent ) :
    QMainWindow( parent ),
    ui( new Ui::Manager ),
    settings( 0 )
{
    self = this;       
    ui->setupUi( this );

    leftPanel = new FancyTabBar( TabBarPosition::Left );
    leftPanel->addTab( QIcon( ":/notebook" ), tr( "Notes" ) );
    leftPanel->addTab( QIcon( ":/settings" ), tr( "Settings" ) );
    leftPanel->addTab( QIcon( ":/about" ),    tr( "About" ) );
    leftPanel->addTab( QIcon( ":/quit" ),     tr( "Quit" ) );
    leftPanel->setCurrentIndex( 0 );
    connect( leftPanel, SIGNAL( currentChanged(int) ), SLOT( buttonLeftPanelClicked(int) ) );

    ui->panel->setLayout( new QVBoxLayout() );
    ui->panel->layout()->setMargin( 0 );
    ui->panel->layout()->addWidget( leftPanel );
    ui->panel->setSizePolicy( QSizePolicy::Fixed, ui->panel->sizePolicy().verticalPolicy() );

    pageNotes = new Page_Notes( this );
    pageSettings = new Page_Settings();
    pageAbout = new Page_About();

    connect( pageSettings, SIGNAL( message(QString, int) ), ui->statusBar, SLOT( showMessage(QString, int) ) );
    connect( pageSettings, SIGNAL( acceptChangeSettings() ), SLOT( acceptChangeSettings() ) );

    ui->stackedWidget_Pages->addWidget( pageNotes );
    ui->stackedWidget_Pages->addWidget( pageSettings );
    ui->stackedWidget_Pages->addWidget( pageAbout );
    ui->stackedWidget_Pages->setCurrentIndex( leftPanel->currentIndex() );

    createMenu();
    createToolBars();
    createTray();

    connect( &autoSaveTimer, SIGNAL( timeout() ), SLOT( writeSettings() ) );
}

Manager::~Manager()
{
    delete ui;
    self = 0;
}

void Manager::loadNotes()
{    
    const QString & fileName = qApp->applicationDirPath() + "/notes/notebook.xml";
    QFile file( fileName );
    if ( !file.open( QFile::ReadOnly | QFile::Text ) )
    {
        QMessageBox::warning( this, tr( "Error" ), tr( "Cannot read file %1:\n%2." ).arg( fileName ).arg( file.errorString() ) );
        return;
    }
    pageNotes->read( &file );
}
void Manager::setSettings( QSettings * s )
{
    settings = s;
    pageNotes->settings = settings;
    pageSettings->setSettings( settings );
}
void Manager::nowReadyPhase()
{
    bool minimizeTrayOnStartup = pageSettings->mapSettings[ "MinimizeTrayOnStartup" ].toBool();
    if ( minimizeTrayOnStartup )
        hide();
    else
        show_Manager();
}

void Manager::createToolBars()
{
    Qt::ToolBarAreas allowedAreas = Qt::RightToolBarArea | Qt::TopToolBarArea;


    QToolBar * toolbarMain = addToolBar( ui->dockMain->windowTitle() );
    toolbarMain->setObjectName( ui->dockMain->windowTitle() );
    toolbarMain->setAllowedAreas( allowedAreas );

    QAction * actionMain = ui->menuToolbars->addAction( QIcon( "" ), ui->dockMain->windowTitle() );
    actionMain->setCheckable( true );
    actionMain->setChecked( toolbarMain->isVisible() );

    connect( actionMain, SIGNAL( triggered(bool) ), toolbarMain, SLOT( setVisible(bool) ) );
    connect( toolbarMain, SIGNAL( visibilityChanged(bool) ), actionMain, SLOT( setChecked(bool) ) );

    addToolBar( toolbarMain );

    toolbarMain->addSeparator();
    toolbarMain->addWidget( ui->tButtonSettings );
    toolbarMain->addWidget( ui->tButtonAbout );
    toolbarMain->addWidget( ui->tButtonQuit );

    // Эти виджеты уже больше не нужны, поэтому нужно удалить их
    delete ui->dockMain;

    connect( ui->tButtonAbout, SIGNAL( clicked() ), SLOT( show_page_about() ) );
    connect( ui->tButtonQuit, SIGNAL( clicked() ), SLOT( quit() ) );
    connect( ui->tButtonSettings, SIGNAL( clicked() ), SLOT( show_page_settings() ) );
}
void Manager::createMenu()
{
    connect( ui->actionAbout, SIGNAL( triggered() ), SLOT( show_page_about() ) );
    connect( ui->actionDocumentation, SIGNAL( triggered() ), SLOT( show_page_documentation() ) );
    connect( ui->actionQuit, SIGNAL( triggered() ), SLOT( quit() ) );
    connect( ui->actionSettings, SIGNAL( triggered() ), SLOT( show_page_settings() ) );
}
void Manager::createTray()
{
    tray.setIcon( QIcon( ":/icon-mini" ) );
    tray.setToolTip( QString( "%1 %2\n%3" ).arg( qApp->applicationName() ).arg( qApp->applicationVersion() ).arg( tr( "Program to create notes" ) ) );

    QMenu * trayMenu = new QMenu();
    tray.setContextMenu( trayMenu );

    connect( &tray, SIGNAL( activated(QSystemTrayIcon::ActivationReason) ), SLOT( messageReceived(QSystemTrayIcon::ActivationReason) ) );
    tray.show();

    actionOpenManager = Create::Action::triggered( QIcon( "" ), tr( "Open manager" ), this, SLOT( show_Manager() ) );
    actionSettings = Create::Action::triggered( QIcon( ":/settings" ), ui->tButtonSettings->text(), this, SLOT( show_page_settings() ) );
    actionAbout = Create::Action::triggered( QIcon( ":/about" ), ui->tButtonAbout->text(), this, SLOT( show_page_about() ) );
    actionQuit = Create::Action::triggered( QIcon( ":/quit" ), ui->tButtonQuit->text(), this, SLOT( quit() ) );

    trayMenu->addAction( actionOpenManager );
    trayMenu->addSeparator();
    trayMenu->addAction( actionSettings );
    trayMenu->addSeparator();
    trayMenu->addAction( actionAbout );
    trayMenu->addAction( actionQuit );
}

void Manager::buttonLeftPanelClicked( int index )
{
    switch ( index )
    {
    case 0:
        ui->stackedWidget_Pages->setCurrentWidget( pageNotes );
        break;

    case 1:
        ui->stackedWidget_Pages->setCurrentWidget( pageSettings );
        break;

    case 2:
        ui->stackedWidget_Pages->setCurrentWidget( pageAbout );
        pageAbout->setCurrentTab( Page_About::About );
        break;

    case 3:
        quit();
        break;
    }
}
void Manager::messageReceived( const QString & text )
{
    foreach ( const QString & command, text.split( " " ) )
    {
        if ( command == "-show" )
            show_Manager();

        else if ( command == "-message" )
            tray.showMessage( tr( "Information" ), tr( "Application is already running" ), QSystemTrayIcon::Information, 5000 );
    }
}
void Manager::messageReceived( QSystemTrayIcon::ActivationReason reason )
{
    if ( reason == QSystemTrayIcon::Trigger )
    {
        if ( isVisible() )
            hide();
        else
            show_Manager();

    }
}

void Manager::acceptChangeSettings()
{
    setActivateTimerAutosave( pageSettings->mapSettings[ "Autosave" ].toBool() );
    setIntervalAutosave( pageSettings->mapSettings[ "AutosaveInterval" ].toInt() );
}

void Manager::show_page_notes()
{
    show_Manager();
    leftPanel->setCurrentIndex( 0 );
}
void Manager::show_page_settings()
{
    show_Manager();
    leftPanel->setCurrentIndex( 1 );
}
void Manager::show_page_about()
{
    show_Manager();
    leftPanel->setCurrentIndex( 2 );
}
void Manager::show_page_documentation()
{
    if ( !QDesktopServices::openUrl( QUrl::fromLocalFile( qApp->applicationDirPath() + "/doc/ru/html/index.html" ) ) )
    {
        QMessageBox::information( this, tr( "Information" ), tr( "Unable to open documents" ) );
        qDebug() << tr( "Unable to open documents" );
    }
}

void Manager::show_Manager()
{
    if ( isHidden() )
        QTimer::singleShot( 0, this, SLOT( show() ) );

    if ( isMinimized() )
        QTimer::singleShot( 0, this, SLOT( showNormal() ) );

    if ( !hasFocus() )
        setFocus();

    if ( !isActiveWindow() )
        activateWindow();
}
void Manager::quit()
{
    bool askBeforeExiting = pageSettings->mapSettings[ "AskBeforeExiting" ].toBool();

    if ( askBeforeExiting )
    {
        QMessageBox::StandardButton result = showNewMessageBox( 0, QMessageBox::Question, tr( "Question" ), tr( "Really quit?" ), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok );
        if ( result == QMessageBox::Cancel )
            return;
    }

    writeSettings();
    qApp->quit();
}

void Manager::readSettings()
{
    if ( !settings )
        return;

    settings->beginGroup( "Manager" );
    restoreGeometry( settings->value( "Geometry" ).toByteArray() );
    restoreState( settings->value( "State" ).toByteArray() );

    settings->endGroup();

    pageNotes->readSettings();
    pageSettings->readSettings();
    acceptChangeSettings();
}
void Manager::writeSettings()
{
    if ( !settings )
        return;

    settings->beginGroup( "Manager" );
    settings->setValue( "Geometry", saveGeometry() );
    settings->setValue( "State", saveState() );
    settings->endGroup();

    pageNotes->writeSettings();
    pageSettings->writeSettings();

    settings->sync();

    ui->statusBar->showMessage( tr( "Save completed" ), 5000 );


    const QString & fileName = qApp->applicationDirPath() + "/notes/notebook.xml";
    QFile file( fileName );
    if ( !file.open( QFile::WriteOnly | QFile::Text ) )
    {
        QMessageBox::warning( this, tr( "Error" ), tr( "Cannot write file %1:\n%2." ).arg( fileName ).arg( file.errorString() ) );
        return;
    }
    pageNotes->write( &file );
}

void Manager::setActivateTimerAutosave( bool activate )
{
    if ( activate )
        autoSaveTimer.start();
    else
        autoSaveTimer.stop();
}
bool Manager::isActivateTimerAutosave()
{
    return autoSaveTimer.isActive();
}
void Manager::setIntervalAutosave( quint64 minutes )
{
    autoSaveTimer.setInterval( 1000 * 60 * minutes );
}
int Manager::intervalAutosave()
{
    return autoSaveTimer.interval() / ( 1000 * 60 );
}

void Manager::closeEvent( QCloseEvent * event )
{
    event->ignore();

    bool minimizeTrayWhenClosing = pageSettings->mapSettings[ "MinimizeTrayWhenClosing" ].toBool();
    if ( minimizeTrayWhenClosing )
        hide();
    else
        quit();
}
void Manager::changeEvent( QEvent * event )
{
    QEvent::Type type = event->type();

    if ( type == QEvent::WindowStateChange )
    {
        bool minimizeTrayWhenMinimizing = pageSettings->mapSettings[ "MinimizeTrayWhenMinimizing" ].toBool();
        if ( isMinimized() && minimizeTrayWhenMinimizing )
        {
            event->ignore();
            QTimer::singleShot( 0, this, SLOT( hide() ) );
            return;
        }
    }

    QMainWindow::changeEvent( event );
}
