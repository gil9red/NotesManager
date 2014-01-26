#include "abstractnote.h"

#include <QPainter>
#include <QSettings>
#include <QTextCodec>
#include <QTextEdit>
#include <QToolBar>
#include <QToolButton>
#include <QPushButton>
#include <QApplication>
#include <QVBoxLayout>
#include <QDebug>
#include <QAction>
#include <QDateTime>
#include <QMessageBox>


AbstractNote::AbstractNote( QWidget * parent ) :
    QMainWindow( parent ),
    d( new d_AbstractNote() ),
    head( new AbstractNoteHead() ),
    body( new AbstractNoteBody() )
{
    setContextMenuPolicy( Qt::DefaultContextMenu );

    d->d_head = head->d;
    d->d_body = body->d;

    setStatusBar( new QStatusBar() );

    // TODO: значения по умолчанию
    d->sides = Abstract::Frame::visible;
    d->penSides = Shared::Left | Shared::Right | Shared::Top | Shared::Bottom;
    d->colorSides = Abstract::Frame::color;
    d->widthPenSides = Abstract::Frame::width;

    propertyAttachable = new PropertyAttachable( this );
    propertyAttachable->installTo( head );

    connect( head, SIGNAL( doubleClicked() ), SLOT( doubleHeadClicked() ) );
    connect( body, SIGNAL( changeVisibleToolbar(bool) ), SIGNAL( changeVisibleToolbar(bool) ) );

    QVBoxLayout * mainLayout = new QVBoxLayout();
    mainLayout->setSpacing( 0 );
    mainLayout->setMargin( 2 );

    mainLayout->addWidget( head );
    mainLayout->addWidget( body );

    QWidget * mainWidget = new QWidget();
    mainWidget->setLayout( mainLayout );

    setCentralWidget( mainWidget );

    // TODO: значения по умолчанию
    // Минимальная высота. Выбрал ее как наоболее подходящую
    setMinimumHeight( Abstract::minimalHeight );
    setWindowFlags( Abstract::flags );

    connect( &timerAutosave, SIGNAL( timeout() ), SLOT( save() ) );
//  TODO:  setActivateTimerAutosave( true );
//    setIntervalAutosave( 7 ); // интервал автосохранения в минутах

    // TODO: значения по умолчанию
    setVisibleFrame( Abstract::Frame::visible );
    setWidthPenSides( Abstract::Frame::width );
    setColorSides( Abstract::Frame::color );
}
AbstractNote::~AbstractNote()
{
    delete d;
}

void AbstractNote::removeFile()
{
    if ( !QFile::remove( path() ) )
    {
        QMessageBox::warning( this, tr( "Warning" ), tr( "I can not delete" ) );
        return;
    }
}


void AbstractNote::setPath( const QString & fileName )
{
    d->path = fileName;
}
QString AbstractNote::path()
{
    return d->path;
}

void AbstractNote::setWidget( QWidget * w )
{
    body->setWidget( w );
}
QWidget * AbstractNote::widget()
{
    return body->widget();
}

void AbstractNote::addWidgetToToolBar(QWidget * w )
{
    body->addWidgetToToolBar( w );
}
void AbstractNote::addSeparatorToToolBar()
{
    body->addSeparator();
}

void AbstractNote::addAction(QAction *action)
{
    contextMenu.addAction( action );
}
void AbstractNote::addActions(const QList<QAction*> & actions)
{
    contextMenu.addActions( actions );
}
void AbstractNote::insertAction(QAction *before, QAction *action)
{
    contextMenu.insertAction( before, action );
}
void AbstractNote::insertActions(QAction *before, QList<QAction *> &actions)
{
    contextMenu.insertActions( before, actions );
}
void AbstractNote::removeAction(QAction *action)
{
    contextMenu.removeAction( action );
}
void AbstractNote::addSeparator()
{
    QAction * separator = new QAction( this );
    separator->setSeparator( true );

    addAction( separator );
}
QMenu * AbstractNote::menu()
{
    return &contextMenu;
}
void AbstractNote::addMenu(QMenu *menu)
{
    contextMenu.addMenu( menu );
}


void AbstractNote::doubleHeadClicked()
{
    setMinimize( !d->isMinimize );
}


