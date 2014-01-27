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

#ifndef ABSTRACTNOTEHEAD_H
#define ABSTRACTNOTEHEAD_H

#include <QWidget>
#include <QLabel>
#include <QString>
#include <QColor>
#include <QFont>
#include <QPoint>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QLineEdit>

class d_AbstractHead;

#include "NoteShared.h"

//! Класс реализует голову (шапку) абстрактной заметки.
/*! Этот класс отображает название заметки и также через него
 *  возможно перемещение заметки по экрану
 */
class AbstractNoteHead: public QWidget
{
    Q_OBJECT

public:
    AbstractNoteHead( QWidget * parent = 0 );
    ~AbstractNoteHead();

    //! Установка заголовка.
    /*! \param str текст
     *  \sa title()
     */
    void setTitle( const QString & str );

    //! Возвращение заголовка.
    /*! \sa setTitle() */
    QString title();

    //! Установка шрифта заголовка.
    /*! \param f класс шрифта
     *  \sa font()
     */
    void setFont( const QFont & f );

    //! Возвращение шрифта заголовка.
    /*! \sa setFont() */
    QFont font();

    //! Устанавливает цвет.
    /*! \sa color() */
    void setColor( const QColor & color );

    //! Возвращение цвета.
    /*! \sa setColor() */
    QColor color();

public:
    d_AbstractHead * d; //!< Дата
    QLabel * label;     //!< Класс, который отображает текст заголовка

protected:
    void paintEvent( QPaintEvent * );
    void mousePressEvent( QMouseEvent * event );
    void mouseReleaseEvent( QMouseEvent *  );
    void mouseMoveEvent( QMouseEvent * event );
};


//! Класс данных шапки абстрактной заметки
/*! \sa AbstractNoteHead, d_AbstractNote */
class d_AbstractHead
{
public:
    QColor color;       //!< Цвет
//    QString text;       //!< Текст
//    QFont font;         //!< Шрифт текста
    QPoint oldPosition; //!< Старая позиция
    bool isMousePress;  //!< Текущее состояние левой кнопки мыши
};

#endif // ABSTRACTNOTEHEAD_H
