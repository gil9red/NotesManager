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


#include "fadclearlineedit.h"

FadClearLineEdit::FadClearLineEdit( const QString & text, QWidget * parent )
    : FadLineEdit( text, parent )
{
    init();
    updateStates();
}
FadClearLineEdit::FadClearLineEdit( QWidget * parent )
    : FadLineEdit( parent )
{
    init();
    updateStates();
}

void FadClearLineEdit::init()
{
    tButtonClearText.setCursor( Qt::ArrowCursor );
    tButtonClearText.setFocusPolicy( Qt::NoFocus );
    tButtonClearText.setStyleSheet( "QToolButton { border: none; }" );
    tButtonClearText.setToolButtonStyle( Qt::ToolButtonIconOnly );
    tButtonClearText.setIcon( QIcon( ":/text-clear" ) );
    tButtonClearText.setIconSize( QSize( 14, 14 ) );
    tButtonClearText.setToolTip( tr( "Clear text" ) );
    tButtonClearText.setStatusTip( tr( "Clear text" ) );
    tButtonClearText.setWhatsThis( tr( "Clear text" ) );
    addButton( &tButtonClearText );

    connect( &tButtonClearText, SIGNAL( clicked() ), SLOT( clear() ) );
    connect( &tButtonClearText, SIGNAL( clicked() ), SIGNAL( cleansingText() ) );
    connect( &tButtonClearText, SIGNAL( clicked() ), SLOT( updateStates() ) );

    connect( this, SIGNAL( textEdited(QString) ), SLOT( updateStates() ) );
}


void FadClearLineEdit::updateStates()
{
    tButtonClearText.setVisible( !text().isEmpty() );
}
