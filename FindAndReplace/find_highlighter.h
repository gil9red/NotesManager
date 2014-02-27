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


#ifndef Find_Highlighter_H
#define Find_Highlighter_H

#include <QSyntaxHighlighter>
#include "findshared.h"

//! Подсвечивает обнаруженные совпадения, при заданной строке поиска и флагов.
class Find_Highlighter: public QSyntaxHighlighter
{
    Q_OBJECT

public:
    //! Принимает в качестве обязательного параметра указатель на документ.
    Find_Highlighter( QTextDocument * document );

    //! Указание текста, который будем искать и флагов.
    void setHighlightText( const QString & text, Shared::FindFlags flags );

    //! Цвет подсветки.
    void setHighlightColor( const QColor & color );

private:
    QString expression;          //!< Поисковое выражение
    QTextCharFormat format;      //!< Правила подсветки
    Shared::FindFlags findFlags; //!< Поисковые флаги

public slots:    
    void clearHighlight(); //! Очистить от подсветки.

protected:
    //! Вызов подсветки для текущего блока.
    void highlightBlock( const QString & text );
};

#endif
