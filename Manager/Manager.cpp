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
#include "FormattingToolbar/FormattingToolbar.h"
#include "utils/func.h"
#include "ImageCropper/fullscreenshotcropper.h"
#include "NavigationPanel/src/notebook.h"

Manager * Manager::self = 0;

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
    QObject::connect( ui->sidebar, SIGNAL( currentChanged(int) ), SLOT( buttonSidebarClicked(int) ) );


    pageNotes = new Page_Notes();
    pageSettings = new Page_Settings();
    pageAbout = new Page_About();

    QObject::connect( pageNotes, SIGNAL( sg_About_UpdateStates() ), SLOT( updateStates() ) );

    QObject::connect( pageSettings, SIGNAL( message(QString, int) ), ui->statusBar, SLOT( showMessage(QString, int) ) );
    QObject::connect( pageSettings, SIGNAL( acceptChangeSettings() ), SLOT( acceptChangeSettings() ) );

    ui->stackedWidget_Pages->addWidget( pageNotes );
    ui->stackedWidget_Pages->addWidget( pageSettings );
    ui->stackedWidget_Pages->addWidget( pageAbout );
    ui->stackedWidget_Pages->setCurrentIndex( ui->sidebar->currentIndex() );


    // Меню и действия
    {
        ui->menuFile->addAction( pageNotes->ui->actionAddNote );
        ui->menuFile->addAction( pageNotes->ui->actionAddNoteFromClipboard );
        ui->menuFile->addAction( pageNotes->ui->actionAddNoteFromScreen );
        ui->menuFile->addSeparator();
        ui->menuFile->addAction( pageNotes->ui->actionPrintNote );
        ui->menuFile->addAction( pageNotes->ui->actionPreviewPrintNote );
        ui->menuFile->addSeparator();
        ui->menuFile->addAction( pageNotes->ui->actionSaveNoteAs );
        ui->menuFile->addSeparator();
        ui->menuFile->addAction( pageNotes->ui->actionShowAllNotes );
        ui->menuFile->addAction( pageNotes->ui->actionHideAllNotes );
        ui->menuFile->addSeparator();
        ui->menuFile->addAction( ui->actionQuit );

        ui->menuNote->addActions( pageNotes->ui->toolBarNote->actions() );

        ui->menuService->addAction( ui->actionSettings );

        ui->menuHelp->addSeparator();
        ui->menuHelp->addAction( ui->actionAbout );


        QObject::connect( ui->actionSettings, SIGNAL( triggered() ), SLOT( showPageSettings() ) );
        QObject::connect( ui->actionAbout, SIGNAL( triggered() ), SLOT( showPageAbout() ) );
        QObject::connect( ui->actionQuit, SIGNAL( triggered() ), SLOT( quit() ) );

        QObject::connect( ui->actionOpenDict, SIGNAL( triggered() ), SLOT( openDictionary() ) );
        QObject::connect( ui->actionCloseDict, SIGNAL( triggered() ), SLOT( closeDictionary() ) );

        QObject::connect( ui->actionFullScreen, SIGNAL( triggered(bool) ), SLOT( setFullScreen(bool) ) );
        QObject::connect( ui->actionShowSidebar, SIGNAL( triggered(bool) ), SLOT( setShowSidebar(bool) ) );
        QObject::connect( ui->actionShowStatusBar, SIGNAL( triggered(bool) ), SLOT( setShowStatusBar(bool) ) );

        QObject::connect( ui->actionDocumentation, SIGNAL( triggered() ), SLOT( showPageDocumentation() ) );


        // Меню трея
        {
            tray.setIcon( QIcon( ":/App/icon-mini" ) );
            const QString & name = qApp->applicationName();
            const QString & version = qApp->applicationVersion();
            const QString & description = tr( "The program creates notes" );
            tray.setToolTip( QString( "%1 %2\n%3" ).arg( name ).arg( version ).arg( description ) );
            QObject::connect( &tray, SIGNAL( activated(QSystemTrayIcon::ActivationReason) ), SLOT( messageReceived(QSystemTrayIcon::ActivationReason) ) );
            tray.show();

            QMenu * trayMenu = new QMenu();
            trayMenu->addAction( QIcon( "" ), tr( "Open manager" ), this, SLOT( showManager() ), QKeySequence() ); // TODO: add icon
            trayMenu->addSeparator();
            trayMenu->addAction( pageNotes->ui->actionAddNote );
            trayMenu->addAction( pageNotes->ui->actionAddNoteFromClipboard );
            trayMenu->addAction( pageNotes->ui->actionAddNoteFromScreen );
            trayMenu->addSeparator();
            trayMenu->addAction( pageNotes->ui->actionShowAllNotes );
            trayMenu->addAction( pageNotes->ui->actionHideAllNotes );
            trayMenu->addAction( pageNotes->ui->actionSaveAllNotes );
            trayMenu->addSeparator();
            trayMenu->addAction( ui->actionSettings );
            trayMenu->addSeparator();
            trayMenu->addAction( ui->actionDocumentation );
            trayMenu->addSeparator();
            trayMenu->addAction( ui->actionAbout );
            trayMenu->addSeparator();
            trayMenu->addAction( ui->actionQuit );

            tray.setContextMenu( trayMenu );
        }

        // Управление видимостью тулбаров, размещенных на странице Заметки
        {
            QToolBar * toolBarManage = pageNotes->ui->toolBarManage;
            QAction * actionVisibleToolbarManage = new QAction( QIcon( "" ), toolBarManage->windowTitle(), this );
            actionVisibleToolbarManage->setCheckable( true );
            QObject::connect( actionVisibleToolbarManage, SIGNAL(triggered(bool)), toolBarManage, SLOT(setVisible(bool)) );
            QObject::connect( toolBarManage, SIGNAL(visibilityChanged(bool)), actionVisibleToolbarManage, SLOT(setChecked(bool)) );

            QToolBar * toolBarNote = pageNotes->ui->toolBarNote;
            QAction * actionVisibleToolbarNote = new QAction( QIcon( "" ), toolBarNote->windowTitle(), this );
            actionVisibleToolbarNote->setCheckable( true );
            QObject::connect( actionVisibleToolbarNote, SIGNAL(triggered(bool)), toolBarNote, SLOT(setVisible(bool)) );
            QObject::connect( toolBarNote, SIGNAL(visibilityChanged(bool)), actionVisibleToolbarNote, SLOT(setChecked(bool)) );

            ui->menuToolbars->addAction( actionVisibleToolbarManage );
            ui->menuToolbars->addAction( actionVisibleToolbarNote );
        }
    }

    QObject::connect( &autoSaveTimer, SIGNAL( timeout() ), SLOT( writeSettings() ) );

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
        showManager();
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
            showManager();

        else if ( command == "-message" )
            tray.showMessage( tr( "Information" ), tr( "Application is already running" ), QSystemTrayIcon::Information, 5000 );

        else if ( command == "-beep" )
            qApp->beep();
    }
}
void Manager::messageReceived( QSystemTrayIcon::ActivationReason reason )
{
    if ( reason == QSystemTrayIcon::Trigger )
    {
        if ( isVisible() )
            hide();
        else
            showManager();
    }
}

