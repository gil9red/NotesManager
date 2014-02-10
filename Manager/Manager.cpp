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

#include "Manager.h"
#include "ui_Manager.h"

#include <QStandardItem>
#include <QFileInfo>
#include <QAbstractItemModel>
#include <QClipboard>
#include <QDesktopWidget>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>

Manager * Manager::self = 0;

#include "FormattingToolbar/FormattingToolbar.h"
#include "utils/func.h"
#include "JlCompress.h"
#include "ImageCropper/fullscreenshotcropper.h"

QStandardItem * toStandardItem( RichTextNote * note, int role )
{
    QVariant data;
    data.setValue( note );

    QStandardItem * item = new QStandardItem();
    item->setData( data, role );

    return item;
}

RichTextNote * toNote( const QStandardItem * item, int role )
{
    return item->data( role ).value < RichTextNote * > ();
}

RichTextNote * toNote( const QModelIndex index, int role )
{
    return index.data( role ).value < RichTextNote * > ();
}

QList < QStandardItem * > toStandardItems( RichTextNote * note )
{
    QStandardItem * title = toStandardItem( note );
    title->setText( note->title() );
    title->setToolTip( title->text() );

    QStandardItem * visibility = new QStandardItem();
    visibility->setText( note->isVisible() ? QTranslator::tr( "yes" ) : QTranslator::tr( "no" ) );
    visibility->setTextAlignment( Qt::AlignCenter );

    QStandardItem * created = new QStandardItem();
    created->setText( note->created().toString( Qt::SystemLocaleLongDate ) );

    QStandardItem * modified = new QStandardItem();
    modified->setText( note->modified().toString( Qt::SystemLocaleLongDate ) );

    QStandardItem * top = new QStandardItem();
    top->setText( note->isTop() ? QTranslator::tr( "yes" ) : QTranslator::tr( "no" ) );
    top->setTextAlignment( Qt::AlignCenter );

    QStandardItem * attachments = new QStandardItem();
    attachments->setText( QString::number( note->numberOfAttachments() ) );
    attachments->setTextAlignment( Qt::AlignCenter );

    return QList < QStandardItem * > () << title << visibility << created << modified << top << attachments;
}
QList < QStandardItem * > toStandardItems( const QString & path )
{
    QSettings ini( path + "/" + "settings.ini", QSettings::IniFormat );
    ini.setIniCodec( "utf8" );

    QStandardItem * title = new QStandardItem();
    title->setText( ini.value( "Title" ).toString() );
    title->setToolTip( title->text() );

    QStandardItem * visibility = new QStandardItem();
    visibility->setText( ini.value( "Visible" ).toBool() ? QTranslator::tr( "yes" ) : QTranslator::tr( "no" ) );
    visibility->setTextAlignment( Qt::AlignCenter );

    QStandardItem * created = new QStandardItem();
    created->setText( ini.value( "Created" ).toDateTime().toString( Qt::SystemLocaleLongDate ) );

    QStandardItem * modified = new QStandardItem();
    modified->setText( ini.value( "Modified" ).toDateTime().toString( Qt::SystemLocaleLongDate ) );

    QStandardItem * top = new QStandardItem();
    top->setText( ini.value( "Top" ).toBool() ? QTranslator::tr( "yes" ) : QTranslator::tr( "no" ) );
    top->setTextAlignment( Qt::AlignCenter );

    QStandardItem * attachments = new QStandardItem();
    attachments->setText( QString::number( QDir( path + "/" + "attach" ).entryList( QDir::Files ).size() ) );
    attachments->setTextAlignment( Qt::AlignCenter );

    return QList < QStandardItem * > () << title << visibility << created << modified << top << attachments;
}
QStandardItem * findItem( RichTextNote * note, QStandardItemModel * model, int column )
{
    for ( int row = 0; row < model->rowCount(); row++ )
    {
        QStandardItem * item = model->item( row, column );
        if ( note == toNote( item ) )
            return item;
    }

    return 0;
}

