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
#include "NavigationPanel/notebook.h"
#include <QDockWidget>
#include <QtWebKit/QWebView>

Manager * Manager::self = 0;

Manager::Manager( QWidget * parent ) :
    QMainWindow( parent ),
    ui( new Ui::Manager ),
    tray( new QSystemTrayIcon( QIcon( ":/App/icon-mini" ) ) ),
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

    // Загрузка списка документаций в меню документации
    {           
        QList < QAction * > list;
        foreach ( const QFileInfo & info, QDir( getDocumentationPath() ).entryInfoList( QDir::Dirs | QDir::NoDotAndDotDot ) )
        {
            const QString & dirName = info.fileName();
            const QString & fullPath = getDocumentationPath() + "/" + dirName + "/html/index.html";
            if ( !QFile( fullPath ).exists() )
                continue;

            QAction * action = new QAction( dirName, this );
            action->setData( QDir::fromNativeSeparators( fullPath ) );
            QObject::connect( action, SIGNAL(triggered()), this, SLOT(showDocumentation()) );

            list << action;
        }
        QMenu * documentations = new QMenu();
        documentations->addActions( list );

        ui->actionDocumentation->setEnabled( !list.isEmpty() );
        ui->actionDocumentation->setMenu( documentations );
    }

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

        QObject::connect( ui->menuEdit, SIGNAL(aboutToShow()), SLOT(prepareMenuEdit()) );

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
        QObject::connect( ui->actionTopOfAllWindows, SIGNAL(triggered(bool)), SLOT(setTopOfAllWindows(bool)) );
        QObject::connect( ui->actionShowSidebar, SIGNAL( triggered(bool) ), SLOT( setShowSidebar(bool) ) );
        QObject::connect( ui->actionShowStatusBar, SIGNAL( triggered(bool) ), SLOT( setShowStatusBar(bool) ) );

        // Меню трея
        {
            QObject::connect( tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), SLOT(messageReceived(QSystemTrayIcon::ActivationReason)) );
            QObject::connect( tray, SIGNAL(messageClicked()), SLOT(showManager()) );

            QMenu * trayMenu = new QMenu();
            trayMenu->addAction( QIcon( ":/Manager/manager" ), tr( "Open manager" ), this, SLOT( showManager() ), QKeySequence() );
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

            tray->setContextMenu( trayMenu );                    

            updateSystemTray();
            QObject::connect( Notebook::instance(), SIGNAL(notesChange()), SLOT(updateSystemTray()) );

            tray->show();
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

    // Меню "Сценарии"
    {
        // Добавление Менежера сценариев в прикрепляемый виджет
        {
            scriptsManager = new ScriptsManager();
            dockScriptsManager = new QDockWidget( scriptsManager->windowTitle() );
            dockScriptsManager->setObjectName( "Dock_ScriptsManager" );
            dockScriptsManager->setWidget( scriptsManager );
            addDockWidget( Qt::RightDockWidgetArea, dockScriptsManager );

            ui->actionShowScriptsManager->setChecked( dockScriptsManager->isVisible() );

            QObject::connect( ui->actionShowScriptsManager, SIGNAL(toggled(bool)), dockScriptsManager, SLOT(setVisible(bool)) );
            QObject::connect( dockScriptsManager, SIGNAL(visibilityChanged(bool)), ui->actionShowScriptsManager, SLOT(setChecked(bool)) );
        }

        // Меню "Закладки"
        ui->menuScripts->addSeparator();
        ui->menuScripts->addMenu( scriptsManager->menuBookmarkScript );
    }

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

void Manager::loadScripts()
{
    scriptsManager->read();
}

void Manager::setSettings( QSettings * s )
{
    settings = s;
    pageNotes->setSettings( settings );
    pageSettings->setSettings( settings );
    scriptsManager->setSettings( settings );
}

