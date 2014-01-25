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


#include "ColorButton.h"

#include <QMenu>
#include <QColorDialog>
#include <QPainter>
#include <QVBoxLayout>
#include <QDebug>


ColorButton::ColorButton( QWidget * parent )
    : QToolButton( parent ),
      d( new d_ColorButton() )
{
    createGUI();
    setColor( Qt::black );
    setDefaultColor( Qt::black );
    connect( this, SIGNAL( clicked() ), SLOT( defaultColor() ) );
}
ColorButton::~ColorButton()
{
    delete d;
}
QColor ColorButton::color()
{
    return d->color;
}


void ColorButton::createGUI()
{
    setIcon( QIcon( ":/color-text" ) );
    setPopupMode( QToolButton::MenuButtonPopup );

    d->tButtonDefaultColor = new QToolButton();
    d->tButtonDefaultColor->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
    d->tButtonDefaultColor->setText( tr( "Default color" ) );
    connect( d->tButtonDefaultColor, SIGNAL( clicked() ), SLOT( defaultColor() ) );

    d->colorPalette = new ColorPalette( ":/palette" );
    connect( d->colorPalette, SIGNAL( selectedColor( QColor ) ), SIGNAL( selectedColor( QColor ) ) );

    d->tButtonSelectColor = new QToolButton();
    d->tButtonSelectColor->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
    d->tButtonSelectColor->setText( tr( "Another color" ) );
    d->tButtonSelectColor->setIcon( QIcon( ":/icon palette" ) );
    connect( d->tButtonSelectColor, SIGNAL( clicked() ), SLOT( anotherColor() ) );


    QVBoxLayout * layout = new QVBoxLayout();
    layout->addWidget( d->tButtonDefaultColor );
    layout->addWidget( d->colorPalette );
    layout->addWidget( d->tButtonSelectColor );
    layout->addStretch();

    QMenu * menu = new QMenu();
    menu->setLayout( layout );

    setMenu( menu );
}


void ColorButton::defaultColor()
{qDebug() << "ColorButton::defaultColor()";
    selectedColor( Qt::black );
}

void ColorButton::anotherColor()
{
    const QColor & color = QColorDialog::getColor( d->color, this );

    if( !color.isValid() )
        return;

    selectedColor( color );
}


void ColorButton::setColor( const QColor & color )
{
    d->color = color;

    QPixmap pixmap( ":/color-text" );

    QPainter painter( &pixmap );
    painter.setBrush( color );
    painter.setPen( color );
    painter.drawRect( 0, pixmap.height() - 25, pixmap.width(), pixmap.height() - 25 );

    setIcon( pixmap );
}
void ColorButton::setDefaultColor( const QColor & color )
{
    QPixmap pixmap( 16, 16 );
    pixmap.fill( color );
    d->tButtonDefaultColor->setIcon( pixmap );
}


ColorPalette::ColorPalette( const QString & path, QWidget * parent )
    : QLabel( parent )
{
    setAutoFillBackground( true );
    setMouseTracking( true );

    setImage( path );
}
ColorPalette::ColorPalette( QWidget * parent )
    : QLabel( parent )
{
    setAutoFillBackground(true);
    setMouseTracking(true);
}

void ColorPalette::setImage( const QString & path )
{
    QPixmap pixmap( path );
    setMask( pixmap.mask() );

    QPalette palette;
    palette.setBrush( backgroundRole(), QBrush( pixmap ) );

    setPalette( palette );
    setFixedSize( pixmap.size() );
}


void ColorPalette::mousePressEvent( QMouseEvent * event )
{
    QRect onePixRect( event->pos(), QSize( 1, 1 ));

    QPixmap pix = QPixmap::grabWidget( this, onePixRect );
    QImage img = pix.toImage();

    emit selectedColor( QColor( img.pixel( 0, 0 ) ) );
}
