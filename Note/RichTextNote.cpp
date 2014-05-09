#include "RichTextNote.h"

#include <QVBoxLayout>
#include <QInputDialog>
#include <QFontDialog>
#include <QColorDialog>
#include <QDebug>
#include <QAction>
#include <QSettings>
#include <QPrinter>
#include <QPrintPreviewDialog>
#include <QPrintDialog>
#include <QKeySequence>
#include <QDir>
#include <QPluginLoader>
#include <QDockWidget>
#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <QFile>
#include <QFileDialog>
#include <QTextCodec>
#include <QImageWriter>
#include <QTextDocumentWriter>
#include <QMessageBox>
#include <QScrollArea>
#include <QDesktopServices>
#include <QDateTime>
#include <QPropertyAnimation>
#include <QDesktopWidget>

#include "utils/func.h"
#include "JlCompress.h"
#include "FormattingToolbar.h"
#include "utils/texttemplateparser.h"

static QActionGroup * createGroupActionsOpacity( QObject * parent = 0 )
{
    QActionGroup * group = new QActionGroup( parent );
    for ( qreal op = nm_Note::maximalOpacity; op > nm_Note::minimalOpacity; op -= 0.1 )
    {
        uchar proc = op * 100;
        const QString & text = QString( "%1%" ).arg( proc );
        const QIcon & icon = QIcon( QString( ":/Note/opacity-%1" ).arg( proc ) );
        const QVariant data( op );

        group->addAction( Create::Action::get( text, icon, data ) );
    }
    group->addAction( Create::Action::get( QObject::tr( "Other" ), QIcon( "" ), QVariant( -1.0 ) ) );

    return group;
}

QVariantMap RichTextNote::defaultMapSettings;

RichTextNote::RichTextNote( const QString & fileName, QWidget * parent )
    : AbstractNote( parent ),
      d( new d_RichTextNote )
{    
    setFileName( fileName );
    init(); 
}
RichTextNote::RichTextNote( QWidget * parent )
    : AbstractNote( parent ),
      d( new d_RichTextNote )
{
    init();
}
RichTextNote::~RichTextNote()
{
    delete d;
}

QTextDocument * RichTextNote::document()
{
    return d->editor.document();
}
TextEditor * RichTextNote::textEditor()
{
    return &d->editor;
}

QDateTime RichTextNote::created()
{
    return mapSettings[ "Created" ].toDateTime();
}
QDateTime RichTextNote::modified()
{
    return mapSettings[ "Modified" ].toDateTime();
}

void RichTextNote::createNew( bool bsave )
{
    QString path = "Note_" + QDateTime::currentDateTime().toString( "yyyy-MM-dd_hh-mm-ss__zzz" );
    setFileName( QDir::fromNativeSeparators( getNotesPath() + "/" + path ) );

    load();

    if ( bsave )
        save();
}

QString RichTextNote::fileName()
{
    return d->noteFileName;
}
QString RichTextNote::attachDirPath()
{
    return QDir::fromNativeSeparators( d->noteFileName + "/attach" );
}
QString RichTextNote::contentFilePath()
{
    return QDir::fromNativeSeparators( d->noteFileName + "/content.html" );
}
QString RichTextNote::settingsFilePath()
{
    return QDir::fromNativeSeparators( d->noteFileName + "/settings.ini" );
}

void RichTextNote::setFileName( const QString & dirName )
{    
    d->noteFileName = dirName;

    if ( QDir( d->noteFileName ).exists() )
        return;

    // Создадим папку заметки с нужными папками и файлами
    QDir().mkdir( d->noteFileName );
    QDir().mkdir( attachDirPath() );
    QFile( contentFilePath() ).open( QIODevice::WriteOnly );
    QFile( settingsFilePath() ).open( QIODevice::WriteOnly );
}

void RichTextNote::setDefaultSettingsFromMap( const QVariantMap & s )
{
    defaultMapSettings[ "Top" ] = s.value( "NewNote_Top" );
    defaultMapSettings[ "ColorTitle" ] = s.value( "NewNote_ColorTitle" );
    defaultMapSettings[ "ColorBody" ] = s.value( "NewNote_ColorBody" );
    defaultMapSettings[ "Opacity" ] = s.value( "NewNote_Opacity" );
    defaultMapSettings[ "Visible" ] = s.value( "NewNote_Visible" );
    defaultMapSettings[ "Title" ] = s.value( "NewNote_Title" );
    defaultMapSettings[ "Text" ] = s.value( "NewNote_Text" );
    defaultMapSettings[ "FontTitle" ] = s.value( "NewNote_FontTitle" );
    defaultMapSettings[ "Position" ] = s.value( "NewNote_Position" );
    defaultMapSettings[ "Size" ] = s.value( "NewNote_Size" );
    defaultMapSettings[ "RandomColor" ] = s.value( "NewNote_RandomColor" );
    defaultMapSettings[ "RandomPositionOnScreen" ] = s.value( "NewNote_RandomPositionOnScreen" );
    defaultMapSettings[ "Autosave" ] = s.value( "Notes_Autosave" );
    defaultMapSettings[ "AutosaveInterval" ] = s.value( "Notes_AutosaveInterval" );
    defaultMapSettings[ "ActionDoubleClickOnTitle" ] = s.value( "Notes_ActionDoubleClickOnTitle" );
}