Manager * Manager::instance()
{
    return self;
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
            tray->showMessage( tr( "Information" ), tr( "Application is already running" ), QSystemTrayIcon::Information, 5000 );

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
void Manager::showDocumentation()
{
    QAction * action = qobject_cast < QAction * > ( sender() );
    if ( !action )
    {
        WARNING( "Null pointer!" );
    }

    const QUrl & path = QUrl::fromLocalFile( action->data().toString() );
    // Создадим окно с документацией.
    QWebView * documentation = new QWebView();
    documentation->setContextMenuPolicy( Qt::NoContextMenu );
    documentation->setAttribute( Qt::WA_DeleteOnClose );
    documentation->show();
    documentation->load( path );
    QObject::connect( this, SIGNAL(destroyed()), documentation, SLOT(deleteLater()) );
}

void Manager::updateStates()
{    
    ui->actionFullScreen->setChecked( isFullScreen() );
    ui->actionTopOfAllWindows->setChecked( isTopOfAllWindows() );

    bool isAutocomplete = Completer::instance()->isAutocomplete();
    ui->actionCloseDict->setEnabled( isAutocomplete );
    ui->actionOpenDict->setEnabled( !isAutocomplete );

    ui->actionVisibleToolbarMain->setChecked( ui->toolBarMain->isVisible() );
}

void Manager::updateSystemTray()
{
    const QList < Note * > & notes = Notebook::instance()->getNotesList();
    int count = notes.size();
    int visible_count = 0;
    foreach ( Note * note, notes)
        if ( note->getRichTextNote()->isVisible() )
            visible_count++;

    QString details;
    details += "\n";
    details += tr( "In total notes: %1" ).arg( count ) + "\n";
    details += tr( "Visible notes: %1" ).arg( visible_count ) + "\n";
    details += tr( "Invisible notes: %1" ).arg( count - visible_count );

    const QString & name = qApp->applicationName();
    const QString & version = qApp->applicationVersion();
    const QString & description = tr( "The program creates notes" );

    QString trayToolTip;
    trayToolTip += QString( "%1 %2\n" ).arg( name ).arg( version );
    trayToolTip += description + "\n";
    trayToolTip += details;
    tray->setToolTip( trayToolTip );
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

    updateStates();
}

void Manager::setTopOfAllWindows( bool top )
{
    setWindowFlags( top ? windowFlags() | Qt::WindowStaysOnTopHint : windowFlags() & ~Qt::WindowStaysOnTopHint );
    show();
    updateStates();
}
bool Manager::isTopOfAllWindows()
{
    return windowFlags().testFlag( Qt::WindowStaysOnTopHint );
}

void Manager::quit()
{
    bool askBeforeExiting = pageSettings->mapSettings[ "AskBeforeExiting" ].toBool();
    if ( askBeforeExiting )
    {
        QMessageBox::StandardButton result = showNewMessageBox( this, QMessageBox::Question, tr( "Question" ), tr( "Really quit?" ), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok );
        if ( result == QMessageBox::Cancel )
            return;
    }

    // Если не скрыть, то после закрытия иконка будет "висеть" в трее, пока на нее не наведешь мышку
    tray->hide();

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
    setActivateDictionary( settings->value( "Autocomplete" ).toBool() );
    setShowSidebar( settings->value( "Sidebar_Visible", true ).toBool() );
    setShowStatusBar( settings->value( "StatusBar_Visible", true ).toBool() );
    setTopOfAllWindows( settings->value( "TopOfAllWindows", false ).toBool() );
    settings->endGroup();

    pageNotes->readSettings();    
    pageSettings->readSettings();
    scriptsManager->readSettings();

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
    settings->setValue( "Autocomplete", isActivateDictionary() );
    settings->setValue( "Sidebar_Visible", isShowSidebar() );
    settings->setValue( "StatusBar_Visible", isShowStatusBar() );
    settings->setValue( "TopOfAllWindows", isTopOfAllWindows() );
    settings->endGroup();

    pageNotes->writeSettings();
    pageNotes->writeToXmlStateNotes();
    pageSettings->writeSettings();
    scriptsManager->writeSettings();
    scriptsManager->write();

    settings->sync();

    ui->statusBar->showMessage( tr( "Save completed" ), 5000 );    
}

void Manager::setActivateDictionary( bool activate )
{
    Completer::instance()->setAutocomplete( activate );
    updateStates();
}
bool Manager::isActivateDictionary()
{
    return Completer::instance()->isAutocomplete();
}
void Manager::openDictionary()
{
    setActivateDictionary( true );
}
void Manager::closeDictionary()
{
    setActivateDictionary( false );
}

void Manager::setShowSidebar( bool show )
{
    ui->sidebar->setVisible( show );
    ui->actionShowSidebar->setChecked(show);
}
bool Manager::isShowSidebar()
{
    return ui->actionShowSidebar->isChecked();
}
void Manager::setShowStatusBar( bool show )
{
    ui->statusBar->setVisible( show );
    ui->actionShowStatusBar->setChecked(show);
}
bool Manager::isShowStatusBar()
{
    return ui->actionShowStatusBar->isChecked();
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

void Manager::prepareMenuEdit()
{
    ui->menuEdit->clear();
    ui->menuEdit->addActions( pageNotes->ui->tabWidget_EditNotes->editActions() );
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
