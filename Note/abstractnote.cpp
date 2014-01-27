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
    d->sides = Note::Frame::visible;
    d->penSides = Shared::Left | Shared::Right | Shared::Top | Shared::Bottom;
    d->colorSides = Note::Frame::color;
    d->widthPenSides = Note::Frame::width;

    propertyAttachable = new PropertyAttachable( this );
    propertyAttachable->installTo( head );

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
    setMinimumHeight( Note::minimalHeight );
    setWindowFlags( Note::flags );

    // TODO: значения по умолчанию
    setVisibleFrame( Note::Frame::visible );
    setWidthPenSides( Note::Frame::width );
    setColorSides( Note::Frame::color );
}
AbstractNote::~AbstractNote()
{
    delete d;
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
    // TODO: AbstractNote::setTop
    qDebug() << "AbstractNote::setTop" << b;
    mapSettings[ "Top" ] = b;

    Qt::WindowFlags flags = Note::flags;
    bool visible = isVisible();

    flags |= b ? Qt::WindowStaysOnTopHint : Qt::WindowStaysOnBottomHint;
    setWindowFlags( flags );

    if ( visible )
        show();

    emit changed( EventsNote::ChangeTop );
}
bool AbstractNote::isTop()
{
    // TODO: AbstractNote::isTop
//    return d->isTop;
    return mapSettings[ "Top" ].toBool();
//    return windowFlags().testFlag( Qt::WindowStaysOnBottomHint );
}

void AbstractNote::setBodyColor( const QColor & c )
{
    emit changed( EventsNote::ChangeColorNote );

    mapSettings[ "ColorBody" ] = c.name();
    body->setColor( c );
    body->update();
    update();
}
QColor AbstractNote::bodyColor()
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
    if( o < Note::minimalOpacity )
        o = Note::minimalOpacity;

    mapSettings[ "Opacity" ] = o;
    setWindowOpacity( o );
}
qreal AbstractNote::opacity()
{
//    return mapSettings[ "Opacity" ].toFloat();
    return windowOpacity();
}

void AbstractNote::closeEvent( QCloseEvent * )
{
    hide();
    emit changed( EventsNote::Close );
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
    const QColor & color = bodyColor();
    painter.setBrush( color );
    painter.setPen( color );
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
