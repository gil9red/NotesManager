/*************************************************************************
**
** Copyright (C) 2013-2014 by Ilya Petrash
** All rights reserved.
** Contact: gil9red@gmail.com, ip1992@inbox.ru
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
    d->sides = Note::Frame::visible;
    d->penSides = Shared::Left | Shared::Right | Shared::Top | Shared::Bottom;
    d->colorSides = Note::Frame::color;
    d->widthPenSides = Note::Frame::width;

    body->setStatusBar( new QStatusBar() );
    //setStatusBar( new QStatusBar() );

    propertyAttachable = new PropertyAttachable( this );
    propertyAttachable->installTo( head );

    connect( head, SIGNAL( doubleClick() ), SIGNAL( doubleClickHead() ) );
    connect( body, SIGNAL( changeVisibleToolbar(bool) ), SIGNAL( changeVisibleToolbar(bool) ) );

    QVBoxLayout * mainLayout = new QVBoxLayout();
    mainLayout->setSpacing( 0 );
    mainLayout->setMargin( 2 );

    mainLayout->addWidget( head );
    mainLayout->addWidget( body );

    QWidget * mainWidget = new QWidget();
    mainWidget->setLayout( mainLayout );
    setCentralWidget( mainWidget );

    setMinimumSize( Note::minimalWidth, Note::minimalHeight );
    setWindowFlags( Note::flags | Qt::WindowStaysOnTopHint );

//    setVisibleFrame( Note::Frame::visible );
//    setWidthPenSides( Note::Frame::width );
//    setColorSides( Note::Frame::color );

    d->sides = Note::Frame::visible;
    d->widthPenSides = Note::Frame::width;
    d->colorSides = Note::Frame::color;
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

//void AbstractNote::setVisibleFrame( bool visible )
//{
//    d->sides = visible;
//    update();
//}
//bool AbstractNote::visibleFrame()
//{
//    return d->sides;
//}

//void AbstractNote::setWidthPenSides( float width )
//{
//    d->widthPenSides = width;
//    update();
//}
//float AbstractNote::widthPenSides()
//{
//    return d->widthPenSides;
//}

//void AbstractNote::setColorSides( const QColor & color )
//{
//    d->colorSides = color;
//    update();
//}
//QColor AbstractNote::colorSides()
//{
//    return d->colorSides;
//}

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
    Qt::WindowFlags f = Note::flags;
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
    if ( o < Note::minimalOpacity )
        o = Note::minimalOpacity;
    if ( o > Note::maximalOpacity )
        o = Note::maximalOpacity;

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