void AbstractNote::setDistanceAttachable( int d )
{
    propertyAttachable->setDistance( d );
}
int AbstractNote::distanceAttachable()
{
    return propertyAttachable->distance();
}

void AbstractNote::setActivateAttachable( bool act )
{
    propertyAttachable->setActivate( act );
}
bool AbstractNote::isActivateAttachable()
{
    return propertyAttachable->isActivate();
}

void AbstractNote::setVisibleFrame( bool visible )
{
    d->sides = visible;
    update();
}
bool AbstractNote::visibleFrame()
{
    return d->sides;
}

void AbstractNote::setWidthPenSides( float width )
{
    d->widthPenSides = width;
    update();
}
float AbstractNote::widthPenSides()
{
    return d->widthPenSides;
}

void AbstractNote::setColorSides( const QColor & color )
{
    d->colorSides = color;
    update();
}
QColor AbstractNote::colorSides()
{
    return d->colorSides;
}

void AbstractNote::setVisibleToolBar( bool visible )
{
    body->setVisibleToolBar( visible );
}
bool AbstractNote::isVisibleToolBar()
{
    return body->isVisibleToolBar();
}

void AbstractNote::save()
{    
    QSettings ini( path(), QSettings::IniFormat );
    ini.setIniCodec( "utf8" );
    ini.setValue( "Size", size() );
    ini.setValue( "OldHeight", d->oldHeight );
    ini.setValue( "Position", pos() );
    ini.setValue( "Top", d->isTop );
    ini.setValue( "Minimize", d->isMinimize );
    ini.setValue( "ColorBody", body->color().name() );
    ini.setValue( "ColorTitle", head->color().name() );
    ini.setValue( "Title", head->title() );
    ini.setValue( "FontTitle", head->font().toString() );
    ini.setValue( "Opacity", d->opacity * 100 );
    ini.setValue( "Visible", isVisible() );

    statusBar()->showMessage( tr( "Save completed" ), 5000 );
    emit changed( EventsNote::SaveEnded );
}
void AbstractNote::load()
{
    QSettings ini( path(), QSettings::IniFormat );
    ini.setIniCodec( "utf8" );
// TODO: значения по умолчанию брать
    resize( ini.value( "Size", QSize( 150, 150 ) ).toSize() );
    d->oldHeight = ini.value( "OldHeight", height() ).toUInt();

    // если не равно, значит тут имело место минимизация,
    // а oldHeight хранит значение до минимизации
    // при минимизации height будет помнить размер при минимизации,
    // что не нужно
    if ( d->oldHeight != height() )
        resize( width(), d->oldHeight );

    move( ini.value( "Position", QPoint( 100, 100 ) ).toPoint() );

    setTop( ini.value( "Top", true ).toBool() );
    setMinimize( ini.value( "Minimize", false ).toBool() );
    setBackgroundColor( QColor( ini.value( "ColorBody", QColor( Qt::gray ).name() ).toString() ) );
    setTitle( ini.value( "Title", tr( "New note" ) + " " + QDateTime::currentDateTime().toString( Qt::SystemLocaleLongDate ) ).toString() );
    setTitleColor( QColor( ini.value( "ColorTitle", QColor( Qt::darkGray ).name() ).toString() ) );
    setOpacity( ini.value( "Opacity", 100 ).toFloat() / 100.0 );
    setVisible( ini.value( "Visible", false ).toBool() );

    QFont titleFont;
    titleFont.fromString( ini.value( "FontTitle" ).toString() );
    setTitleFont( titleFont );

    emit changed( EventsNote::LoadEnded );
}

void AbstractNote::setTitle( const QString & str )
{
    head->setTitle( str );
    emit changed( EventsNote::ChangeTitle );
}
QString AbstractNote::title()
{
    return head->title();
}

void AbstractNote::setTitleFont( const QFont & f )
{
    head->setFont( f );
    emit changed( EventsNote::ChangeFontTitle );
}
QFont AbstractNote::titleFont()
{
    return head->font();
}