void RichTextNote::init()
{
    setupActions();
    setupGUI();

    updateStates();

    // NOTE: лучше сделать или по умолчанию скрытым, или с настраиваемой видимостью
    setVisibleToolBar( false );
    quickFind->setVisible( false );

    QObject::connect( &d->timerAutosave, SIGNAL( timeout() ), SLOT( save() ) );
    QObject::connect( this, SIGNAL( doubleClickHead() ), SLOT( doubleClickingOnTitle() ) );
}
void RichTextNote::setupActions()
{
    actionVisibleToolBar = Create::Action::get( tr( "Toolbar" ), QIcon( "" ), true );
    actionVisibleQuickFind = Create::Action::get( tr( "Quick find" ), QIcon( "" ), true );
    actionVisibleAttachPanel = Create::Action::get( tr( "Attach panel" ), QIcon( ":/Note/attach-panel" ), true );
    actionVisibleFindAndReplace = Create::Action::get( tr( "Find/replace" ), QIcon( ":/Note/find-replace" ), true );
    actionVisibleFormattingToolbar = Create::Action::get( tr( "Formatting toolbar" ), QIcon( ":/FormattingToolbar/formating" ), true );

    QAction * actionDelete = Create::Action::triggered( tr( "Delete" ), QIcon( ":/Note/remove" ), QKeySequence(), this, SLOT( invokeRemove() ) );
    QAction * actionSetTitle = Create::Action::triggered( tr( "Set title" ), QIcon( ":/Note/title" ), QKeySequence(), this, SLOT( selectTitle() ) );
    QAction * actionSetTitleFont = Create::Action::triggered( tr( "Set title font" ), QIcon( ":/Note/title-font" ), QKeySequence(), this, SLOT( selectTitleFont() ) );
    QAction * actionSetTitleColor = Create::Action::triggered( tr( "Set title color" ), QIcon( ":/Note/title-color" ), QKeySequence(), this, SLOT( selectTitleColor() ) );
    QAction * actionSetColor = Create::Action::triggered( tr( "Set window color" ), QIcon::fromTheme( "", QIcon( ":/Note/color" ) ), QKeySequence(), this, SLOT( selectColor() ) );
    QAction * actionHide = Create::Action::triggered( tr( "Hide" ), QIcon::fromTheme( "", QIcon( ":/Note/hide" ) ), QKeySequence(), this, SLOT( hide() ) );
    actionSetTopBottom = Create::Action::bTriggered( tr( "On top of all windows" ), QIcon::fromTheme( "", QIcon( ":/Note/tacks" ) ), QKeySequence(), this, SLOT( setTop( bool ) ) );
    QAction * actionOpen = Create::Action::triggered( tr( "Open" ), QIcon::fromTheme( "", QIcon( ":/Note/open" ) ), QKeySequence( QKeySequence::Open ), this, SLOT( open() ) );
    QAction * actionSaveAs = Create::Action::triggered( tr( "Save as" ), QIcon::fromTheme( "", QIcon( ":/Note/save-as" ) ), QKeySequence( QKeySequence::SaveAs ), this, SLOT( saveAs() ) );
#ifndef QT_NO_PRINTER
    QAction * actionPrint = Create::Action::triggered( tr( "Print" ), QIcon::fromTheme( "", QIcon( ":/Note/print" ) ), QKeySequence( QKeySequence::Print ), this, SLOT( print() ) );
    QAction * actionPreviewPrint = Create::Action::triggered( tr( "Preview print" ), QIcon::fromTheme( "", QIcon( ":/Note/preview-print" ) ), QKeySequence(), this, SLOT( previewPrint() ) );
#endif
    actionSave = Create::Action::triggered( tr( "Save" ), QIcon::fromTheme( "", QIcon( ":/Note/save" ) ), QKeySequence( QKeySequence::Save ), this, SLOT( save() ) );
    QAction * actionAttach = Create::Action::triggered( tr( "Attach" ), QIcon::fromTheme( "", QIcon( ":/Note/attach" ) ), QKeySequence(), this, SLOT( selectAttach() ) );

    QObject::connect( actionVisibleToolBar, SIGNAL( triggered(bool) ), SLOT( setVisibleToolBar(bool) ) );
    QObject::connect( this, SIGNAL( changeVisibleToolbar(bool) ), actionVisibleToolBar, SLOT( setChecked(bool) ) );


    QActionGroup * group = createGroupActionsOpacity( this );
    QObject::connect( group, SIGNAL( triggered(QAction*) ), SLOT( changeOpacity(QAction*)) );

    QMenu * menuOpacity = new QMenu( tr( "Opacity" ) );
    menuOpacity->setIcon( QIcon( ":/Note/opacity" ) );
    menuOpacity->addActions( group->actions() );


    addAction( actionVisibleFormattingToolbar );
    addAction( actionVisibleAttachPanel );
    addAction( actionVisibleFindAndReplace );
    addAction( actionVisibleToolBar );
    addAction( actionVisibleQuickFind );
    addSeparator();
    addAction( actionSetTitle );
    addAction( actionSetTitleFont );
    addAction( actionSetTitleColor );
    addSeparator();
    addMenu( menuOpacity );
    addAction( actionSetColor );
    addSeparator();
    addAction( actionSetTopBottom );
    addSeparator();
    addAction( actionOpen );
    addAction( actionSave );
    addAction( actionSaveAs );
    addSeparator();
    addAction( actionPrint );
    addAction( actionPreviewPrint );
    addSeparator();
    addAction( actionAttach );
    addSeparator();
    addAction( actionHide );
    addAction( actionDelete );    
}
void RichTextNote::setupGUI()
{
    Qt::DockWidgetAreas areas = Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea;

    FormattingToolbar * formattingToolbar = new FormattingToolbar();
    formattingToolbar->installConnect( &d->editor );
    formattingToolbar->setNote( this );

    dockWidgetFormattingToolbar = new QDockWidget();
    dockWidgetFormattingToolbar->setAllowedAreas( areas );
    dockWidgetFormattingToolbar->setWidget( formattingToolbar );
    dockWidgetFormattingToolbar->setWindowTitle( formattingToolbar->windowTitle() );
    dockWidgetFormattingToolbar->setVisible( false );

    tButtonVisibleFormattingToolbar = Create::Button::get( this, tr( "Formatting toolbar" ), QIcon( ":/FormattingToolbar/formating" ), true );
    QObject::connect( tButtonVisibleFormattingToolbar, SIGNAL( clicked(bool) ), dockWidgetFormattingToolbar, SLOT( setVisible(bool) ) );
    QObject::connect( dockWidgetFormattingToolbar, SIGNAL( visibilityChanged(bool) ), tButtonVisibleFormattingToolbar, SLOT( setChecked(bool) ) );

    QObject::connect( actionVisibleFormattingToolbar, SIGNAL( triggered(bool) ), dockWidgetFormattingToolbar, SLOT( setVisible(bool) ) );
    QObject::connect( dockWidgetFormattingToolbar, SIGNAL( visibilityChanged(bool) ), actionVisibleFormattingToolbar, SLOT( setChecked(bool) ) );

    body->addDockWidget( Qt::RightDockWidgetArea, dockWidgetFormattingToolbar );

    tButtonVisibleFormattingToolbar->setChecked( dockWidgetFormattingToolbar->isVisible() );
    actionVisibleFormattingToolbar->setChecked( dockWidgetFormattingToolbar->isVisible() );
    
    
    attachPanel = new AttachPanel();
    attachPanel->setViewTo( this );

    dockWidgetAttachPanel = new QDockWidget();
    dockWidgetAttachPanel->setAllowedAreas( areas );
    dockWidgetAttachPanel->setWidget( attachPanel );
    dockWidgetAttachPanel->setWindowTitle( attachPanel->windowTitle() );
    dockWidgetAttachPanel->setVisible( false );

    tButtonVisibleAttachPanel = Create::Button::get( this, tr( "Attach panel" ), QIcon( ":/Note/attach-panel" ), true );
    QObject::connect( tButtonVisibleAttachPanel, SIGNAL( clicked(bool) ), dockWidgetAttachPanel, SLOT( setVisible(bool) ) );
    QObject::connect( dockWidgetAttachPanel, SIGNAL( visibilityChanged(bool) ), tButtonVisibleAttachPanel, SLOT( setChecked(bool) ) );

    QObject::connect( actionVisibleAttachPanel, SIGNAL( triggered(bool) ), dockWidgetAttachPanel, SLOT( setVisible(bool) ) );
    QObject::connect( dockWidgetAttachPanel, SIGNAL( visibilityChanged(bool) ), actionVisibleAttachPanel, SLOT( setChecked(bool) ) );

    body->addDockWidget( Qt::RightDockWidgetArea, dockWidgetAttachPanel );

    tButtonVisibleAttachPanel->setChecked( dockWidgetAttachPanel->isVisible() );
    actionVisibleAttachPanel->setChecked( dockWidgetAttachPanel->isVisible() );
    
    
    findAndReplace = new FindAndReplace( &d->editor );

    dockWidgetFindAndReplace = new QDockWidget();
    dockWidgetFindAndReplace->setAllowedAreas( areas );
    dockWidgetFindAndReplace->setWidget( findAndReplace );
    dockWidgetFindAndReplace->setWindowTitle( findAndReplace->windowTitle() );
    dockWidgetFindAndReplace->setVisible( false );

    tButtonVisibleFindAndReplace = Create::Button::get( this, tr( "Find/replace" ), QIcon( ":/Note/find-replace" ), true );
    QObject::connect( tButtonVisibleFindAndReplace, SIGNAL( clicked(bool) ), dockWidgetFindAndReplace, SLOT( setVisible(bool) ) );
    QObject::connect( dockWidgetFindAndReplace, SIGNAL( visibilityChanged(bool) ), tButtonVisibleFindAndReplace, SLOT( setChecked(bool) ) );

    QObject::connect( actionVisibleFindAndReplace, SIGNAL( triggered(bool) ), dockWidgetFindAndReplace, SLOT( setVisible(bool) ) );
    QObject::connect( dockWidgetFindAndReplace, SIGNAL( visibilityChanged(bool) ), actionVisibleFindAndReplace, SLOT( setChecked(bool) ) );

    body->addDockWidget( Qt::RightDockWidgetArea, dockWidgetFindAndReplace );

    tButtonVisibleFindAndReplace->setChecked( dockWidgetFindAndReplace->isVisible() );
    actionVisibleFindAndReplace->setChecked( dockWidgetFindAndReplace->isVisible() );
    

    quickFind = new QuickFind( &d->editor );
    QObject::connect( actionVisibleQuickFind, SIGNAL( triggered(bool) ), quickFind, SLOT( setVisible(bool) ) );
    QObject::connect( quickFind, SIGNAL( visibilityChanged(bool) ), actionVisibleQuickFind, SLOT( setChecked(bool) ) );
    actionVisibleQuickFind->setChecked( quickFind->isVisible() );

    d->editor.setFrameStyle( QFrame::NoFrame );
    d->editor.viewport()->setAutoFillBackground( false );
    d->editor.setStyleSheet( nm_Note::style );

    QVBoxLayout * mainLayout = new QVBoxLayout();
    mainLayout->setSpacing( 0 );
    mainLayout->setContentsMargins( 5, 0, 5, 0 );
    mainLayout->addWidget( &d->editor );
    mainLayout->addWidget( quickFind );

    body->setWidget( new QWidget() );
    body->widget()->setLayout( mainLayout );
    body->setToolBarIconSize( QSize( 15, 15 ) );

    QToolButton * tButtonDelete = Create::Button::clicked( tr( "Delete" ), QIcon::fromTheme( "", QIcon( ":/Note/remove" ) ), this, SLOT( invokeRemove() ) );
    QToolButton * tButtonSetTitle = Create::Button::clicked( tr( "Set title" ), QIcon::fromTheme( "", QIcon( ":/Note/title" ) ), this, SLOT( selectTitle() ) );
    QToolButton * tButtonSetTitleFont = Create::Button::clicked( tr( "Set title font" ), QIcon::fromTheme( "", QIcon( ":/Note/title-font" ) ), this, SLOT( selectTitleFont() ) );
    QToolButton * tButtonSetTitleColor = Create::Button::clicked( tr( "Set title color" ), QIcon::fromTheme( "", QIcon( ":/Note/title-color" ) ), this, SLOT( selectTitleColor() ) );
    QToolButton * tButtonSetColor = Create::Button::clicked( tr( "Set window color" ),QIcon::fromTheme( "", QIcon( ":/Note/color" ) ), this, SLOT( selectColor() ) );
    QToolButton * tButtonSetOpacity = Create::Button::clicked( tr( "Opacity" ), QIcon::fromTheme( "", QIcon( ":/Note/opacity" ) ), this, SLOT( selectOpacity() ) );
    QToolButton * tButtonHide = Create::Button::clicked( tr( "Hide" ), QIcon::fromTheme( "", QIcon( ":/Note/hide" ) ), this, SLOT( hide() ) );
    tButtonSetTopBottom = Create::Button::bClicked( tr( "On top of all windows" ), QIcon::fromTheme( "", QIcon( ":/Note/tacks" ) ), this, SLOT( setTop( bool ) ) );
    QToolButton * tButtonOpen = Create::Button::clicked( tr( "Open" ), QIcon::fromTheme( "", QIcon( ":/Note/open" ) ), this, SLOT( open() ) );
    QToolButton * tButtonSaveAs = Create::Button::clicked( tr( "Save as" ), QIcon::fromTheme( "", QIcon( ":/Note/save-as" ) ), this, SLOT( saveAs() ) );
    QToolButton * tButtonPrint = Create::Button::clicked( tr( "Print" ),QIcon::fromTheme( "", QIcon( ":/Note/print" ) ), this, SLOT( print() ) );
    QToolButton * tButtonPreviewPrint = Create::Button::clicked( tr( "Preview print" ), QIcon::fromTheme( "", QIcon( ":/Note/preview-print" ) ), this, SLOT( previewPrint() ) );
    tButtonSave = Create::Button::clicked( tr( "Save" ), QIcon::fromTheme( "", QIcon( ":/Note/save" ) ), this, SLOT( save() ) );
    QToolButton * tButtonAttach = Create::Button::clicked( tr( "Attach" ), QIcon::fromTheme( "", QIcon( ":/Note/attach" ) ), this, SLOT( selectAttach() ) );


    body->addWidgetToToolBar( tButtonHide );
    body->addSeparator();
    body->addWidgetToToolBar( tButtonVisibleFormattingToolbar );
    body->addWidgetToToolBar( tButtonVisibleAttachPanel );
    body->addWidgetToToolBar( tButtonVisibleFindAndReplace );
    body->addSeparator();
    body->addWidgetToToolBar( tButtonSetTitle );
    body->addWidgetToToolBar( tButtonSetTitleFont );
    body->addWidgetToToolBar( tButtonSetTitleColor );
    body->addSeparator();
    body->addWidgetToToolBar( tButtonSetColor );
    body->addWidgetToToolBar( tButtonSetOpacity );
    body->addSeparator();
    body->addWidgetToToolBar( tButtonSetTopBottom );
    body->addSeparator();
    body->addWidgetToToolBar( tButtonOpen );
    body->addWidgetToToolBar( tButtonSaveAs );
    body->addWidgetToToolBar( tButtonSave );
    body->addSeparator();
    body->addWidgetToToolBar( tButtonAttach );
    body->addSeparator();
    body->addWidgetToToolBar( tButtonPrint );
    body->addWidgetToToolBar( tButtonPreviewPrint );
    body->addSeparator();
    body->addWidgetToToolBar( tButtonDelete );

    QActionGroup * group = createGroupActionsOpacity( this );
    QObject::connect( group, SIGNAL( triggered(QAction*) ), SLOT( changeOpacity(QAction*)) );

    QMenu * menuOpacity = new QMenu( tr( "Opacity" ) );
    menuOpacity->setIcon( QIcon( ":/Note/opacity" ) );
    menuOpacity->addActions( group->actions() );

    tButtonSetOpacity->setMenu( menuOpacity );
    tButtonSetOpacity->setPopupMode( QToolButton::MenuButtonPopup );


    QObject::connect( d->editor.document(), SIGNAL( contentsChanged() ), SLOT( contentsChanged() ) );
}

