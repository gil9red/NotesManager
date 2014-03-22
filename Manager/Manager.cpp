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

    ui->leftPanel->addTab( QIcon( ":/Manager/notebook" ), tr( "Notes" ) );
    ui->leftPanel->addTab( QIcon( ":/Manager/settings" ), tr( "Settings" ) );
    ui->leftPanel->addTab( QIcon( ":/Manager/about" ),    tr( "About" ) );
    ui->leftPanel->addTab( QIcon( ":/Manager/quit" ),     tr( "Quit" ) );
    ui->leftPanel->setCurrentIndex( 0 );
    connect( ui->leftPanel, SIGNAL( currentChanged(int) ), SLOT( buttonLeftPanelClicked(int) ) );


    pageNotes = new Page_Notes();
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

    updateStates();
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
    pageNotes->setSettings( settings );
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
    if ( !Completer::instance()->count() )
    {
        ui->openDict->setVisible( false );
        ui->closeDict->setVisible( false );
    }

//    connect( ui->tButtonAbout, SIGNAL( clicked() ), SLOT( show_page_about() ) );
//    connect( ui->tButtonAddNote, SIGNAL( clicked() ), pageNotes, SLOT( addNote() ) );
//    connect( ui->tButtonAddNoteFromClipboard, SIGNAL( clicked() ), SLOT( addNoteFromClipboard() ) );
//    connect( ui->tButtonAddNoteFromScreen, SIGNAL( clicked() ), SLOT( addNoteFromScreen() ) );
//    connect( ui->tButtonRemoveAllNotes, SIGNAL( clicked() ), SLOT( removeAllNotes() ) );
//    connect( ui->tButtonSaveAllNotes, SIGNAL( clicked() ), SLOT( saveAllNotes() ) );
//    connect( ui->tButtonShowAllNotes, SIGNAL( clicked() ), SLOT( showAllNotes() ) );
//    connect( ui->tButtonHideAllNotes, SIGNAL( clicked() ), SLOT( hideAllNotes() ) );
//    connect( ui->tButtonQuit, SIGNAL( clicked() ), SLOT( quit() ) );
//    connect( ui->tButtonSettings, SIGNAL( clicked() ), SLOT( show_page_settings() ) );
//    connect( ui->tButtonOpenDict, SIGNAL( clicked() ), SLOT( openDictionary() ) );
//    connect( ui->tButtonCloseDict, SIGNAL( clicked() ), SLOT( closeDictionary() ) );

//    connect( ui->tButtonRemoveNote, SIGNAL( clicked() ), SLOT( removeNote() ) );
//    connect( ui->tButtonDuplicateNote, SIGNAL( clicked() ), SLOT( duplicateNote() ) );
//    connect( ui->tButtonSaveNoteAs, SIGNAL( clicked() ), SLOT( saveAsNote() ) );
//    connect( ui->tButtonSaveNote, SIGNAL( clicked() ), SLOT( saveNote() ) );
//    connect( ui->tButtonShowNote, SIGNAL( clicked() ), SLOT( showNote() ) );
//    connect( ui->tButtonHideNote, SIGNAL( clicked() ), SLOT( hideNote() ) );
//    connect( ui->tButtonPrintNote, SIGNAL( clicked() ), SLOT( printNote() ) );
//    connect( ui->tButtonPreviewPrintNote, SIGNAL( clicked() ), SLOT( previewPrintNote() ) );
//    connect( ui->tButtonTopNote, SIGNAL( clicked(bool) ), SLOT( setTopNote(bool) ) );
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

//    connect( ui->actionAbout, SIGNAL( triggered() ), SLOT( show_page_about() ) );
//    connect( ui->actionDocumentation, SIGNAL( triggered() ), SLOT( show_page_documentation() ) );

//    connect( ui->actionOpen, SIGNAL( triggered() ), SLOT( open() ) );
//    connect( ui->actionAddNote, SIGNAL( triggered() ), SLOT( addNote() ) );
//    connect( ui->actionAddNoteFromClipboard, SIGNAL( triggered() ), SLOT( addNoteFromClipboard() ) );
//    connect( ui->actionAddNoteFromScreen, SIGNAL( triggered() ), SLOT( addNoteFromScreen() ) );
//    connect( ui->actionRemoveAllNotes, SIGNAL( triggered() ), SLOT( removeAllNotes() ) );
//    connect( ui->actionSaveAllNotes, SIGNAL( triggered() ), SLOT( saveAllNotes() ) );
//    connect( ui->actionShowAllNotes, SIGNAL( triggered() ), SLOT( showAllNotes() ) );
//    connect( ui->actionHideAllNotes, SIGNAL( triggered() ), SLOT( hideAllNotes() ) );
//    connect( ui->actionQuit, SIGNAL( triggered() ), SLOT( quit() ) );
//    connect( ui->actionSettings, SIGNAL( triggered() ), SLOT( show_page_settings() ) );
//    connect( ui->actionOpenDict, SIGNAL( triggered() ), SLOT( openDictionary() ) );
//    connect( ui->actionCloseDict, SIGNAL( triggered() ), SLOT( closeDictionary() ) );

