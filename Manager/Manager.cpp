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

    ui->leftPanel->addTab( QIcon( ":/notebook" ), tr( "Notes" ) );
    ui->leftPanel->addTab( QIcon( ":/settings" ), tr( "Settings" ) );
    ui->leftPanel->addTab( QIcon( ":/about" ),    tr( "About" ) );
    ui->leftPanel->addTab( QIcon( ":/quit" ),     tr( "Quit" ) );
    ui->leftPanel->setCurrentIndex( 0 );
    connect( ui->leftPanel, SIGNAL( currentChanged(int) ), SLOT( buttonLeftPanelClicked(int) ) );


    pageNotes = new Page_Notes( this );
    pageSettings = new Page_Settings();
    pageAbout = new Page_About();

    connect( pageSettings, SIGNAL( message(QString, int) ), ui->statusBar, SLOT( showMessage(QString, int) ) );
    connect( pageSettings, SIGNAL( acceptChangeSettings() ), SLOT( acceptChangeSettings() ) );

    ui->stackedWidget_Pages->addWidget( pageNotes );
    ui->stackedWidget_Pages->addWidget( pageSettings );
    ui->stackedWidget_Pages->addWidget( pageAbout );
    ui->stackedWidget_Pages->setCurrentIndex( ui->leftPanel->currentIndex() );

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

    toolbarMain->addWidget( ui->tButtonAddNote );
    toolbarMain->addWidget( ui->tButtonAddNoteFromClipboard );
    toolbarMain->addWidget( ui->tButtonAddNoteFromScreen );

    if ( Completer::instance()->count() )
    {
        toolbarMain->addSeparator();
        toolbarMain->addWidget( ui->tButtonOpenDict );
        toolbarMain->addWidget( ui->tButtonCloseDict );
    }
    toolbarMain->addSeparator();
    toolbarMain->addWidget( ui->tButtonDuplicateNote );
    toolbarMain->addWidget( ui->tButtonRemoveNote );
    toolbarMain->addWidget( ui->tButtonRemoveAllNotes );
    toolbarMain->addWidget( ui->tButtonSaveAllNotes );
    toolbarMain->addSeparator();
    toolbarMain->addWidget( ui->tButtonSettings );
    toolbarMain->addWidget( ui->tButtonAbout );
    toolbarMain->addWidget( ui->tButtonQuit );


    QToolBar * toolbarNote = addToolBar( ui->dockNote->windowTitle() );
    toolbarNote->setObjectName( ui->dockNote->windowTitle() );
    toolbarNote->setAllowedAreas( allowedAreas );

    QAction * actionNote = ui->menuToolbars->addAction( QIcon( "" ), ui->dockNote->windowTitle() );
    actionNote->setCheckable( true );
    actionNote->setChecked( toolbarNote->isVisible() );

    connect( actionNote, SIGNAL( triggered(bool) ), toolbarNote, SLOT( setVisible(bool) ) );
    connect( toolbarNote, SIGNAL( visibilityChanged(bool) ), actionNote, SLOT( setChecked(bool) ) );

    addToolBar( toolbarNote );

    toolbarNote->addWidget( ui->tButtonSaveNote );
    toolbarNote->addWidget( ui->tButtonSaveNoteAs );
    toolbarNote->addSeparator();
    toolbarNote->addWidget( ui->tButtonShowNote );
    toolbarNote->addWidget( ui->tButtonHideNote );
    toolbarMain->addWidget( ui->tButtonShowAllNotes );
    toolbarMain->addWidget( ui->tButtonHideAllNotes );
    toolbarNote->addSeparator();
    toolbarNote->addWidget( ui->tButtonPrintNote );
    toolbarNote->addWidget( ui->tButtonPreviewPrintNote );
    toolbarNote->addSeparator();
    toolbarNote->addWidget( ui->tButtonTopNote );

    // Эти виджеты уже больше не нужны, поэтому нужно удалить их
    delete ui->dockMain;
    delete ui->dockNote;

    connect( ui->tButtonAbout, SIGNAL( clicked() ), SLOT( show_page_about() ) );
    connect( ui->tButtonAddNote, SIGNAL( clicked() ), SLOT( addNote() ) );
    connect( ui->tButtonAddNoteFromClipboard, SIGNAL( clicked() ), SLOT( addNoteFromClipboard() ) );
    connect( ui->tButtonAddNoteFromScreen, SIGNAL( clicked() ), SLOT( addNoteFromScreen() ) );
    connect( ui->tButtonRemoveAllNotes, SIGNAL( clicked() ), SLOT( removeAllNotes() ) );
    connect( ui->tButtonSaveAllNotes, SIGNAL( clicked() ), SLOT( saveAllNotes() ) );
    connect( ui->tButtonShowAllNotes, SIGNAL( clicked() ), SLOT( showAllNotes() ) );
    connect( ui->tButtonHideAllNotes, SIGNAL( clicked() ), SLOT( hideAllNotes() ) );
    connect( ui->tButtonQuit, SIGNAL( clicked() ), SLOT( quit() ) );
    connect( ui->tButtonSettings, SIGNAL( clicked() ), SLOT( show_page_settings() ) );
    connect( ui->tButtonOpenDict, SIGNAL( clicked() ), SLOT( openDictionary() ) );
    connect( ui->tButtonCloseDict, SIGNAL( clicked() ), SLOT( closeDictionary() ) );

    connect( ui->tButtonRemoveNote, SIGNAL( clicked() ), SLOT( removeNote() ) );
    connect( ui->tButtonDuplicateNote, SIGNAL( clicked() ), SLOT( duplicateNote() ) );
    connect( ui->tButtonSaveNoteAs, SIGNAL( clicked() ), SLOT( saveAsNote() ) );
    connect( ui->tButtonSaveNote, SIGNAL( clicked() ), SLOT( saveNote() ) );
    connect( ui->tButtonShowNote, SIGNAL( clicked() ), SLOT( showNote() ) );
    connect( ui->tButtonHideNote, SIGNAL( clicked() ), SLOT( hideNote() ) );
    connect( ui->tButtonPrintNote, SIGNAL( clicked() ), SLOT( printNote() ) );
    connect( ui->tButtonPreviewPrintNote, SIGNAL( clicked() ), SLOT( previewPrintNote() ) );
    connect( ui->tButtonTopNote, SIGNAL( clicked(bool) ), SLOT( setTopNote(bool) ) );
}
void Manager::createMenu()
{
    // если словарь пуст
    if ( !Completer::instance()->count() )
    {
        ui->actionOpenDict->setVisible( false );
        ui->actionCloseDict->setVisible( false );
    }

    connect( ui->actionVisibleLeftPanel, SIGNAL( triggered(bool) ), SLOT( setVisibleLeftPanel(bool) ) );

    connect( ui->actionAbout, SIGNAL( triggered() ), SLOT( show_page_about() ) );
    connect( ui->actionDocumentation, SIGNAL( triggered() ), SLOT( show_page_documentation() ) );
    connect( ui->actionOpen, SIGNAL( triggered() ), SLOT( open() ) );
    connect( ui->actionAddNote, SIGNAL( triggered() ), SLOT( addNote() ) );
    connect( ui->actionAddNoteFromClipboard, SIGNAL( triggered() ), SLOT( addNoteFromClipboard() ) );
    connect( ui->actionAddNoteFromScreen, SIGNAL( triggered() ), SLOT( addNoteFromScreen() ) );
    connect( ui->actionRemoveAllNotes, SIGNAL( triggered() ), SLOT( removeAllNotes() ) );
    connect( ui->actionSaveAllNotes, SIGNAL( triggered() ), SLOT( saveAllNotes() ) );
    connect( ui->actionShowAllNotes, SIGNAL( triggered() ), SLOT( showAllNotes() ) );
    connect( ui->actionHideAllNotes, SIGNAL( triggered() ), SLOT( hideAllNotes() ) );
    connect( ui->actionQuit, SIGNAL( triggered() ), SLOT( quit() ) );
    connect( ui->actionSettings, SIGNAL( triggered() ), SLOT( show_page_settings() ) );
    connect( ui->actionOpenDict, SIGNAL( triggered() ), SLOT( openDictionary() ) );
    connect( ui->actionCloseDict, SIGNAL( triggered() ), SLOT( closeDictionary() ) );

    connect( ui->actionRemoveNote, SIGNAL( triggered() ), SLOT( removeNote() ) );
    connect( ui->actionDuplicateNote, SIGNAL( triggered() ), SLOT( duplicateNote() ) );

    connect( ui->actionSaveNoteAs, SIGNAL( triggered() ), SLOT( saveAsNote() ) );
    connect( ui->actionSaveNote, SIGNAL( triggered() ), SLOT( saveNote() ) );
    connect( ui->actionShowNote, SIGNAL( triggered() ), SLOT( showNote() ) );
    connect( ui->actionHideNote, SIGNAL( triggered() ), SLOT( hideNote() ) );
    connect( ui->actionPrintNote, SIGNAL( triggered() ), SLOT( printNote() ) );
    connect( ui->actionPreviewPrintNote, SIGNAL( triggered() ), SLOT( previewPrintNote() ) );
    connect( ui->actionTopNote, SIGNAL( triggered(bool) ), SLOT( setTopNote(bool) ) );
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
    actionAddNote = Create::Action::triggered( QIcon( ":/add" ), ui->tButtonAddNote->text(), this, SLOT( addNote() ) );
    actionAddNoteFromClipboard = Create::Action::triggered( QIcon( ":/add-from_clipboard" ), ui->tButtonAddNoteFromClipboard->text(), this, SLOT( addNoteFromClipboard() ) );
    actionAddNoteFromScreen = Create::Action::triggered( QIcon( ":/screenshot" ), ui->tButtonAddNoteFromScreen->text(), this, SLOT( addNoteFromScreen() ) );
    actionShowAllNotes = Create::Action::triggered( QIcon( ":/show_all" ), ui->tButtonShowAllNotes->text(), this, SLOT( showAllNotes() ) );
    actionHideAllNotes = Create::Action::triggered( QIcon( ":/hide_all" ), ui->tButtonHideAllNotes->text(), this, SLOT( hideAllNotes() ) );
    actionSaveAllNotes = Create::Action::triggered( QIcon( ":/save_all" ), ui->tButtonSaveAllNotes->text(), this, SLOT( saveAllNotes() ) );
    actionRemoveAllNotes = Create::Action::triggered( QIcon( ":/remove_all" ), ui->tButtonRemoveAllNotes->text(), this, SLOT( removeAllNotes() ) );
    actionSettings = Create::Action::triggered( QIcon( ":/settings" ), ui->tButtonSettings->text(), this, SLOT( show_page_settings() ) );
    actionAbout = Create::Action::triggered( QIcon( ":/about" ), ui->tButtonAbout->text(), this, SLOT( show_page_about() ) );
    actionQuit = Create::Action::triggered( QIcon( ":/quit" ), ui->tButtonQuit->text(), this, SLOT( quit() ) );

    trayMenu->addAction( actionOpenManager );
    trayMenu->addSeparator();
    trayMenu->addAction( actionAddNote );
    trayMenu->addAction( actionAddNoteFromClipboard );
    trayMenu->addAction( actionAddNoteFromScreen );
    trayMenu->addSeparator();
    trayMenu->addAction( actionShowAllNotes );
    trayMenu->addAction( actionHideAllNotes );
    trayMenu->addSeparator();
    trayMenu->addAction( actionSaveAllNotes );
    trayMenu->addAction( actionRemoveAllNotes );
    trayMenu->addSeparator();
    trayMenu->addAction( actionSettings );
    trayMenu->addSeparator();
    trayMenu->addAction( actionAbout );
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
    ui->leftPanel->setCurrentIndex( 0 );
}
void Manager::show_page_settings()
{
    show_Manager();
    ui->leftPanel->setCurrentIndex( 1 );
}
void Manager::show_page_about()
{
    show_Manager();
    ui->leftPanel->setCurrentIndex( 2 );
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
    setVisibleLeftPanel( settings->value( "LeftPanel_Visible", true ).toBool() );
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
    settings->setValue( "LeftPanel_Visible", isVisibleLeftPanel() );
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

void Manager::setVisibleLeftPanel( bool visible )
{
    ui->leftPanel->setVisible( visible );
    ui->actionVisibleLeftPanel->setChecked( visible );
}
bool Manager::isVisibleLeftPanel()
{
    return ui->leftPanel->isVisible();
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
