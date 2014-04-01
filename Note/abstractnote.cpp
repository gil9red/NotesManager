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
    setStatusBar( new QStatusBar() );

    d->d_head = head->d;
    d->d_body = body->d;
    d->sides = nm_Note::Frame::visible;
    d->penSides = Shared::Left | Shared::Right | Shared::Top | Shared::Bottom;
    d->colorSides = nm_Note::Frame::color;
    d->widthPenSides = nm_Note::Frame::width;

    propertyAttachable = new PropertyAttachable( this );
    propertyAttachable->installTo( head );

    QObject::connect( head, SIGNAL( doubleClick() ), SIGNAL( doubleClickHead() ) );
    QObject::connect( body, SIGNAL( changeVisibleToolbar(bool) ), SIGNAL( changeVisibleToolbar(bool) ) );

    QVBoxLayout * mainLayout = new QVBoxLayout();
    mainLayout->setSpacing( 0 );
    mainLayout->setMargin( 2 );

    mainLayout->addWidget( head );
    mainLayout->addWidget( body );

    QWidget * mainWidget = new QWidget();
    mainWidget->setLayout( mainLayout );
    setCentralWidget( mainWidget );

    setMinimumSize( nm_Note::minimalWidth, nm_Note::minimalHeight );
    setWindowFlags( nm_Note::flags | Qt::WindowStaysOnTopHint );

    d->sides = nm_Note::Frame::visible;
    d->widthPenSides = nm_Note::Frame::width;
    d->colorSides = nm_Note::Frame::color;
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
    if ( head->title() == str )
        return;

    head->setTitle( str );
    emit changed( EventsNote::ChangeTitle );
}
QString AbstractNote::title()
{
    return head->title();
}

void AbstractNote::setTitleFont( const QFont & f )
{
    if ( head->font() == f )
        return;

    head->setFont( f );
    emit changed( EventsNote::ChangeFontTitle );
}
QFont AbstractNote::titleFont()
{
    return head->font();
}

void AbstractNote::setTop( bool b )
{
    Qt::WindowFlags f = nm_Note::flags;
    bool visible = isVisible();

    f |= b ? Qt::WindowStaysOnTopHint : Qt::WindowStaysOnBottomHint;
    setWindowFlags( f );

    if ( visible )
        show();

    emit changed( EventsNote::ChangeTop );
}
bool AbstractNote::isTop()
{
    Qt::WindowFlags f = windowFlags();
    bool top = f.testFlag( Qt::WindowStaysOnTopHint );
    bool bottom = f.testFlag( Qt::WindowStaysOnBottomHint );

    // По дефолту, окно должно быть поверх всех окон
    if ( !top && !bottom )
    {
        setWindowFlags( Qt::WindowStaysOnTopHint );
        return true;
    }

    if ( top )
        return true;

    if ( bottom )
        return false;

    return true;
}

void AbstractNote::setBodyColor( const QColor & c )
{
    emit changed( EventsNote::ChangeColorNote );

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
    if ( opacity() == o )
        return;
    if ( o < nm_Note::minimalOpacity )
        o = nm_Note::minimalOpacity;
    if ( o > nm_Note::maximalOpacity )
        o = nm_Note::maximalOpacity;

    mapSettings[ "Opacity" ] = o;
    setWindowOpacity( o );
}
qreal AbstractNote::opacity()
{
    return mapSettings[ "Opacity" ].toFloat();
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
void AbstractNote::paintEvent( QPaintEvent * event )
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