void AbstractNote::setTop( bool b )
{
    d->isTop = b;
    Qt::WindowFlags flags = Abstract::flags;
    bool visible = isVisible();

    flags |= d->isTop ? Qt::WindowStaysOnTopHint : Qt::WindowStaysOnBottomHint;
    setWindowFlags( flags );

    if ( visible )
        show();

    emit changed( EventsNote::ChangeTop );
}
bool AbstractNote::isTop()
{
    return d->isTop;
}

void AbstractNote::setMinimize( bool b )
{
    emit changed( EventsNote::ChangeMinimize );

    // если у нас уже установлен режим, то ничего менять не будем
    if ( d->isMinimize == b )
        return;

    d->isMinimize = b;
    body->centralWidget()->setVisible( !b );

    if ( d->isMinimize )
    {        
        d->oldHeight = height(); // запоминаем высоту
        setMaximumHeight( minimumHeight() ); // фиксируем высоту
    } else
    {        
        setMaximumHeight( QWIDGETSIZE_MAX ); // убираем фиксированную высоту
        resize( width(), d->oldHeight ); // восстанавливает высоту
    }
}
bool AbstractNote::isMinimize()
{
    return d->isMinimize;
}

void AbstractNote::setBackgroundColor( const QColor & c )
{
    emit changed( EventsNote::ChangeColorNote );

    d->color = c;
    body->setColor( d->color );
    body->update();
    update();
}
QColor AbstractNote::backgroundColor()
{
    return body->color();
}

void AbstractNote::setTitleColor( const QColor & color )
{
    emit changed( EventsNote::ChangeColorTitle );

    head->setColor( color );
    head->update();
}
QColor AbstractNote::titleColor()
{
    return head->color();
}

void AbstractNote::setOpacity( qreal o )
{
    if( o < 0.2 )
        o = 0.2;
    setWindowOpacity( o );
    d->opacity = o;
}
qreal AbstractNote::opacity()
{
    return d->opacity;
}

void AbstractNote::setActivateTimerAutosave( bool activate )
{
    d->activateTimerAutosave = activate;

    if ( activate )
        timerAutosave.start();
    else
        timerAutosave.stop();
}
bool AbstractNote::isActivateTimerAutosave()
{
    return d->activateTimerAutosave;
}

void AbstractNote::setIntervalAutosave( quint64 minutes )
{
    timerAutosave.setInterval( 1000 * 60 * minutes );
    d->intervalAutosave = minutes;
}
quint64 AbstractNote::intervalAutosave()
{
    return d->intervalAutosave;
}


void AbstractNote::closeEvent( QCloseEvent * )
{
    hide();
    emit changed( EventsNote::Close );
}
void AbstractNote::resizeEvent( QResizeEvent * event )
{
    if ( !d->isMinimize )
        d->oldHeight = event->size().height(); // запоминаем высоту
}
void AbstractNote::contextMenuEvent( QContextMenuEvent * event )
{
    contextMenu.exec( event->globalPos() );
}
void AbstractNote::showEvent( QShowEvent * event )
{
    QWidget::showEvent( event );

    emit changed( EventsNote::IShow );
    emit changed( EventsNote::ChangeVisibility );
}
void AbstractNote::hideEvent( QHideEvent * event )
{
    QWidget::hideEvent( event );

    emit changed( EventsNote::IHidden );
    emit changed( EventsNote::ChangeVisibility );
}
void AbstractNote::paintEvent( QPaintEvent * )
{
    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing );
    painter.setBrush( d->color );
    painter.setPen( d->color );
    painter.drawRect( rect() );

    if ( !d->sides )
        return;

    int indent = d->widthPenSides;

    // Нарисуем обводку
    painter.setPen( QPen( d->colorSides, indent ) );

    if ( d->penSides.testFlag( Shared::Top ) )
        painter.drawLine( QPoint( indent, indent ), QPoint( width() - indent, indent ) );

    if ( d->penSides.testFlag( Shared::Left ) )
        painter.drawLine( QPoint( indent, indent ), QPoint( indent, height() - indent ) );

    if ( d->penSides.testFlag( Shared::Right ) )
        painter.drawLine( QPoint( width() - indent, indent ), QPoint( width() - indent, height() - indent ) );

    if ( d->penSides.testFlag( Shared::Bottom ) )
        painter.drawLine( QPoint( indent, height() - indent ), QPoint( width() - indent, height() - indent ) );
}