void RichTextNote::save()
{
    mapSettings[ "Top" ] = isTop();
    mapSettings[ "ColorTitle" ] = titleColor().name();
    mapSettings[ "ColorBody" ] = bodyColor().name();
    mapSettings[ "Opacity" ] = opacity();
    mapSettings[ "Visible" ] = isVisible();
    mapSettings[ "Title" ] = title();
    mapSettings[ "FontTitle" ] = titleFont().toString();
    mapSettings[ "Position" ] = pos();
    mapSettings[ "Size" ] = size();

    QSettings ini( settingsFilePath(), QSettings::IniFormat );
    ini.setIniCodec( "utf8" );
    ini.setValue( "Settings", mapSettings );

    saveContent();
    statusBar()->showMessage( tr( "Save completed" ), 5000 );
    emit changed( EventsNote::SaveEnded );
}
void RichTextNote::load()
{
    QSettings ini( settingsFilePath(), QSettings::IniFormat );
    ini.setIniCodec( "utf8" );
    mapSettings = ini.value( "Settings" ).toMap();

    QString _title;
    QFont _fontTitle;
    QSize _size;
    QPoint _position;
    QColor _titleColor;
    QColor _bodyColor;
    bool _top;
    qreal _opacity;
    bool _visible;

    // Если пуст, значит эта заметка новая -> берем параметры из дэфолтных настроек
    if ( mapSettings.isEmpty() )
    {
        const QDateTime & currentDateTime = QDateTime::currentDateTime();
        const QRect & desktop = QDesktopWidget().geometry();
        bool randomPosition = defaultMapSettings[ "RandomPositionOnScreen" ].toBool();
        bool randomColor = defaultMapSettings[ "RandomColor" ].toBool();

        mapSettings[ "Created" ] = currentDateTime;
        mapSettings[ "Modified" ] = currentDateTime;

        _title = TextTemplateParser::get( defaultMapSettings[ "Title" ].toString() );
        _fontTitle.fromString( defaultMapSettings[ "FontTitle" ].toString() );
        _size = defaultMapSettings[ "Size" ].toSize();

        // TODO: доработать с учетом размера заметок, т.к. новые заметки может разместить за экран
        if ( randomPosition )
            _position = QPoint( qrand() % desktop.width(), qrand() % desktop.height() );
        else
            _position = defaultMapSettings[ "Position" ].toPoint();

        if ( randomColor )
        {
            _titleColor.setRgb( qrand() % 0xff, qrand() % 0xff, qrand() % 0xff );
            _bodyColor.setRgb( qrand() % 0xff, qrand() % 0xff, qrand() % 0xff );
        } else
        {
            _titleColor = QColor( defaultMapSettings[ "ColorTitle" ].toString() );
            _bodyColor = QColor( defaultMapSettings[ "ColorBody" ].toString() );
        }

        _top = defaultMapSettings[ "Top" ].toBool();
        _opacity = defaultMapSettings.value( "Opacity" ).toDouble();
        _visible = defaultMapSettings.value( "Visible" ).toBool();

        setText( TextTemplateParser::get( defaultMapSettings[ "Text" ].toString() ) );

    } else
    {
        _title = mapSettings[ "Title" ].toString();
        _fontTitle.fromString( mapSettings[ "FontTitle" ].toString() );
        _size = mapSettings[ "Size" ].toSize();
        _position = mapSettings[ "Position" ].toPoint();
        _titleColor = QColor( mapSettings[ "ColorTitle" ].toString() );
        _bodyColor = QColor( mapSettings[ "ColorBody" ].toString() );
        _top = mapSettings[ "Top" ].toBool();
        _opacity = mapSettings[ "Opacity" ].toDouble();
        _visible = mapSettings.value( "Visible" ).toBool();

        loadContent();
    }

    setTitle( _title );
    setTitleFont( _fontTitle );
    resize( _size );
    move( _position );
    setTitleColor( _titleColor );
    setBodyColor( _bodyColor );
    setTop( _top );
    setOpacity( _opacity );
    setVisible( _visible );

    setActivateTimerAutosave( defaultMapSettings[ "Autosave" ].toBool() );
    setIntervalAutosave( defaultMapSettings[ "AutosaveInterval" ].toInt() );

    updateStates();
    emit changed( EventsNote::LoadEnded );
}
void RichTextNote::saveContent()
{
    QFile content( contentFilePath() );
    if ( !content.open( QIODevice::Truncate | QIODevice::WriteOnly ) )
    {
        QMessageBox::critical( this, tr( "Error" ), tr( "An error occurred saving notes" ) );
        return;
    }

    QTextStream in( &content );
    in.setCodec( "utf8" );
    in << text();
    content.close();

    updateStates();
}
void RichTextNote::loadContent()
{
   d->editor.setSource( QUrl::fromLocalFile( contentFilePath() ) );
}
void RichTextNote::setText( const QString & str )
{
    if ( text() == str )
        return;

    d->editor.setHtml( str );
}
QString RichTextNote::text()
{
    return d->editor.document()->toHtml( "utf-8" );
}
void RichTextNote::removeDir()
{
    if ( !removePath( fileName() ) )
        QMessageBox::warning( this, tr( "Warning" ), tr( "I can not delete" ) );
}
void RichTextNote::remove()
{
    invokeRemove();
    removeDir();
    close();
    deleteLater();
}
void RichTextNote::invokeRemove()
{
    emit changed( EventsNote::Remove );
}

