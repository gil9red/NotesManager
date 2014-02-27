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

#include "abstractnotehead.h"

#include <QVBoxLayout>
#include <QPainter>

AbstractNoteHead::AbstractNoteHead( QWidget * parent )
    : QWidget( parent ),
      d( new d_AbstractHead ),
      label( new QLabel )
{
    d->isMousePress = false;

    setFixedHeight( Note::topBorder );

    QVBoxLayout * mainLayout = new QVBoxLayout();
    mainLayout->setSpacing( 0 );
    mainLayout->setContentsMargins( 5, 5, 5, 5 );
    setLayout( mainLayout );

    mainLayout->addWidget( label, 0, Qt::AlignCenter );
}
AbstractNoteHead::~AbstractNoteHead()
{
    delete d;
}

void AbstractNoteHead::setTitle( const QString & str )
{
    label->setText( str );
}
QString AbstractNoteHead::title()
{
    return label->text();
}

void AbstractNoteHead::setFont( const QFont & f )
{
    label->setFont( f );
}
QFont AbstractNoteHead::font()
{
    return label->font();
}

void AbstractNoteHead::setColor( const QColor & color )
{
    d->color = color;
}
QColor AbstractNoteHead::color()
{
    return d->color;
}

void AbstractNoteHead::paintEvent( QPaintEvent * )
{
    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing );

    painter.setBrush( d->color );
    painter.setPen( d->color );
    painter.drawRect( rect() );
}
void AbstractNoteHead::mouseDoubleClickEvent(QMouseEvent *)
{
    emit doubleClick();
}
void AbstractNoteHead::mousePressEvent( QMouseEvent * event )
{
    if ( event->button() == Qt::LeftButton )
    {
        d->oldPosition = event->pos();
        d->isMousePress = true;
    }
}
void AbstractNoteHead::mouseReleaseEvent( QMouseEvent * )
{
    d->isMousePress = false;
}
void AbstractNoteHead::mouseMoveEvent( QMouseEvent * event )
{
    if ( d->isMousePress )
    {
        QPoint delta = event->pos() - d->oldPosition;       
        if ( !parentWidget()->parentWidget() )
            move( pos() + delta );
        else
            parentWidget()->parentWidget()->move( mapToGlobal( pos() ) + delta );
    }
}