Manager::Manager( QWidget * parent ) :
    QMainWindow( parent ),
    ui( new Ui::Manager ),
    settings( 0 )
{
    self = this;       

    QStringList labels;
    labels << tr( "Title" ) << tr( "Visibility" ) << tr( "Created" )
           << tr( "Modified" ) << tr( "On top of all windows" )
           << tr( "Number of attachments" );
    model.setHorizontalHeaderLabels( labels );

    ui->setupUi( this );

    leftPanel = new FancyTabBar( TabBarPosition::Left );
    leftPanel->addTab( QIcon( ":/notebook" ), tr( "Notes" ) );
    leftPanel->addTab( QIcon( ":/trash" ),    tr( "Trash" ) );
    leftPanel->addTab( QIcon( ":/settings" ), tr( "Settings" ) );
    leftPanel->addTab( QIcon( ":/about" ),    tr( "About" ) );
    leftPanel->addTab( QIcon( ":/quit" ),     tr( "Quit" ) );
    leftPanel->setCurrentIndex( 0 );
    connect( leftPanel, SIGNAL( currentChanged(int) ), SLOT( buttonLeftPanelClicked(int) ) );

    ui->panel->setLayout( new QVBoxLayout() );
    ui->panel->layout()->setMargin( 0 );
    ui->panel->layout()->addWidget( leftPanel );
    ui->panel->setSizePolicy( QSizePolicy::Fixed, ui->panel->sizePolicy().verticalPolicy() );

    sortModel = new QSortFilterProxyModel( this );
    sortModel->setSourceModel( &model );

    pageNotes = new Page_Notes( this, sortModel );
    pageTrash = new Page_Trash( labels );
    pageSettings = new Page_Settings();
    pageAbout = new Page_About();

    connect( pageNotes, SIGNAL( changeSelection() ), SLOT( updateStates() ) );
    connect( pageSettings, SIGNAL( message(QString, int) ), ui->statusBar, SLOT( showMessage(QString, int) ) );
    connect( pageSettings, SIGNAL( acceptChangeSettings() ), SLOT( acceptChangeSettings() ) );

    ui->stackedWidget_Pages->addWidget( pageNotes );
    ui->stackedWidget_Pages->addWidget( pageTrash );
    ui->stackedWidget_Pages->addWidget( pageSettings );
    ui->stackedWidget_Pages->addWidget( pageAbout );
    ui->stackedWidget_Pages->setCurrentIndex( leftPanel->currentIndex() );

    createMenu();
    createToolBars();
    createTray();

    connect( &autoSaveTimer, SIGNAL( timeout() ), SLOT( writeSettings() ) );

    updateStates();
}

Manager::~Manager()
{
    blockSignals( true );
    for ( int i = 0; i < model.rowCount(); i++ )
    {
        RichTextNote * note = toNote( model.item( i, 0 ) );
        note->blockSignals( true ); // не даем возможность посылать сигналы - все равно удаляем
        note->save();
        note->deleteLater();
    }

    delete ui;
    self = 0;
}