void RichTextNote::setTop( bool b )
{
    if ( isTop() == b )
        return;

    AbstractNote::setTop( b );
    updateStates();
}

void RichTextNote::selectTitle()
{
    const QString & text = QInputDialog::getText( this, tr( "Select title" ), tr( "Title: " ), QLineEdit::Normal, title() );
    if ( text.isEmpty() )
        return;

    setTitle( text );
}
void RichTextNote::selectTitleFont()
{
    bool ok;
    const QFont & font = QFontDialog::getFont( &ok, titleFont(), this, tr( "Select font" ) );
    if ( !ok )
        return;

    setTitleFont( font );
}

void RichTextNote::selectTitleColor()
{
    const QColor & color = QColorDialog::getColor( titleColor(), this, tr( "Select color" ) );
    if ( !color.isValid() )
        return;

    setTitleColor( color );
}
void RichTextNote::selectColor()
{
    const QColor & color = QColorDialog::getColor( bodyColor(), this, tr( "Select color" ) );
    if ( !color.isValid() )
        return;

    setBodyColor( color );
}

void RichTextNote::selectOpacity()
{    
    int current = windowOpacity() * 100.0;
    int min = nm_Note::minimalOpacity * 100.0;
    int max = nm_Note::maximalOpacity * 100.0;
    bool b;
    int op = QInputDialog::getInt( this, tr( "Select opacity" ), tr( "Opacity:" ), current, min, max, 1, &b );
    if ( !b )
        return;

    setOpacity( (qreal)op / 100.0 );
}

