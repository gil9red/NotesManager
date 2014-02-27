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


#ifndef FadLineEdit_H
#define FadLineEdit_H

#include <QLineEdit>
#include <QList>
#include <QAbstractButton>
#include <QHBoxLayout>

//! Однострочный редактор с возможностью добавления на него кнопок.
class FadLineEdit: public QLineEdit
{
    Q_OBJECT

public:
    //! Положение кнопки.
    enum Side
    {
        None, //!< Никакое
        Left, //!< Слева
        Right //!< Справа
    };

public:    
    //! Создает однострочный редактор уже с текстом.
    /*! \param text строка редактора
     *  \param parent указатель на виджет-родитель
     */
    explicit FadLineEdit( const QString & text, QWidget * parent = 0 );

    /*! Перегруженный конструктор. */
    explicit FadLineEdit( QWidget * parent = 0 );

    //! Добавление кнопки на текстовый редактор.
    /*! \param button указатель на кнопку
     *  \param side положение кнопки (Left или Right)
     */
    void addButton( QAbstractButton * button, Side side = Right );

    //! Удаление кнопки из текстового редактора.
    /*! Функция вернет не 0 указатель, если кнопку удалось обнаружить и убрать из редактора.
     *  \param button указатель на кнопку
     *  \param side положение кнопки (Left или Right)
     */
    QAbstractButton * removeButton( QAbstractButton * button, Side side = Right );

    //! Функция вернет список кнопок, находящиеся слева.
    QList < QAbstractButton * > * leftSideButtons();

    //! Функция вернет список кнопок, находящиеся справа.
    QList < QAbstractButton * > * rightSideButtons();

    //! Функция вернет положение кнопки (Side) на текстовом редакторе.
    /*! \param button указатель на кнопку */
    Side sideButton( QAbstractButton * button );

private:
    void init();               //!< Инициализация класса
    void refreshTextMargins(); //!< Обновление внутренней геометрии класса

protected:
    QList < QAbstractButton * > d_leftSideButtons;  //!< Cписок кнопок, находящиеся слева.
    QList < QAbstractButton * > d_rightSideButtons; //!< Cписок кнопок, находящиеся справа.

    QHBoxLayout leftSideLayout;  //! Компоновщик кнопок, находящиеся слева.
    QHBoxLayout rightSideLayout; //! Компоновщик кнопок, находящиеся справа.
};

#endif
