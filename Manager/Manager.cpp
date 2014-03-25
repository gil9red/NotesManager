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
    settings(0)
{
    self = this;       
    ui->setupUi( this );

    ui->sidebar->addTab( QIcon( ":/Manager/notebook" ), tr( "Notes" ) );
    ui->sidebar->addTab( QIcon( ":/Manager/settings" ), tr( "Settings" ) );
    ui->sidebar->addTab( QIcon( ":/Manager/about" ),    tr( "About" ) );
    ui->sidebar->addTab( QIcon( ":/Manager/quit" ),     tr( "Quit" ) );
    ui->sidebar->setCurrentIndex( 0 );
    connect( ui->sidebar, SIGNAL( currentChanged(int) ), SLOT( buttonSidebarClicked(int) ) );


    pageNotes = new Page_Notes();
    pageSettings = new Page_Settings();
    pageAbout = new Page_About();

    connect( pageNotes, SIGNAL( about_updateStates() ), SLOT( updateStates() ) );

    connect( pageSettings, SIGNAL( message(QString, int) ), ui->statusBar, SLOT( showMessage(QString, int) ) );
    connect( pageSettings, SIGNAL( acceptChangeSettings() ), SLOT( acceptChangeSettings() ) );

    ui->stackedWidget_Pages->addWidget( pageNotes );
    ui->stackedWidget_Pages->addWidget( pageSettings );
    ui->stackedWidget_Pages->addWidget( pageAbout );
    ui->stackedWidget_Pages->setCurrentIndex( ui->sidebar->currentIndex() );

    createMenu();
    createToolBars();
    createTray();

    connect( &autoSaveTimer, SIGNAL( timeout() ), SLOT( writeSettings() ) );

    updateStates();
}