void Manager::loadNotes()
{    
    sortModel->setDynamicSortFilter( false );

    foreach ( const QString & path, QDir( getNotesPath() ).entryList( QDir::Dirs | QDir::NoDotAndDotDot ) )
    {       
        RichTextNote * note = new RichTextNote();
        note->setFileName( QDir::fromNativeSeparators( getNotesPath() + "/" + path ) );
        note->load();
        note->setModified( false );
        connect( note, SIGNAL( changed(int) ), SLOT( noteChange(int) ) );
        model.appendRow( toStandardItems( note ) );
    }

    sortModel->setDynamicSortFilter( true );

    updateStates();
}
void Manager::setSettings( QSettings * s )
{
    settings = s;
    pageNotes->setSettings( settings );
    pageTrash->setSettings( settings );
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


    FormattingToolbar * formattingToolbar = new FormattingToolbar();
    formattingToolbar->setAlterActivityComponents( true );
    formattingToolbar->hide();
    formattingToolbar->setParent( this );
//    formattingToolbar->installConnect( pageNotes->editor() );

    QMenu * menuFormatting = ui->menuToolbars->addMenu( QIcon( "" ), tr( "Text formatting" ) );

    foreach ( QToolBar * toolbar, formattingToolbar->toolBars() )
    {
        toolbar->setAllowedAreas( allowedAreas );

        QAction * action = new QAction( QIcon( "" ), toolbar->windowTitle(), this );
        action->setCheckable( true );
        action->setChecked( toolbar->isVisible() );

        connect( action, SIGNAL( triggered(bool) ), toolbar, SLOT( setVisible(bool) ) );
        connect( toolbar, SIGNAL( visibilityChanged(bool) ), action, SLOT( setChecked(bool) ) );

        addToolBar( toolbar );
        menuFormatting->addAction( action );
    }


    foreach ( QDockWidget * dock, findChildren < QDockWidget * > () )
        dock->deleteLater();

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
//    connect( ui->actionReadOnlyNote, SIGNAL( triggered(bool) ), SLOT( setReadOnlyNote(bool) ) );

    // TODO: создать и добавить действия форматирования текста
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
        ui->stackedWidget_Pages->setCurrentWidget( pageTrash );
        break;

    case 2:
        ui->stackedWidget_Pages->setCurrentWidget( pageSettings );
        break;

    case 3:
        ui->stackedWidget_Pages->setCurrentWidget( pageAbout );
        pageAbout->setCurrentTab( Page_About::About );
        break;

    case 4:
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

    }else if ( reason == QSystemTrayIcon::MiddleClick )
        addNote();
}

void Manager::acceptChangeSettings()
{
    setActivateTimerAutosave( pageSettings->mapSettings[ "Autosave" ].toBool() );
    setIntervalAutosave( pageSettings->mapSettings[ "AutosaveInterval" ].toInt() );

    qApp->setOverrideCursor( Qt::WaitCursor );

    bool activateTimerAutosaveNotes = pageSettings->mapSettings[ "Notes_Autosave" ].toBool();
    int intervalTimerAutosaveNotes = pageSettings->mapSettings[ "Notes_AutosaveInterval" ].toInt();
    for ( int i = 0; i < model.rowCount(); i++ )
    {
        qApp->processEvents();
        RichTextNote * note = toNote( model.item( i, 0 ) );
        note->setActivateTimerAutosave( activateTimerAutosaveNotes );
        note->setIntervalAutosave( intervalTimerAutosaveNotes );
    }

    qApp->restoreOverrideCursor();
}

void Manager::updateStates()
{
    bool hasSelection = pageNotes->hasSelection();
    bool isEmpty = model.rowCount() == 0;
    bool isOpenDict = Completer::instance()->isAutocomplete();

    bool isModified = false;
    bool isVisibleSelectionNote = false;
    bool isTopSelectionNote = false;

    if ( hasSelection )
    {
        QAbstractItemModel * viewModel = pageNotes->getModel();

        int row = pageNotes->currentRow();        
        RichTextNote * note = toNote( viewModel->index( row, 0 ) );
        Q_ASSERT_X( note, "updateStates", "указатель на заметку пуст" );

        isVisibleSelectionNote = note->isVisible();
        isTopSelectionNote = note->isTop();
        isModified = note->isModified();
    }

    ui->tButtonRemoveNote->setEnabled( hasSelection );
    ui->tButtonSaveNoteAs->setEnabled( hasSelection );
    ui->tButtonSaveNote->setEnabled( hasSelection && isModified );
    ui->tButtonPrintNote->setEnabled( hasSelection );
    ui->tButtonPreviewPrintNote->setEnabled( hasSelection );
    ui->tButtonTopNote->setEnabled( hasSelection );
    ui->tButtonTopNote->setChecked( isTopSelectionNote );
    ui->tButtonShowNote->setEnabled( hasSelection && !isVisibleSelectionNote );
    ui->tButtonHideNote->setEnabled( hasSelection && isVisibleSelectionNote );
    ui->tButtonRemoveAllNotes->setEnabled( !isEmpty );
    ui->tButtonSaveAllNotes->setEnabled( !isEmpty );
    ui->tButtonShowAllNotes->setEnabled( !isEmpty );
    ui->tButtonHideAllNotes->setEnabled( !isEmpty );
    ui->tButtonOpenDict->setEnabled( !isOpenDict );
    ui->tButtonCloseDict->setEnabled( isOpenDict );
    ui->tButtonDuplicateNote->setEnabled( hasSelection );

    ui->actionRemoveNote->setEnabled( hasSelection );
    ui->actionSaveNoteAs->setEnabled( hasSelection );
    ui->actionSaveNote->setEnabled( hasSelection && isModified );
    ui->actionPrintNote->setEnabled( hasSelection );
    ui->actionPreviewPrintNote->setEnabled( hasSelection );
    ui->actionTopNote->setEnabled( hasSelection );
    ui->actionTopNote->setChecked( isTopSelectionNote );
    ui->actionShowNote->setEnabled( hasSelection && !isVisibleSelectionNote );
    ui->actionHideNote->setEnabled( hasSelection && isVisibleSelectionNote );
    actionRemoveAllNotes->setEnabled( !isEmpty );
    actionSaveAllNotes->setEnabled( !isEmpty );
    actionShowAllNotes->setEnabled( !isEmpty );
    actionHideAllNotes->setEnabled( !isEmpty );
    ui->actionOpenDict->setEnabled( !isOpenDict );
    ui->actionCloseDict->setEnabled( isOpenDict );
    ui->actionDuplicateNote->setEnabled( hasSelection );
}

