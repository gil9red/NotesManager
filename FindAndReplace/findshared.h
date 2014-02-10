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


#ifndef FindShared_H
#define FindShared_H

#include <QFlags>

//! Содержит глобальные общие перечисления, переменные, классы и функции.
namespace Shared
{
    //! Поиск.
    /*! Перечисление флагов поиска. */
    enum FindFlag
    {
        CaseSensitive = 0x0001, //!< Чувствительность к регистру
        WholeWords    = 0x0002, //!< Только целые слова
        RegExp        = 0x0004  //!< Использование регулярных выражений
    };
    Q_DECLARE_FLAGS ( FindFlags, FindFlag )
}

Q_DECLARE_OPERATORS_FOR_FLAGS ( Shared::FindFlags )

#include <QIcon>
#include <QPainter>

//! Содержит функции, связанные с виджетами-поисковиками.
namespace Find
{
    //! Функция, в зависимости от переданных флагов возвращает иконку.
    /*! \param flags поисковые флаги
     *  \return иконка
     */
    QIcon iconForFindFlags( Shared::FindFlags flags );
}

#endif // FindShared_H