Manager::~Manager()
{
    self = 0;
    delete ui;
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
    updateStates();
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
        ui->toolBarOpenDict->setVisible( false );
        ui->toolBarCloseDict->setVisible( false );
    }

    connect( ui->toolBarSettings, SIGNAL( triggered() ), SLOT( show_page_settings() ) );
    connect( ui->toolBarAbout, SIGNAL( triggered() ), SLOT( show_page_about() ) );
    connect( ui->toolBarQuit, SIGNAL( triggered() ), SLOT( quit() ) );

    connect( ui->toolBarAddNote, SIGNAL( triggered() ), pageNotes, SLOT( addNote() ) );
    connect( ui->toolBarAddNoteFromClipboard, SIGNAL( triggered() ), pageNotes, SLOT( addNoteFromClipboard() ) );
    connect( ui->toolBarAddNoteFromScreen, SIGNAL( triggered() ), pageNotes, SLOT( addNoteFromScreen() ) );
    connect( ui->toolBarAddFolder, SIGNAL( triggered() ), pageNotes, SLOT( addFolder() ) );
    connect( ui->toolBarRemoveToTrash, SIGNAL( triggered() ), pageNotes, SLOT( removeToTrash() ) );
    connect( ui->toolBarRemoveAllNotes, SIGNAL( triggered() ), pageNotes, SLOT( removeAllToTrash() ) );
    connect( ui->toolBarDelete, SIGNAL( triggered() ), pageNotes, SLOT( removeFromTrash() ) );
    connect( ui->toolBarClearTrash, SIGNAL( triggered() ), pageNotes, SLOT( clearTrash() ) );

    connect( ui->toolBarOpenDict, SIGNAL( triggered() ), SLOT( openDictionary() ) );
    connect( ui->toolBarCloseDict, SIGNAL( triggered() ), SLOT( closeDictionary() ) );
    connect( ui->toolBarSaveAllNotes, SIGNAL( triggered() ), pageNotes, SLOT( saveAllNotes() ) );
    connect( ui->toolBarShowAllNotes, SIGNAL( triggered() ), pageNotes, SLOT( showAllNotes() ) );
    connect( ui->toolBarHideAllNotes, SIGNAL( triggered() ), pageNotes, SLOT( hideAllNotes() ) );
    connect( ui->toolBarSaveNoteAs, SIGNAL( triggered() ), pageNotes, SLOT( saveAsNote() ) );
    connect( ui->toolBarSaveNote, SIGNAL( triggered() ), pageNotes, SLOT( saveNote() ) );
    connect( ui->toolBarShowNote, SIGNAL( triggered() ), pageNotes, SLOT( showNote() ) );
    connect( ui->toolBarHideNote, SIGNAL( triggered() ), pageNotes, SLOT( hideNote() ) );
    connect( ui->toolBarPrintNote, SIGNAL( triggered() ), pageNotes, SLOT( printNote() ) );
    connect( ui->toolBarPreviewPrintNote, SIGNAL( triggered() ), pageNotes, SLOT( previewPrintNote() ) );
    connect( ui->toolBarTopNote, SIGNAL( triggered(bool) ), pageNotes, SLOT( setTopNote(bool) ) );
    //connect( ui->duplicateNote, SIGNAL( triggered() ), SLOT( duplicateNote() ) );
}
void Manager::createMenu()
{
    // если словарь пуст
    if ( !Completer::instance()->count() )
    {
        ui->actionOpenDict->setVisible( false );
        ui->actionCloseDict->setVisible( false );
    }

    connect( ui->actionShowSidebar, SIGNAL( triggered(bool) ), SLOT( setShowSidebar(bool) ) );

    connect( ui->actionFull_screen, SIGNAL( triggered(bool) ), SLOT( setFullScreen(bool) ) );

    connect( ui->actionQuit, SIGNAL( triggered() ), ui->toolBarQuit, SIGNAL( triggered() ) );
    connect( ui->actionSettings, SIGNAL( triggered() ), ui->toolBarSettings, SIGNAL( triggered() ) );
    connect( ui->actionAbout, SIGNAL( triggered() ), ui->toolBarAbout, SIGNAL( triggered() ) );
    connect( ui->actionDocumentation, SIGNAL( triggered() ), SLOT( show_page_documentation() ) );

    //  TODO:  connect( ui->actionOpen, SIGNAL( triggered() ), SLOT( open() ) );

    connect( ui->actionAddNote, SIGNAL( triggered() ), ui->toolBarAddNote, SIGNAL( triggered() ) );
    connect( ui->actionAddNoteFromClipboard, SIGNAL( triggered() ), ui->toolBarAddNoteFromClipboard, SIGNAL( triggered() ) );
    connect( ui->actionAddNoteFromScreen, SIGNAL( triggered() ), ui->toolBarAddNoteFromScreen, SIGNAL( triggered() ) );
//    connect( ui->actionRemoveNote, SIGNAL( triggered() ), SLOT( removeNote() ) );
//    connect( ui->actionRemoveAllNotes, SIGNAL( triggered() ), SLOT( removeAllNotes() ) );

    connect( ui->actionOpenDict, SIGNAL( triggered() ), ui->toolBarOpenDict, SIGNAL( triggered() ) );
    connect( ui->actionCloseDict, SIGNAL( triggered() ), ui->toolBarCloseDict, SIGNAL( triggered() ) );
    connect( ui->actionSaveAllNotes, SIGNAL( triggered() ), ui->toolBarSaveAllNotes, SIGNAL( triggered() ) );
    connect( ui->actionShowAllNotes, SIGNAL( triggered() ), ui->toolBarShowAllNotes, SIGNAL( triggered() ) );
    connect( ui->actionHideAllNotes, SIGNAL( triggered() ), ui->toolBarHideAllNotes, SIGNAL( triggered() ) );    
    connect( ui->actionSaveNoteAs, SIGNAL( triggered() ), ui->toolBarSaveNoteAs, SIGNAL( triggered() ) );
    connect( ui->actionSaveNote, SIGNAL( triggered() ), ui->toolBarSaveNote, SIGNAL( triggered() ) );
    connect( ui->actionShowNote, SIGNAL( triggered() ), ui->toolBarShowNote, SIGNAL( triggered() ) );
    connect( ui->actionHideNote, SIGNAL( triggered() ), ui->toolBarHideNote, SIGNAL( triggered() ) );
    connect( ui->actionPrintNote, SIGNAL( triggered() ), ui->toolBarPrintNote, SIGNAL( triggered() ) );
    connect( ui->actionPreviewPrintNote, SIGNAL( triggered() ), ui->toolBarPreviewPrintNote, SIGNAL( triggered() ) );
    connect( ui->actionTopNote, SIGNAL( triggered(bool) ), ui->toolBarTopNote, SIGNAL( triggered() ) );
    //connect( ui->actionDuplicateNote, SIGNAL( triggered() ), SLOT( duplicateNote() ) );
}
void Manager::createTray()
{
    tray.setIcon( QIcon( ":/App/icon-mini" ) );
    const QString & name = qApp->applicationName();
    const QString & version = qApp->applicationVersion();
    const QString & description = tr( "The program creates notes" );
    tray.setToolTip( QString( "%1 %2\n%3" ).arg( name ).arg( version ).arg( description ) );
    connect( &tray, SIGNAL( activated(QSystemTrayIcon::ActivationReason) ), SLOT( messageReceived(QSystemTrayIcon::ActivationReason) ) );
    tray.show();

    trayActionOpenManager = Create::Action::triggered( QIcon( "" ), tr( "Open manager" ), this, SLOT( show_Manager() ) );
    trayActionSettings = Create::Action::triggered( ui->toolBarSettings->icon(), ui->toolBarSettings->text(), ui->toolBarSettings, SIGNAL( triggered() ) );
    trayActionDocumentation = Create::Action::triggered( ui->actionDocumentation->icon(), ui->actionDocumentation->text(), ui->actionDocumentation, SIGNAL( triggered() ) );
    trayActionAbout = Create::Action::triggered( ui->toolBarAbout->icon(), ui->toolBarAbout->text(), ui->toolBarAbout, SIGNAL( triggered() ) );
    trayActionQuit = Create::Action::triggered( ui->toolBarQuit->icon(), ui->toolBarQuit->text(), ui->toolBarQuit, SIGNAL( triggered() ) );
    trayActionAddNote = Create::Action::triggered( ui->toolBarAddNote->icon(), ui->toolBarAddNote->text(), ui->toolBarAddNote, SIGNAL( triggered() ) );
    trayActionAddNoteFromClipboard = Create::Action::triggered( ui->toolBarAddNoteFromClipboard->icon(), ui->toolBarAddNoteFromClipboard->text(), ui->toolBarAddNoteFromClipboard, SIGNAL( triggered() ) );
    trayActionAddNoteFromScreen = Create::Action::triggered( ui->toolBarAddNoteFromScreen->icon(), ui->toolBarAddNoteFromScreen->text(), ui->toolBarAddNoteFromScreen, SIGNAL( triggered() ) );
    trayActionShowAllNotes = Create::Action::triggered( ui->toolBarShowAllNotes->icon(), ui->toolBarShowAllNotes->text(), ui->toolBarShowAllNotes, SIGNAL( triggered() ) );
    trayActionHideAllNotes = Create::Action::triggered( ui->toolBarHideAllNotes->icon(), ui->toolBarHideAllNotes->text(), ui->toolBarHideAllNotes, SIGNAL( triggered() ) );
    trayActionSaveAllNotes = Create::Action::triggered( ui->toolBarSaveAllNotes->icon(), ui->toolBarSaveAllNotes->text(), ui->toolBarSaveAllNotes, SIGNAL( triggered() ) );

    QMenu * trayMenu = new QMenu();
    trayMenu->addAction( trayActionOpenManager );
    trayMenu->addSeparator();
    trayMenu->addAction( trayActionAddNote );
    trayMenu->addAction( trayActionAddNoteFromClipboard );
    trayMenu->addAction( trayActionAddNoteFromScreen );
    trayMenu->addSeparator();
    trayMenu->addAction( trayActionShowAllNotes );
    trayMenu->addAction( trayActionHideAllNotes );
    trayMenu->addAction( trayActionSaveAllNotes );
    trayMenu->addSeparator();
    trayMenu->addAction( trayActionSettings );
    trayMenu->addSeparator();
    trayMenu->addAction( trayActionDocumentation );
    trayMenu->addSeparator();
    trayMenu->addAction( trayActionAbout );
    trayMenu->addSeparator();
    trayMenu->addAction( trayActionQuit );

    tray.setContextMenu( trayMenu );
}

