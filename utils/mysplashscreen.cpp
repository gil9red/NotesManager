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

#include "mysplashscreen.h"
#include <QApplication>

MySplashScreen::MySplashScreen( const QPixmap & pixmap, Qt::WindowFlags f )
    : QSplashScreen( pixmap, f )
{
    setWindowFlags( windowFlags() | Qt::WindowStaysOnTopHint );
    setCursor( Qt::ArrowCursor );

    labelMessage = new QLabel();

    QVBoxLayout * layout = new QVBoxLayout();
    layout->addSpacing( 60 );
    layout->addWidget( labelMessage, 0, Qt::AlignCenter );
    setLayout(layout);
}

void MySplashScreen::setMessage( const QString & message, const QFont & font, const QColor & color )
{
    qApp->processEvents();

    labelMessage->setText( QString( "<font color=\"%2\">%1</font>" ).arg( message ).arg( color.name() ) );
    labelMessage->setFont( font );
}

void MySplashScreen::mousePressEvent( QMouseEvent * event )
{
    if ( event->button() == Qt::LeftButton )
    {
        oldPosition = event->pos();
        buttonMousePress = true;
    }
}
void MySplashScreen::mouseReleaseEvent( QMouseEvent * event )
{
    buttonMousePress = false;
}
void MySplashScreen::mouseMoveEvent( QMouseEvent * event )
{
    if ( buttonMousePress )
    {
        QPoint delta = event->pos() - oldPosition;
        move( pos() + delta );
    }
}
