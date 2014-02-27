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


#ifndef ButtonBulletedList_h
#define ButtonBulletedList_h

#include "ButtonList.h"

#include <QMenu>
#include <QVBoxLayout>
#include <QTextCharFormat>

//! Кнопка с списком "маркированный текст".
class ButtonBulletedList: public ButtonList
{
  Q_OBJECT

public:
    ButtonBulletedList( QWidget * parent = 0 )
        : ButtonList( parent )
    {
        setCheckable( true );
        setPopupMode( QToolButton::MenuButtonPopup );
        setIcon( QIcon( ":/list-disc" ) );
        setDefaultId( QTextListFormat::ListDisc );

        QSize size( 60, 60 );
        QSize iconSize( 50, 50 );

        tButtonSquare = Create::Button::get( size, iconSize, QIcon( ":/list-square" ));
        tButtonDisc = Create::Button::get( size, iconSize, QIcon( ":/list-disc" ));
        tButtonCircle = Create::Button::get( size, iconSize, QIcon( ":/list-circle" ));

        QMenu * menu = new QMenu();
        menu->setLayout(new QVBoxLayout());
        setMenu( menu );
        menu->layout()->setSpacing( 0 );
        menu->layout()->setMargin( 0 );
        menu->layout()->addWidget( tButtonSquare );
        menu->layout()->addWidget( tButtonDisc );
        menu->layout()->addWidget( tButtonCircle );

        addButton( tButtonCircle, QTextListFormat::ListCircle );
        addButton( tButtonDisc, QTextListFormat::ListDisc );
        addButton( tButtonSquare, QTextListFormat::ListSquare );
    }

private:
    QToolButton * tButtonSquare;
    QToolButton * tButtonDisc;
    QToolButton * tButtonCircle;
};

#endif