void Manager::buttonSidebarClicked( int index )
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
    foreach ( const QString & command, text.split(" ") )
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
    ui->sidebar->setCurrentIndex( 0 );
}
void Manager::show_page_settings()
{
    show_Manager();
    ui->sidebar->setCurrentIndex( 1 );
}
void Manager::show_page_about()
{
    show_Manager();
    ui->sidebar->setCurrentIndex( 2 );
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
    bool isEmpty = pageNotes->isEmpty();
    ui->toolBarHideAllNotes->setEnabled( !isEmpty );
    ui->toolBarShowAllNotes->setEnabled( !isEmpty );
    ui->toolBarSaveAllNotes->setEnabled( !isEmpty );

    ui->actionFull_screen->setChecked( isFullScreen() );

    bool isAutocomplete = Completer::instance()->isAutocomplete();
    ui->toolBarCloseDict->setEnabled( isAutocomplete );
    ui->toolBarOpenDict->setEnabled( !isAutocomplete );

//    ui->toolBarDuplicateNote->setEnabled( false );

    ui->toolBarSaveNote->setEnabled( false );
    ui->toolBarSaveNoteAs->setEnabled( false );
    ui->toolBarPrintNote->setEnabled( false );
    ui->toolBarPreviewPrintNote->setEnabled( false );
    ui->toolBarHideNote->setEnabled( false );
    ui->toolBarShowNote->setEnabled( false );
    ui->toolBarTopNote->setEnabled( false );

    ui->toolBarRemoveAllNotes->setEnabled( !pageNotes->isEmpty() );
    ui->toolBarDelete->setEnabled( false );
    ui->toolBarRemoveToTrash->setEnabled( false );
    ui->toolBarClearTrash->setEnabled( !pageNotes->trashIsEmpty() );

    bool hasCurrent = pageNotes->hasCurrent();
    if ( hasCurrent )
    {
        bool isNote = pageNotes->currentIsNote();
        if ( isNote )
        {
            ui->toolBarSaveNote->setEnabled( true );
            ui->toolBarSaveNoteAs->setEnabled( true );
            ui->toolBarPrintNote->setEnabled( true );
            ui->toolBarPreviewPrintNote->setEnabled( true );

            bool currentNoteIsVisible = pageNotes->currentNoteIsVisible();
            ui->toolBarHideNote->setEnabled( currentNoteIsVisible );
            ui->toolBarShowNote->setEnabled( !currentNoteIsVisible );

            bool currentNoteIsTop = pageNotes->currentNoteIsTop();
            ui->toolBarTopNote->setEnabled( currentNoteIsTop );
        }

        bool isChildTrash = pageNotes->currentIsChildTrash(); // если элемент есть в корзине
        ui->toolBarRemoveToTrash->setEnabled( !pageNotes->currentIsTrash() && !isChildTrash ); // переместить в корзину
        ui->toolBarDelete->setEnabled( isChildTrash );
        ui->toolBarClearTrash->setEnabled( pageNotes->currentIsTrash() || isChildTrash );
    }

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
void Manager::setFullScreen( bool fs )
{
    if ( fs )
        showFullScreen();
    else
        showNormal();

    ui->actionFull_screen->setChecked( fs );
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
    setShowSidebar( settings->value( "sidebar_Visible", true ).toBool() );
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
    settings->setValue( "sidebar_Visible", isShowSidebar() );
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

void Manager::openDictionary()
{
    Completer::instance()->setAutocomplete( true );
    updateStates();
}
void Manager::closeDictionary()
{
    Completer::instance()->setAutocomplete( false );
    updateStates();
}

void Manager::setShowSidebar( bool visible )
{
    ui->sidebar->setVisible( visible );
    ui->actionShowSidebar->setChecked( visible );
}
bool Manager::isShowSidebar()
{
    return ui->sidebar->isVisible();
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
