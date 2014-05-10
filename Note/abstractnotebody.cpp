#include "abstractnotebody.h"

#include <QPainter>
#include <QDebug>


AbstractNoteBody::AbstractNoteBody( QWidget * parent )
    : QMainWindow( parent ),
      d( new d_AbstractBody )
{
    setContextMenuPolicy( Qt::NoContextMenu );

    tBar = new QToolBar();
    tBar->setMovable( false );
    tBar->setFloatable( false );

    addToolBar( tBar );

    QObject::connect( tBar, SIGNAL( visibilityChanged(bool) ), SIGNAL( changeVisibleToolbar(bool) ) );
}

AbstractNoteBody::~AbstractNoteBody()
{
    delete d;
}

void AbstractNoteBody::setColor( const QColor & color )
{
    d->color = color;
}
QColor AbstractNoteBody::color()
{
    return d->color;
}

void AbstractNoteBody::setVisibleToolBar( bool visible )
{
    tBar->setVisible( visible );
}
bool AbstractNoteBody::isVisibleToolBar()
{
    return tBar->isVisible();
}

void AbstractNoteBody::setToolBarIconSize( const QSize & size )
{
    tBar->setIconSize( size );
}
QSize AbstractNoteBody::toolBarIconSize()
{
    return tBar->iconSize();
}

void AbstractNoteBody::setWidget( QWidget * widget )
{
    setCentralWidget( widget );
}
QWidget * AbstractNoteBody::widget()
{
    return centralWidget();
}

void AbstractNoteBody::addWidgetToToolBar( QWidget * widget )
{   
    tBar->addWidget( widget );
}

void AbstractNoteBody::addActionToToolBar( QAction * action )
{
    tBar->addAction( action );
}

void AbstractNoteBody::addSeparator()
{
    tBar->addSeparator();
}

QToolBar * AbstractNoteBody::toolBar()
{
    return tBar;
}

void AbstractNoteBody::paintEvent( QPaintEvent * )
{
    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing );
    painter.setBrush( d->color );
    painter.setPen( d->color );
    painter.drawRect( rect() );
}
