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


#ifndef UBUTTONUNDERLINESTYLELIST_H
#define UBUTTONUNDERLINESTYLELIST_H

#include "ButtonList.h"
#include "ColorButton.h"

#include <QMenu>
#include <QVBoxLayout>
#include <QTextCharFormat>

//! Кнопка с списком "стили подчеркивающей текст линии".
class ButtonUnderlineStyleList: public ButtonList
{
  Q_OBJECT

public:
    ButtonUnderlineStyleList( QWidget * parent = 0 )
        : ButtonList( parent )
    {
        setCheckable( true );
        setPopupMode( QToolButton::MenuButtonPopup );
        setIcon( QIcon( ":/underline" ) );
        setDefaultId( QTextCharFormat::SingleUnderline );

        const QSize size = QSize(160, 20);
        const QSize iconSize = QSize(155, 20);

        tButtonNoUnderline = new QToolButton();
        tButtonNoUnderline->setText( tr( "No underline" ) );
        tButtonNoUnderline->setFixedSize( size );

        tButtonSingleUnderline = Create::Button::get( size, iconSize, QIcon( ":/underline-single" ));
        tButtonDashUnderline = Create::Button::get( size, iconSize, QIcon( ":/underline-dash" ));
        tButtonDotLine = Create::Button::get( size, iconSize, QIcon( ":/underline-dot" ));
        tButtonDashDotLine = Create::Button::get( size, iconSize, QIcon( ":/underline-dash-dot" ));
        tButtonDashDotDotLine = Create::Button::get( size, iconSize, QIcon( ":/underline-dash-dot-dot" ));
        tButtonWaveUnderline = Create::Button::get( size, iconSize, QIcon( ":/underline-wave" ));

        colorButton = new ColorButton();
        colorButton->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
        colorButton->setText( tr( "Color underline" ) );
        colorButton->setFixedWidth( size.width() );


        QMenu * menu = new QMenu();
        setMenu( menu );
        menu->setLayout( new QVBoxLayout() );
        menu->layout()->setSpacing( 0 );
        menu->layout()->setMargin( 0 );
        menu->layout()->addWidget( tButtonNoUnderline );
        menu->layout()->addWidget( tButtonSingleUnderline );
        menu->layout()->addWidget( tButtonDashUnderline );
        menu->layout()->addWidget( tButtonDotLine );
        menu->layout()->addWidget( tButtonDashDotLine );
        menu->layout()->addWidget( tButtonDashDotDotLine );
        menu->layout()->addWidget( tButtonWaveUnderline );
        menu->layout()->addWidget( colorButton );

        addButton( tButtonNoUnderline, QTextCharFormat::NoUnderline );
        addButton( tButtonSingleUnderline, QTextCharFormat::SingleUnderline );
        addButton( tButtonDashUnderline, QTextCharFormat::DashUnderline );
        addButton( tButtonDotLine, QTextCharFormat::DotLine );
        addButton( tButtonDashDotLine, QTextCharFormat::DashDotLine );
        addButton( tButtonDashDotDotLine, QTextCharFormat::DashDotDotLine );
        addButton( tButtonWaveUnderline, QTextCharFormat::WaveUnderline );

        connect( colorButton, SIGNAL( selectedColor(QColor) ), SIGNAL( selected(QColor) ) );
    }

private:
    QToolButton * tButtonNoUnderline;
    QToolButton * tButtonSingleUnderline;
    QToolButton * tButtonDashUnderline;
    QToolButton * tButtonDotLine;
    QToolButton * tButtonDashDotLine;
    QToolButton * tButtonDashDotDotLine;
    QToolButton * tButtonWaveUnderline;
    ColorButton * colorButton;

public slots:
    void setColor( const QColor & color )
    {
        colorButton->setColor( color );
    }

signals:
    void selected(QColor);
};

#endif
