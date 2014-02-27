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


#ifndef UBUTTONORDEREDLIST_H
#define UBUTTONORDEREDLIST_H

#include "ButtonList.h"

#include <QMenu>
#include <QTextListFormat>
#include <QVBoxLayout>

//! Кнопка с списком "упорядоченный текст".
class ButtonOrderedList: public ButtonList
{
  Q_OBJECT

public:
    ButtonOrderedList( QWidget *parent = 0 )
        : ButtonList( parent )
    {
        setCheckable( true );
        setPopupMode( QToolButton::MenuButtonPopup );
        setIcon( QIcon( ":/list-decimal" ) );
        setDefaultId( QTextListFormat::ListDecimal );

        QSize size( 60, 60 );
        QSize iconSize( 50, 50 );

        tButtonDecimal = Create::Button::get( size, iconSize, QIcon( ":/list-decimal" ));
        tButtonAlphaLower = Create::Button::get( size, iconSize, QIcon( ":/list-lower-alpha" ));
        tButtonAlphaUpper = Create::Button::get( size, iconSize, QIcon( ":/list-upper-alpha" ));
        tButtonRomanLower = Create::Button::get( size, iconSize, QIcon( ":/list-lower-roman" ));
        tButtonRomanUpper = Create::Button::get( size, iconSize, QIcon( ":/list-upper-roman" ));

        QMenu * menu = new QMenu();
        menu->setLayout( new QVBoxLayout() );
        setMenu( menu );
        menu->layout()->setSpacing( 0 );
        menu->layout()->setMargin( 0 );
        menu->layout()->addWidget( tButtonDecimal );
        menu->layout()->addWidget( tButtonAlphaLower );
        menu->layout()->addWidget( tButtonAlphaUpper );
        menu->layout()->addWidget( tButtonRomanLower );
        menu->layout()->addWidget( tButtonRomanUpper );

        addButton( tButtonDecimal, QTextListFormat::ListDecimal );
        addButton( tButtonAlphaLower, QTextListFormat::ListLowerAlpha );
        addButton( tButtonAlphaUpper, QTextListFormat::ListUpperAlpha );
        addButton( tButtonRomanLower, QTextListFormat::ListLowerRoman );
        addButton( tButtonRomanUpper, QTextListFormat::ListUpperRoman );
    }

private:
    QToolButton * tButtonDecimal;
    QToolButton * tButtonAlphaLower;
    QToolButton * tButtonAlphaUpper;
    QToolButton * tButtonRomanLower;
    QToolButton * tButtonRomanUpper;
};

#endif