void RichTextNote::saveAs()
{
    QStringList imageFormats;
    foreach ( const QByteArray & format, QImageWriter::supportedImageFormats() )
        imageFormats << QString( format );

    QStringList textFormats;
    foreach ( const QByteArray & format, QTextDocumentWriter::supportedDocumentFormats() )
        textFormats << QString( format );

    QString filters;
    filters.append( tr( "File notes" ) + QString( " ( *.%1 )\n" ).arg( getNoteFormat() ) );
    filters.append( "TXT ( *.txt )\n" );
    filters.append( "PDF ( *.pdf )\n" );
    filters.append( "PSF ( *.psf )\n" );

    foreach ( const QString & format, textFormats )
        filters.append( QString( "%1 ( *.%2 )\n" ).arg( format.toUpper() ).arg( format ) );

    foreach ( const QString & format, imageFormats )
        filters.append( QString( "%1 ( *.%2 )\n" ).arg( format.toUpper() ).arg( format ) );


    QString title = this->title();
    // Замена неугодных винде символов в именах файлов, знаком нижнего подчеркивания
    title = title.replace( QRegExp( "[:/|*?\"<>]" ), "_" );

    const QString & saveFileName = QFileDialog::getSaveFileName( this, QString(), title, filters );
    if ( saveFileName.isEmpty() )
        return;

    QString suffix = QFileInfo( saveFileName ).suffix();

    // для создания pdf файлов используется другой класс
    if ( suffix.contains( "pdf", Qt::CaseInsensitive ) || suffix.contains( "psf", Qt::CaseInsensitive ) )
    {
        QPrinter printer;
        printer.setOutputFileName( saveFileName );
        textEditor()->print( &printer );

    } else if ( imageFormats.contains( suffix, Qt::CaseInsensitive ) )
    {
        QPixmap::grabWidget( this ).save( saveFileName );

    } else if ( suffix.contains( "txt", Qt::CaseInsensitive ) )
    {
        QTextDocumentWriter textDocumentWriter;
        textDocumentWriter.setFormat( suffix.toAscii() );
        textDocumentWriter.setCodec( QTextCodec::codecForName( "utf8" ) );
        textDocumentWriter.setFileName( saveFileName );
        textDocumentWriter.write( document() );

    } else if ( suffix.contains( "filenotes", Qt::CaseInsensitive ) )
    {
        if ( !JlCompress::compressDir( saveFileName, fileName() ) )
        {
            QMessageBox::information( this, tr( "Information" ), tr( "An error occurred saving notes" ) );
            WARNING( "An error occurred saving notes" );
            return;
        }
    }
}
void RichTextNote::open()
{   
    const QString & fileName = QFileDialog::getOpenFileName( this );
    if ( fileName.isEmpty() )
        return;

    QFile file( fileName );
    if ( !file.open( QIODevice::ReadOnly ) )
        return;

    QTextStream out( &file );
    out.setCodec( "utf8" );

    setText( out.readAll() );
}
void RichTextNote::print()
{
    QPrinter printer( QPrinter::HighResolution );
    printer.setDocName( tr( "document" ) + QString( " \"%1\"" ).arg( title() ) );

    QPrintDialog dialog( &printer, this );
    if ( dialog.exec() == QDialog::Accepted )
        print( &printer );
}
void RichTextNote::previewPrint()
{
    QPrinter printer( QPrinter::HighResolution );
    printer.setDocName( tr( "document" ) + QString( " \"%1\"" ).arg( title() ) );

    QPrintPreviewDialog preview( &printer, this, Qt::Dialog | Qt::WindowMaximizeButtonHint );

    QObject::connect( &preview, SIGNAL( paintRequested( QPrinter * ) ), SLOT( print( QPrinter * ) ) );
    preview.exec();
}
void RichTextNote::selectAttach()
{
    QString dir = QDesktopServices::storageLocation( QDesktopServices::HomeLocation );
    QStringList files = QFileDialog::getOpenFileNames( this, tr( "Select the files" ), dir );
    if ( files.isEmpty() )
        return;

    foreach ( const QString & fileName, files )
        attach( fileName );
}

