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
#include "settingsnotedialog.h"

QActionGroup * createGroupActionsOpacity( QObject * parent = 0 )
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
    : AbstractNote( parent )
{    
    setFileName( fileName );
    init(); 
}
RichTextNote::RichTextNote( QWidget * parent )
    : AbstractNote( parent )
{
    init();
}

QTextDocument * RichTextNote::document()
{
    return editor.document();
}
TextEditor * RichTextNote::textEditor()
{
    return &editor;
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
    return noteFileName;
}
QString RichTextNote::attachDirPath()
{
    return QDir::fromNativeSeparators( noteFileName + "/attach" );
}
QString RichTextNote::contentFilePath()
{
    return QDir::fromNativeSeparators( noteFileName + "/content.html" );
}
QString RichTextNote::settingsFilePath()
{
    return QDir::fromNativeSeparators( noteFileName + "/settings.ini" );
}

void RichTextNote::setFileName( const QString & dirName )
{    
    noteFileName = dirName;

    // Если уже существует, то выходим
    if ( QDir( noteFileName ).exists() )
        return;

    // Создадим папку заметки с нужными папками и файлами
    QDir().mkdir( noteFileName );
    QDir().mkdir( attachDirPath() );
    QFile( contentFilePath() ).open( QIODevice::WriteOnly );
    QFile( settingsFilePath() ).open( QIODevice::WriteOnly );
}

void RichTextNote::setDefaultSettingsFromMap(const QVariantMap & data )
{
    defaultMapSettings[ "Top" ]                      = data.value( "NewNote_Top" );
    defaultMapSettings[ "ColorTitle" ]               = data.value( "NewNote_ColorTitle" );
    defaultMapSettings[ "ColorBody" ]                = data.value( "NewNote_ColorBody" );
    defaultMapSettings[ "Opacity" ]                  = data.value( "NewNote_Opacity" );
    defaultMapSettings[ "Visible" ]                  = data.value( "NewNote_Visible" );
    defaultMapSettings[ "Title" ]                    = data.value( "NewNote_Title" );
    defaultMapSettings[ "Text" ]                     = data.value( "NewNote_Text" );
    defaultMapSettings[ "FontTitle" ]                = data.value( "NewNote_FontTitle" );
    defaultMapSettings[ "Position" ]                 = data.value( "NewNote_Position" );
    defaultMapSettings[ "Size" ]                     = data.value( "NewNote_Size" );
    defaultMapSettings[ "RandomColor" ]              = data.value( "NewNote_RandomColor" );
    defaultMapSettings[ "RandomPositionOnScreen" ]   = data.value( "NewNote_RandomPositionOnScreen" );
    defaultMapSettings[ "Autosave" ]                 = data.value( "Notes_Autosave" );
    defaultMapSettings[ "AutosaveInterval" ]         = data.value( "Notes_AutosaveInterval" );
    defaultMapSettings[ "ActionDoubleClickOnTitle" ] = data.value( "Notes_ActionDoubleClickOnTitle" );
}

