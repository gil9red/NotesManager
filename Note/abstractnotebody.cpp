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

    connect( tBar, SIGNAL( visibilityChanged(bool) ), SIGNAL( changeVisibleToolbar(bool) ) );
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