void RichTextNote::insertImage( const QString & fileName )
{
    QString newFileName = attach( fileName );
    QString relativePath = QFileInfo( attachDirPath() ).fileName() + "/" + QFileInfo( newFileName ).fileName();
    relativePath = QDir::toNativeSeparators( relativePath );

    document()->addResource( QTextDocument::ImageResource, QUrl( relativePath ), QImage( newFileName ) );
    d->editor.textCursor().insertImage( relativePath );
}
void RichTextNote::insertImage( const QPixmap & pixmap )
{
    // Сохраняем в папке прикрепленных файлов
    const QString & name = tr( "image" ) + QString( "_%1.png" ).arg( QDateTime::currentDateTime().toString( "hh-mm-ss_yyyy-MM-dd" ) );
    QString path = attachDirPath() + "/" + name;
    path = QDir::toNativeSeparators( path );

    if ( !pixmap.save( path ) )
    {
        qDebug() << "Произошла ошибка при сохранении";
        return;
    }

    insertImage( path );
    updateAttachList();
}

QString RichTextNote::attach( const QString & fileName )
{
    QString newFileName = attachDirPath() + QDir::separator() + QFileInfo( fileName ).fileName();
    QFile::copy( fileName, newFileName );

    attachModel.appendRow( new QStandardItem( QFileInfo( fileName ).fileName() ) );
    emit changed( EventsNote::ChangeAttach );
    return newFileName;
}
void RichTextNote::updateAttachList()
{
    attachModel.clear();
    foreach ( const QFileInfo & fileInfo, QDir( attachDirPath() ).entryInfoList( QDir::Files ) )
        attachModel.appendRow( new QStandardItem( fileInfo.fileName() ) );
}
int RichTextNote::numberOfAttachments()
{
    return QDir( attachDirPath() ).entryList( QDir::Files ).size();
}