void RichTextNote::init()
{
    setupGUI();

    updateStates();

    setVisibleToolBar( false );
    quickFind->setVisible( false );

    QObject::connect( &timerAutosave, SIGNAL( timeout() ), SLOT( save() ) );
    QObject::connect( this, SIGNAL( doubleClickHead() ), SLOT( doubleClickingOnTitle() ) );
    QObject::connect( editor.document(), SIGNAL( contentsChanged() ), SLOT( contentsChanged() ) );
}
void RichTextNote::setupGUI()
{
    actionVisibleToolBar                     = Create::Action::get( tr( "Toolbar" ), QIcon( "" ), true );
    actionVisibleQuickFind                   = Create::Action::get( tr( "Quick find" ), QIcon( "" ), true );
    QAction * actionVisibleAttachPanel       = Create::Action::get( tr( "Attach panel" ), QIcon( ":/Note/attach-panel" ), true );
    QAction * actionVisibleFindAndReplace    = Create::Action::get( tr( "Find/replace" ), QIcon( ":/Note/find-replace" ), true );
    QAction * actionVisibleFormattingToolbar = Create::Action::get( tr( "Formatting toolbar" ), QIcon( ":/FormattingToolbar/formating" ), true );
    QAction * actionDelete                   = Create::Action::triggered( tr( "Delete" ), QIcon( ":/Note/remove" ), QKeySequence(), this, SLOT( invokeRemove() ) );
    QAction * actionSetTitle                 = Create::Action::triggered( tr( "Set title" ), QIcon( ":/Note/title" ), QKeySequence(), this, SLOT( selectTitle() ) );
    QAction * actionSetTitleFont             = Create::Action::triggered( tr( "Set title font" ), QIcon( ":/Note/title-font" ), QKeySequence(), this, SLOT( selectTitleFont() ) );
    QAction * actionSetTitleColor            = Create::Action::triggered( tr( "Set title color" ), QIcon( ":/Note/title-color" ), QKeySequence(), this, SLOT( selectTitleColor() ) );
    QAction * actionSetColor                 = Create::Action::triggered( tr( "Set window color" ), QIcon::fromTheme( "", QIcon( ":/Note/color" ) ), QKeySequence(), this, SLOT( selectColor() ) );
    QAction * actionHide                     = Create::Action::triggered( tr( "Hide" ), QIcon::fromTheme( "", QIcon( ":/Note/hide" ) ), QKeySequence(), this, SLOT( hide() ) );
    actionSetTopBottom                       = Create::Action::bTriggered( tr( "On top of all windows" ), QIcon::fromTheme( "", QIcon( ":/Note/tacks" ) ), QKeySequence(), this, SLOT( setTop( bool ) ) );
    QAction * actionOpen                     = Create::Action::triggered( tr( "Open" ), QIcon::fromTheme( "", QIcon( ":/Note/open" ) ), QKeySequence(), this, SLOT( open() ) );
    QAction * actionSaveAs                   = Create::Action::triggered( tr( "Save as" ), QIcon::fromTheme( "", QIcon( ":/Note/save-as" ) ), QKeySequence(), this, SLOT( saveAs() ) );
#ifndef QT_NO_PRINTER
    QAction * actionPrint                    = Create::Action::triggered( tr( "Print" ), QIcon::fromTheme( "", QIcon( ":/Note/print" ) ), QKeySequence(), this, SLOT( print() ) );
    QAction * actionPreviewPrint             = Create::Action::triggered( tr( "Preview print" ), QIcon::fromTheme( "", QIcon( ":/Note/preview-print" ) ), QKeySequence(), this, SLOT( previewPrint() ) );
#endif
    QAction * actionSave                     = Create::Action::triggered( tr( "Save" ), QIcon::fromTheme( "", QIcon( ":/Note/save" ) ), QKeySequence(), this, SLOT( save() ) );
    QAction * actionAttach                   = Create::Action::triggered( tr( "Attach" ), QIcon::fromTheme( "", QIcon( ":/Note/attach" ) ), QKeySequence(), this, SLOT( selectAttach() ) );
    QAction * actionSettings                 = Create::Action::triggered( tr( "Settings" ), QIcon::fromTheme( "", QIcon( ":/Manager/settings" ) ), QKeySequence(), this, SLOT(settings()) );


    QObject::connect( actionVisibleToolBar, SIGNAL( triggered(bool) ), SLOT( setVisibleToolBar(bool) ) );
    QObject::connect( this, SIGNAL( changeVisibleToolbar(bool) ), actionVisibleToolBar, SLOT( setChecked(bool) ) );


    // Прозрачность
    QMenu * menuOpacity = new QMenu( tr( "Opacity" ) );
    {
        QActionGroup * group = createGroupActionsOpacity( this );
        QObject::connect( group, SIGNAL( triggered(QAction*) ), SLOT( changeOpacity(QAction*)) );

        menuOpacity->setIcon( QIcon( ":/Note/opacity" ) );
        menuOpacity->addActions( group->actions() );
    }


    addAction( actionVisibleFormattingToolbar );
    addAction( actionVisibleAttachPanel );
    addAction( actionVisibleFindAndReplace );
    addAction( actionVisibleToolBar );
    addAction( actionVisibleQuickFind );
    addSeparator();
    addAction( actionSettings );
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



    Qt::DockWidgetAreas areas = Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea;

    // Панель форматирования
    {
        FormattingToolbar * formattingToolbar = new FormattingToolbar();
        formattingToolbar->installConnect( &editor );
        formattingToolbar->setNote( this );

        dockWidgetFormattingToolbar = new QDockWidget();
        dockWidgetFormattingToolbar->setAllowedAreas( areas );
        dockWidgetFormattingToolbar->setWidget( formattingToolbar );
        dockWidgetFormattingToolbar->setWindowTitle( formattingToolbar->windowTitle() );
        dockWidgetFormattingToolbar->setVisible( false );

        QObject::connect( actionVisibleFormattingToolbar, SIGNAL( triggered(bool) ), dockWidgetFormattingToolbar, SLOT( setVisible(bool) ) );
        QObject::connect( dockWidgetFormattingToolbar, SIGNAL( visibilityChanged(bool) ), actionVisibleFormattingToolbar, SLOT( setChecked(bool) ) );

        body->addDockWidget( Qt::RightDockWidgetArea, dockWidgetFormattingToolbar );

        actionVisibleFormattingToolbar->setChecked( dockWidgetFormattingToolbar->isVisible() );
    }

    // Панель прикрепленных файлов
    {
        attachPanel = new AttachPanel();
        attachPanel->setViewTo( this );

        dockWidgetAttachPanel = new QDockWidget();
        dockWidgetAttachPanel->setAllowedAreas( areas );
        dockWidgetAttachPanel->setWidget( attachPanel );
        dockWidgetAttachPanel->setWindowTitle( attachPanel->windowTitle() );
        dockWidgetAttachPanel->setVisible( false );

        QObject::connect( actionVisibleAttachPanel, SIGNAL( triggered(bool) ), dockWidgetAttachPanel, SLOT( setVisible(bool) ) );
        QObject::connect( dockWidgetAttachPanel, SIGNAL( visibilityChanged(bool) ), actionVisibleAttachPanel, SLOT( setChecked(bool) ) );

        body->addDockWidget( Qt::RightDockWidgetArea, dockWidgetAttachPanel );

        actionVisibleAttachPanel->setChecked( dockWidgetAttachPanel->isVisible() );
    }

    // Поиск и замена
    {
        findAndReplace = new FindAndReplace( &editor );

        dockWidgetFindAndReplace = new QDockWidget();
        dockWidgetFindAndReplace->setAllowedAreas( areas );
        dockWidgetFindAndReplace->setWidget( findAndReplace );
        dockWidgetFindAndReplace->setWindowTitle( findAndReplace->windowTitle() );
        dockWidgetFindAndReplace->setVisible( false );

        QObject::connect( actionVisibleFindAndReplace, SIGNAL( triggered(bool) ), dockWidgetFindAndReplace, SLOT( setVisible(bool) ) );
        QObject::connect( dockWidgetFindAndReplace, SIGNAL( visibilityChanged(bool) ), actionVisibleFindAndReplace, SLOT( setChecked(bool) ) );

        body->addDockWidget( Qt::RightDockWidgetArea, dockWidgetFindAndReplace );

        actionVisibleFindAndReplace->setChecked( dockWidgetFindAndReplace->isVisible() );
    }

    quickFind = new QuickFind( &editor );
    QObject::connect( actionVisibleQuickFind, SIGNAL( triggered(bool) ), quickFind, SLOT( setVisible(bool) ) );
    QObject::connect( quickFind, SIGNAL( visibilityChanged(bool) ), actionVisibleQuickFind, SLOT( setChecked(bool) ) );
    actionVisibleQuickFind->setChecked( quickFind->isVisible() );

    editor.setFrameStyle( QFrame::NoFrame );
    editor.viewport()->setAutoFillBackground( false );
    editor.setStyleSheet( nm_Note::style );

    QVBoxLayout * mainLayout = new QVBoxLayout();
    mainLayout->setSpacing( 0 );
    mainLayout->setContentsMargins( 5, 0, 5, 0 );
    mainLayout->addWidget( &editor );
    mainLayout->addWidget( quickFind );

    body->setWidget( new QWidget() );
    body->widget()->setLayout( mainLayout );
    body->setToolBarIconSize( QSize( 15, 15 ) );


    body->addActionToToolBar( actionHide );
    body->addSeparator();
    body->addActionToToolBar( actionVisibleFormattingToolbar );
    body->addActionToToolBar( actionVisibleAttachPanel );
    body->addActionToToolBar( actionVisibleFindAndReplace );
    body->addSeparator();
    body->addActionToToolBar( actionSettings );
    body->addSeparator();
    body->addActionToToolBar( actionSetTitle );
    body->addActionToToolBar( actionSetTitleFont );
    body->addActionToToolBar( actionSetTitleColor );
    body->addSeparator();
    body->addActionToToolBar( actionSetColor );
    body->addActionToToolBar( menuOpacity->menuAction() );
    body->addSeparator();
    body->addActionToToolBar( actionSetTopBottom );
    body->addSeparator();
    body->addActionToToolBar( actionOpen );
    body->addActionToToolBar( actionSaveAs );
    body->addActionToToolBar( actionSave );
    body->addSeparator();
    body->addActionToToolBar( actionAttach );
    body->addSeparator();
    body->addActionToToolBar( actionPrint );
    body->addActionToToolBar( actionPreviewPrint );
    body->addSeparator();
    body->addActionToToolBar( actionDelete );
}