void Manager::show_page_notes()
{
    show_Manager();
    leftPanel->setCurrentIndex( 0 );
}
void Manager::show_page_trash()
{
    show_Manager();
    leftPanel->setCurrentIndex( 1 );
}
void Manager::show_page_settings()
{
    show_Manager();
    leftPanel->setCurrentIndex( 2 );
}
void Manager::show_page_about()
{
    show_Manager();
    leftPanel->setCurrentIndex( 3 );
}
void Manager::show_page_documentation()
{
    if ( !QDesktopServices::openUrl( QUrl::fromLocalFile( qApp->applicationDirPath() + "/doc/ru/html/index.html" ) ) )
    {
        QMessageBox::information( this, tr( "Information" ), tr( "Unable to open documents" ) );
        qDebug() << tr( "Unable to open documents" );
    }
}
void Manager::open()
{
    const QString & fileName = QFileDialog::getOpenFileName( this, QString(), QString(), tr( "File notes" ) + QString( " ( *.%1 )\n" ).arg( getNoteFormat() ) );
    if ( fileName.isEmpty() )
        return;

    QString nameDirNote = "New note_" + QDateTime::currentDateTime().toString( "yyyy-MM-dd_hh-mm-ss__zzz" );
    QString absNameDirNote = getNotesPath() + "/" + nameDirNote;

    if ( !QDir().mkdir( absNameDirNote ) )
    {
        QMessageBox::information( this, tr( "Information" ), tr( "An error occurred while creating the directory" ) );
        qDebug() << tr( "An error occurred while creating the directory" );
        return;
    }

    if ( JlCompress::extractDir( fileName, absNameDirNote ).isEmpty() )
    {
        QMessageBox::information( this, tr( "Information" ), tr( "An error occurred while extracting the files and folders notes" ) );
        qDebug() << tr( "An error occurred while extracting the files and folders notes" );
        return;
    }

    RichTextNote * note = new RichTextNote();
    note->setFileName( QDir::fromNativeSeparators( absNameDirNote ) );
    note->load();
    note->setModified( false );
    connect( note, SIGNAL( changed(int) ), SLOT( noteChange(int) ) );
    model.appendRow( toStandardItems( note ) );
}
void Manager::addNote()
{
    qApp->setOverrideCursor( Qt::WaitCursor );

    RichTextNote * note = new RichTextNote();
    note->createNew();

    connect( note, SIGNAL( changed(int) ), SLOT( noteChange(int) ) );
    model.appendRow( toStandardItems( note ) );
    updateStates();

    qApp->restoreOverrideCursor();
}
void Manager::addNoteFromClipboard()
{
    qApp->setOverrideCursor( Qt::WaitCursor );

    RichTextNote * note = new RichTextNote();
    note->createNew( false );
    note->setText( qApp->clipboard()->text() );
    note->save();

    connect( note, SIGNAL( changed(int) ), SLOT( noteChange(int) ) );
    model.appendRow( toStandardItems( note ) );
    updateStates();

    qApp->restoreOverrideCursor();
}
void Manager::addNoteFromScreen()
{    
    FullscreenshotCropper cropper;
    cropper.setImage( QPixmap::grabWindow( QApplication::desktop()->winId() ) );
    cropper.showFullScreen();
    if ( !cropper.exec() )
        return;

    const QPixmap & screenshot = cropper.cropperImage();

    qApp->setOverrideCursor( Qt::WaitCursor );

    RichTextNote * note = new RichTextNote();
    note->createNew( false );
    note->insertImage( screenshot );
    note->save();

    connect( note, SIGNAL( changed(int) ), SLOT( noteChange(int) ) );
    model.appendRow( toStandardItems( note ) );
    updateStates();

    qApp->restoreOverrideCursor();
}
void Manager::removeAllNotes()
{
    QMessageBox::StandardButton result = QMessageBox::question( this, tr( "Question" ), tr( "Delete all notes?" ), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );
    if ( result == QMessageBox::No )
        return;

    for ( int i = model.rowCount() - 1; i >= 0; i-- )
    {
        qApp->processEvents();
        toNote( model.item( i, 0 ) )->invokeRemove();
    }
}
void Manager::saveAllNotes()
{
    qApp->setOverrideCursor( Qt::WaitCursor );

    for ( int i = 0; i < model.rowCount(); i++ )
    {
        qApp->processEvents();
        toNote( model.item( i, 0 ) )->save();
    }

    qApp->restoreOverrideCursor();
}
void Manager::showAllNotes()
{
    qApp->setOverrideCursor( Qt::WaitCursor );

    for ( int i = 0; i < model.rowCount(); i++ )
    {
        qApp->processEvents();

        RichTextNote * note = toNote( model.item( i, 0 ) );
        if ( note->isVisible() )
            continue;
        else
            note->show();
    }

    qApp->restoreOverrideCursor();
}
void Manager::hideAllNotes()
{
    qApp->setOverrideCursor( Qt::WaitCursor );

    for ( int i = 0; i < model.rowCount(); i++ )
    {
        qApp->processEvents();

        RichTextNote * note = toNote( model.item( i, 0 ) );
        if ( !note->isVisible()  )
            continue;
        else
            note->hide();
    }

    qApp->restoreOverrideCursor();
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

void Manager::removeNote()
{
    QAbstractItemModel * viewModel = pageNotes->getModel();

    int row = pageNotes->currentRow();
    toNote( viewModel->index( row, 0 ) )->invokeRemove();
}
void Manager::duplicateNote()
{
    QAbstractItemModel * viewModel = pageNotes->getModel();

    int row = pageNotes->currentRow();
    toNote( viewModel->index( row, 0 ) )->duplicate();
}

void Manager::saveNote()
{
    QAbstractItemModel * viewModel = pageNotes->getModel();

    int row = pageNotes->currentRow();
    toNote( viewModel->index( row, 0 ) )->save();
}
void Manager::saveAsNote()
{
    QAbstractItemModel * viewModel = pageNotes->getModel();

    int row = pageNotes->currentRow();
    toNote( viewModel->index( row, 0 ) )->saveAs();
}
void Manager::showNote()
{
    QAbstractItemModel * viewModel = pageNotes->getModel();

    int row = pageNotes->currentRow();
    toNote( viewModel->index( row, 0 ) )->show();
}
void Manager::hideNote()
{
    QAbstractItemModel * viewModel = pageNotes->getModel();

    int row = pageNotes->currentRow();
    toNote( viewModel->index( row, 0 ) )->hide();
}
void Manager::printNote()
{
    QAbstractItemModel * viewModel = pageNotes->getModel();

    int row = pageNotes->currentRow();
    toNote( viewModel->index( row, 0 ) )->print();
}
void Manager::previewPrintNote()
{
    QAbstractItemModel * viewModel = pageNotes->getModel();

    int row = pageNotes->currentRow();
    toNote( viewModel->index( row, 0 ) )->previewPrint();
}
void Manager::setTopNote( bool top )
{
    QAbstractItemModel * viewModel = pageNotes->getModel();

    int row = pageNotes->currentRow();
    toNote( viewModel->index( row, 0 ) )->setTop( top );
}

void Manager::noteChange( int index )
{
    RichTextNote * note = qobject_cast < RichTextNote * > ( sender() );
    Q_ASSERT_X( note, "noteChange", "отправителем сигнала оказалась не заметка" );

    QStandardItem * item = findItem( note, &model );
    if ( !item )
        return;

    int row = item->row();

    switch ( index )
    {
    case EventsNote::Remove:
    {
//        pageNotes->clearContentNote();
//        pageNotes->selectionModel->clearSelection();

        const QString & dirNote = note->fileName();
        if ( !copyPath2( dirNote, getTrashPath() + "/" + QFileInfo( dirNote ).fileName() ) )
        {
            qDebug() << "ошибка при копировании в корзину";
            return;
        }

//        // локальное удаление папки с заметками
//        note->blockSignals( true );
//        note->remove();

        pageTrash->loadNotes();

        model.removeRow( row );

//        const QModelIndex index1 = model.index( pageNotes->currentRow(), 0 );
//        const QModelIndex index2 = model.index( pageNotes->currentRow(), pageNotes->getModel()->columnCount() - 1 );
//        pageNotes->selectionModel->select( QItemSelection( index1, index2 ), QItemSelectionModel::Select );
//        pageNotes->displayContentNote( pageNotes->selectionModel->currentIndex().row() );

        updateStates();        
        break;
    }
    case EventsNote::ChangeTitle:
    {
        model.item( row, Columns::Title )->setText( note->title() );
        updateStates();
        break;
    }
    case EventsNote::ChangeVisibility:
    {
        model.item( row, Columns::Visibility )->setText( note->isVisible() ? QTranslator::tr( "yes" ) : QTranslator::tr( "no" ) );
        updateStates();
        break;
    }
    case EventsNote::ChangeText:
    {
        model.item( row, Columns::Modified )->setText( note->modified().toString( Qt::SystemLocaleLongDate ) );
        updateStates();
        break;
    }
    case EventsNote::ChangeTop:
    {
        model.item( row, Columns::Top )->setText( note->isTop() ? QTranslator::tr( "yes" ) : QTranslator::tr( "no" ) );
        updateStates();
        break;
    }
    case EventsNote::SaveEnded:
    {
        updateStates();
        break;
    }
    case EventsNote::Duplicate:
    {
        const QString & oldPath = note->fileName();
        const QString & oldName = QFileInfo( oldPath ).fileName();
        const QString & newName = oldName + " " + tr( "copy" );
        QString newPath = getNotesPath() + QDir::separator() + newName;

        // проверим, есть ли заметка с таким названием
        bool successfull = false;
        for( int i = 0; i < model.rowCount(); i++ )
            if ( model.item( i, 0 )->text() == newName )
            {
                successfull = true;
                break;
            }

        // если такая заметка уже есть, добавим текущую дату/время чтоб избежать коллизий
        if ( successfull )
            newPath +=  "_" + tr( "from" ) + "_" + QDateTime::currentDateTime().toString( "yyyy-MM-dd_hh-mm-ss__zzz" );

        copyPathE( oldPath, newPath );
        RichTextNote * duplicate = new RichTextNote();
        duplicate->setFileName( newPath );
        duplicate->load();
        duplicate->save();

        connect( duplicate, SIGNAL( changed(int) ), SLOT( noteChange(int) ) );
        model.appendRow( toStandardItems( duplicate ) );
        updateStates();
        break;
    }
    case EventsNote::ChangeAttach:
    {
        model.item( row, Columns::Attachments )->setText( QString::number( note->numberOfAttachments() ) );
        updateStates();
        break;
    }

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
    pageTrash->readSettings();
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
    pageTrash->writeSettings();
    pageSettings->writeSettings();

    settings->sync();

    ui->statusBar->showMessage( tr( "Save completed" ), 5000 );
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