void RichTextNote::setActivateTimerAutosave( bool activate )
{
    if ( activate == d->timerAutosave.isActive() )
        return;

    if ( activate )
        d->timerAutosave.start();
    else
        d->timerAutosave.stop();
}
bool RichTextNote::isActivateTimerAutosave()
{
    return d->timerAutosave.isActive();
}

void RichTextNote::setIntervalAutosave( quint64 minutes )
{
    d->timerAutosave.setInterval( 1000 * 60 * minutes );
}
quint64 RichTextNote::intervalAutosave()
{
    return d->timerAutosave.interval() / ( 1000 * 60 );
}

void RichTextNote::changeOpacity( QAction * action )
{
    qreal o = action->data().toReal();
    if ( o < nm_Note::minimalOpacity )
        selectOpacity();
    else
        setOpacity( o );
}
void RichTextNote::print( QPrinter * printer )
{
    d->editor.print( printer );
}
void RichTextNote::updateStates()
{
    bool top = isTop();
    tButtonSetTopBottom->setChecked( top );
    actionSetTopBottom->setChecked( top );
    tButtonSave->setEnabled( true );
    actionSave->setEnabled( true );
    actionVisibleToolBar->setChecked( isVisibleToolBar() );
    actionVisibleQuickFind->setChecked( quickFind->isVisible() );
}
void RichTextNote::contentsChanged()
{
    mapSettings[ "Modified" ] = QDateTime::currentDateTime();

    updateStates();
    emit changed( EventsNote::ChangeText );
}
void RichTextNote::doubleClickingOnTitle()
{
    int data = defaultMapSettings[ "ActionDoubleClickOnTitle" ].toInt();
    switch ( data )
    {
    case Shared::DoNothing:
        break;

    case Shared::EditTitleNote:
        selectTitle();
        break;

    case Shared::HideNote:
        hide();
        break;

    case Shared::DeleteNote:
        invokeRemove();
        break;

    case Shared::SaveNote:
        save();
        break;

    case Shared::SaveNoteAs:
        saveAs();
        break;

    case Shared::PrintNotes:
        print();
        break;
    }
}

void RichTextNote::enterEvent( QEvent * )
{
    QPropertyAnimation * animation = new QPropertyAnimation( this, "windowOpacity" );
    animation->setDuration( 200 );
    animation->setStartValue( opacity() );
    animation->setEndValue( nm_Note::maximalOpacity );
    animation->start( QAbstractAnimation::DeleteWhenStopped );
}
void RichTextNote::leaveEvent( QEvent * )
{
    QPropertyAnimation * animation = new QPropertyAnimation( this, "windowOpacity" );
    animation->setDuration( 700 );
    animation->setStartValue( nm_Note::maximalOpacity );
    animation->setEndValue( opacity() );
    animation->start( QAbstractAnimation::DeleteWhenStopped );
}