void RichTextNote::save()
{
    mapSettings[ "Top" ]        = isTop();
    mapSettings[ "ColorTitle" ] = titleColor().name();
    mapSettings[ "ColorBody" ]  = bodyColor().name();
    mapSettings[ "Opacity" ]    = opacity();
    mapSettings[ "Visible" ]    = isVisible();
    mapSettings[ "Title" ]      = title();
    mapSettings[ "FontTitle" ]  = titleFont().toString();
    mapSettings[ "Position" ]   = pos();
    mapSettings[ "Size" ]       = size();

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
            _bodyColor  = QColor( defaultMapSettings[ "ColorBody" ].toString() );
        }

        _top = defaultMapSettings[ "Top" ].toBool();
        _opacity = defaultMapSettings.value( "Opacity" ).toDouble();
        _visible = defaultMapSettings.value( "Visible" ).toBool();

        setText( TextTemplateParser::get( defaultMapSettings[ "Text" ].toString() ) );

    } else
    {
        _fontTitle.fromString( mapSettings[ "FontTitle" ].toString() );
        _title      = mapSettings[ "Title" ].toString();
        _size       = mapSettings[ "Size" ].toSize();
        _position   = mapSettings[ "Position" ].toPoint();
        _titleColor = QColor( mapSettings[ "ColorTitle" ].toString() );
        _bodyColor  = QColor( mapSettings[ "ColorBody" ].toString() );
        _top        = mapSettings[ "Top" ].toBool();
        _opacity    = mapSettings[ "Opacity" ].toDouble();
        _visible    = mapSettings.value( "Visible" ).toBool();

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
   editor.setSource( QUrl::fromLocalFile( contentFilePath() ) );
}
void RichTextNote::setText( const QString & str )
{
    if ( text() == str )
        return;

    editor.setHtml( str );
}
QString RichTextNote::text()
{
    return editor.document()->toHtml( "utf-8" );
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

void RichTextNote::settings()
{
    SettingsNoteDialog settingsDialog( this );
    settingsDialog.setNote( this );
    settingsDialog.setWindowTitle( tr( "Settings" ) + QString( " \"%1\"" ).arg( title() ) );
    settingsDialog.resize( settingsDialog.minimumSizeHint() );
    settingsDialog.exec();
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
        if ( format.contains( "plaintext" ) ) // plaintext тот же txt
            textFormats << "txt";
        else
            textFormats << QString( format.toLower() );

    QString filters;
    filters.append( tr( "File notes" ) + QString( " ( *.%1 )\n" ).arg( getNoteFormat() ) );
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

    // для создания pdf или psf файлов используется другой класс
    if ( suffix.contains( "pdf", Qt::CaseInsensitive ) || suffix.contains( "psf", Qt::CaseInsensitive ) )
    {
        QPrinter printer;
        printer.setOutputFileName( saveFileName );
        textEditor()->print( &printer );

    } else if ( imageFormats.contains( suffix, Qt::CaseInsensitive ) ) // Если хотим сохранить как картинку, сохраним скрин окна заметки
    {        
        QPixmap::grabWidget( this ).save( saveFileName );

    } else if ( suffix.contains( "txt", Qt::CaseInsensitive ) // Если у нас txt, html или odf
                || suffix.contains( "html", Qt::CaseInsensitive )
                || suffix.contains( "odf", Qt::CaseInsensitive ))
    {
        QTextDocumentWriter textDocumentWriter;
        textDocumentWriter.setFormat( suffix.toAscii() );
        textDocumentWriter.setCodec( QTextCodec::codecForName( "utf8" ) );
        textDocumentWriter.setFileName( saveFileName );
        textDocumentWriter.write( document() );

    } else if ( suffix.contains( "filenotes", Qt::CaseInsensitive ) ) // Если свой формат, то сохраняем в архиве
    {
        if ( !JlCompress::compressDir( saveFileName, fileName() ) )
        {
            QMessageBox::information( this, tr( "Information" ), tr( "An error occurred saving notes" ) );
            WARNING( "An error occurred saving notes" );
            return;
        }
    } else
        WARNING( "Unknown format!" );
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
    editor.textCursor().insertImage( relativePath );
}
void RichTextNote::insertImage( const QPixmap & pixmap )
{
    // Сохраняем в папке прикрепленных файлов
    const QString & name = tr( "image" ) + QString( "_%1.png" ).arg( QDateTime::currentDateTime().toString( "hh-mm-ss_yyyy-MM-dd" ) );
    QString path = attachDirPath() + "/" + name;
    path = QDir::toNativeSeparators( path );
    if ( !pixmap.save( path ) )
    {
        WARNING( "Error when saving!" )
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
    if ( activate == timerAutosave.isActive() )
        return;

    if ( activate )
        timerAutosave.start();
    else
        timerAutosave.stop();
}
bool RichTextNote::isActivateTimerAutosave()
{
    return timerAutosave.isActive();
}

void RichTextNote::setIntervalAutosave( quint64 minutes )
{
    timerAutosave.setInterval( 1000 * 60 * minutes );
}
quint64 RichTextNote::intervalAutosave()
{
    return timerAutosave.interval() / ( 1000 * 60 );
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
    editor.print( printer );
}
void RichTextNote::updateStates()
{
    bool top = isTop();
    actionSetTopBottom->setChecked( top );
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