//    connect( ui->actionRemoveNote, SIGNAL( triggered() ), SLOT( removeNote() ) );
//    connect( ui->actionDuplicateNote, SIGNAL( triggered() ), SLOT( duplicateNote() ) );

//    connect( ui->actionSaveNoteAs, SIGNAL( triggered() ), SLOT( saveAsNote() ) );
//    connect( ui->actionSaveNote, SIGNAL( triggered() ), SLOT( saveNote() ) );
//    connect( ui->actionShowNote, SIGNAL( triggered() ), SLOT( showNote() ) );
//    connect( ui->actionHideNote, SIGNAL( triggered() ), SLOT( hideNote() ) );
//    connect( ui->actionPrintNote, SIGNAL( triggered() ), SLOT( printNote() ) );
//    connect( ui->actionPreviewPrintNote, SIGNAL( triggered() ), SLOT( previewPrintNote() ) );
//    connect( ui->actionTopNote, SIGNAL( triggered(bool) ), SLOT( setTopNote(bool) ) );
}
void Manager::createTray()
{
    tray.setIcon( QIcon( ":/App/icon-mini" ) );
    tray.setToolTip( QString( "%1 %2\n%3" ).arg( qApp->applicationName() ).arg( qApp->applicationVersion() ).arg( tr( "Program to create notes" ) ) );

    QMenu * trayMenu = new QMenu();
    tray.setContextMenu( trayMenu );

    connect( &tray, SIGNAL( activated(QSystemTrayIcon::ActivationReason) ), SLOT( messageReceived(QSystemTrayIcon::ActivationReason) ) );
    tray.show();

    actionOpenManager = Create::Action::triggered( QIcon( "" ), tr( "Open manager" ), this, SLOT( show_Manager() ) );
    actionSettings = Create::Action::triggered( ui->settings->icon(), ui->settings->text(), this, SLOT( show_page_settings() ) );
    actionAbout = Create::Action::triggered( ui->about->icon(), ui->about->text(), this, SLOT( show_page_about() ) );
    actionQuit = Create::Action::triggered( ui->quit->icon(), ui->quit->text(), this, SLOT( quit() ) );
//    actionAddNote = Create::Action::triggered( ui->addNote->icon(), ui->addNote->text(), this, SLOT( addNote() ) );
//    actionAddNoteFromClipboard = Create::Action::triggered( ui->addNoteFromClipboard->icon(), ui->addNoteFromClipboard->text(), this, SLOT( addNoteFromClipboard() ) );
//    actionAddNoteFromScreen = Create::Action::triggered( ui->addNoteFromScreen->icon(), ui->addNoteFromScreen->text(), this, SLOT( addNoteFromScreen() ) );
//    actionShowAllNotes = Create::Action::triggered( ui->showAllNotes->icon(), ui->showAllNotes->text(), this, SLOT( showAllNotes() ) );
//    actionHideAllNotes = Create::Action::triggered( ui->hideAllNotes->icon(), ui->hideAllNotes->text(), this, SLOT( hideAllNotes() ) );
//    actionSaveAllNotes = Create::Action::triggered( ui->saveAllNotes->icon(), ui->saveAllNotes->text(), this, SLOT( saveAllNotes() ) );
//    actionRemoveAllNotes = Create::Action::triggered( ui->removeAllNotes->icon(), ui->removeAllNotes->text(), this, SLOT( removeAllNotes() ) );

    trayMenu->addAction( actionOpenManager );
    trayMenu->addSeparator();
//    trayMenu->addAction( actionAddNote );
//    trayMenu->addAction( actionAddNoteFromClipboard );
//    trayMenu->addAction( actionAddNoteFromScreen );
//    trayMenu->addSeparator();
//    trayMenu->addAction( actionShowAllNotes );
//    trayMenu->addAction( actionHideAllNotes );
//    trayMenu->addSeparator();
//    trayMenu->addAction( actionSaveAllNotes );
//    trayMenu->addAction( actionRemoveAllNotes );
//    trayMenu->addSeparator();
    trayMenu->addAction( actionSettings );
    trayMenu->addSeparator();
    trayMenu->addAction( actionAbout );
    trayMenu->addSeparator();
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
        WARNING( "Unable to open documents" );
    }
}

void Manager::updateStates()
{
    ui->actionVisibleToolbarMain->setChecked( ui->toolBarMain->isVisible() );
    ui->actionVisibleToolbarManage->setChecked( ui->toolBarManage->isVisible() );
    ui->actionVisibleToolbarNote->setChecked( ui->toolBarNote->isVisible() );
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

    updateStates();
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