void Manager::acceptChangeSettings()
{
    setActivateTimerAutosave( pageSettings->mapSettings[ "Autosave" ].toBool() );
    setIntervalAutosave( pageSettings->mapSettings[ "AutosaveInterval" ].toInt() );
}

void Manager::showPageNotes()
{
    showManager();
    ui->sidebar->setCurrentIndex( 0 );
}
void Manager::showPageSettings()
{
    showManager();
    ui->sidebar->setCurrentIndex( 1 );
}
void Manager::showPageAbout()
{
    showManager();
    ui->sidebar->setCurrentIndex( 2 );
}
void Manager::showPageDocumentation()
{
    if ( !QDesktopServices::openUrl( QUrl::fromLocalFile( qApp->applicationDirPath() + "/doc/ru/html/index.html" ) ) )
    {
        QMessageBox::information( this, tr( "Information" ), tr( "Unable to open documents" ) );
        WARNING( "Unable to open documents" );
    }
}

void Manager::updateStates()
{    
    ui->actionFullScreen->setChecked( isFullScreen() );

    bool isAutocomplete = Completer::instance()->isAutocomplete();
    ui->actionCloseDict->setEnabled( isAutocomplete );
    ui->actionOpenDict->setEnabled( !isAutocomplete );

    ui->actionVisibleToolbarMain->setChecked( ui->toolBarMain->isVisible() );
}

void Manager::showManager()
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

    ui->actionFullScreen->setChecked( fs );
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
    setShowSidebar( settings->value( "Sidebar_Visible", true ).toBool() );
    setShowStatusBar( settings->value( "StatusBar_Visible", true ).toBool() );
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
    settings->setValue( "Sidebar_Visible", isShowSidebar() );
    settings->setValue( "StatusBar_Visible", isShowStatusBar() );
    settings->endGroup();

    pageNotes->writeSettings();
    pageSettings->writeSettings();

    settings->sync();

    ui->statusBar->showMessage( tr( "Save completed" ), 5000 );


    // Передача xml файла, в котором будет описано иерархическое дерево с заметками
    {
        const QString & fileName = qApp->applicationDirPath() + "/notes/notebook.xml";
        QFile file( fileName );
        if ( !file.open( QFile::WriteOnly | QFile::Text ) )
        {
            QMessageBox::warning( this, tr( "Error" ), tr( "Cannot write file %1:\n%2." ).arg( fileName ).arg( file.errorString() ) );
            return;
        }
        pageNotes->write( &file );
    }
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
void Manager::setShowStatusBar( bool show )
{
    ui->statusBar->setVisible( show );
    ui->actionShowStatusBar->setChecked( show );
}
bool Manager::isShowStatusBar()
{
    return ui->statusBar->isVisible();
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
