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


#ifndef TextEditor_H
#define TextEditor_H

#include <QTextEdit>
#include <QKeyEvent>
#include <QTextBrowser>
#include <QApplication>
#include <QKeyEvent>
#include <QDebug>

#include "completer.h"
#include "utils/func.h"

//! Редактор заметки
class TextEditor: public QTextBrowser
{
    Q_OBJECT

public:
    TextEditor( QWidget * parent = 0 );

    //! Активация ссылок в редакторе
    void setActivateLink( bool activate );

private slots:
    void openLink( QUrl url ); //!< Открытие ссылки

signals:
    //!< Сигнал отсылается при открытии ссылки
    void isOpenLink( bool );

protected:
    void focusInEvent( QFocusEvent * event );
    void keyPressEvent( QKeyEvent * event );
    void keyReleaseEvent( QKeyEvent * event );
    void changeEvent( QEvent * event );
    void contextMenuEvent( QContextMenuEvent * event );
};

#endif
