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

#ifndef ABSTRACTNOTEBODY_H
#define ABSTRACTNOTEBODY_H

#include <QMainWindow>
#include <QColor>
#include <QSize>
#include <QWidget>
#include <QToolBar>
#include <QStatusBar>
#include <QPaintEvent>

class d_AbstractBody;

#include "NoteShared.h"

//! Класс реализует тело абстрактной заметки.
/*! Этот класс содержит центральный виджет заметки (например, текстовый редактор).
 *  Также класс дает панель инструментов.
 *  \sa AbstractNote
 */
class AbstractNoteBody: public QMainWindow
{
    Q_OBJECT

public:
    AbstractNoteBody( QWidget * parent = 0 );
    ~AbstractNoteBody();

    //! Устанавливает цвет.
    /*! \sa color() */
    void setColor( const QColor & color );

    //! Возвращение цвета.
    /*! \sa setColor() */
    QColor color();

    //! Изменение видимости панели инструментов.
    /*! \sa isVisibleToolBar() */
    void setVisibleToolBar( bool visible );

    //! Возвращение видимости панели инструментов.
    /*! \retval true - панель видима
     *  \retval false - панель невидима
     *  \sa setVisibleToolBar()
     */
    bool isVisibleToolBar();

    void setToolBarIconSize( const QSize & size );
    QSize toolBarIconSize();

    //! Указание центрального виджета.
    /*! Центральный виджет будет размещен на виджете.
     *  \param w указатель на центральный виджет
     *  \sa widget()
     */
    void setWidget( QWidget * widget );

    //! Функция возвращает указатель на центральный виджет.
    /*! \sa setWidget() */
    QWidget * widget();

    //! Добавление виджета на панель инструментов заметки.
    /*! \param w указатель на добавляемый виджет
     *  \sa addSeparatorToToolBar()
     */
    void addWidgetToToolBar( QWidget * widget );

    //! Добавление разделителя на панель инструментов.
    /*! \sa addWidgetToToolBar() */
    void addSeparator();

    //! Функция возвращает указатель на панель инструментов.
    /*! \sa setWidget() */
    QToolBar * toolBar();

public:
    d_AbstractBody * d; //!< Дата
//    QStatusBar * statusBar; //!< Панель состояний

private:
    QToolBar * tBar; //!< Панель инструментов

protected:
    void paintEvent( QPaintEvent * );

signals:
    //! Сигнал отсылается при изменении видимости панели инструментов.
    void changeVisibleToolbar( bool );
};

//! Класс данных абстрактной заметки
/*! \sa AbstractNoteBody */
class d_AbstractBody
{
public:
    QColor color; //!< Цвет
};

#endif // ABSTRACTNOTEBODY_H
