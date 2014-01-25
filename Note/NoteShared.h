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

#ifndef NoteShared_H
#define NoteShared_H

#include <QFlags>
#include <Qt>
#include <QColor>

//! Содержит глобальные общие перечисления, переменные, классы и функции.
namespace Shared
{
    //! Стороны.
    /*! Перечисление сторон рамки заметки. */
    enum Side
    {
        Left = 0x0001,  //!< Левая
        Right = 0x0002, //!< Правая
        Top = 0x0004,   //!< Верхняя
        Bottom = 0x0008 //!< Нижняя
    };
    Q_DECLARE_FLAGS ( Sides, Side )
}

Q_DECLARE_OPERATORS_FOR_FLAGS ( Shared::Sides )

//! Содержит значения по умолчанию абстрактной заметки.
namespace Abstract
{
    const int topBorder = 31; //!< Высота шапки заметки
    const int minimalHeight = 75; //!< Минимальная высота заметки

    //! Окно без кнопки на панели задач и без системной рамки.
    const Qt::WindowFlags flags = Qt::Tool | Qt::FramelessWindowHint;

    //! Содержит описание параметров рамки заметки (по умолчанию).
    namespace Frame
    {
        const bool visible = true;      //!< Видимость
        const float width = 1.0;        //!< Ширина (толщина)
        const QColor color = Qt::black; //!< Цвет
    }
}

#endif // NoteShared_H
