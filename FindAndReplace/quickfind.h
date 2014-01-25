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


#ifndef QuickFind_H
#define QuickFind_H

#include "findandreplace.h"

//! Виджет быстрого поиска текста.
class QuickFind: public FindAndReplace
{
    Q_OBJECT

public:
    //! Конструктор.
    /*! \param textedit указатель на текстовый редактор,
     *  в котором и будем делать поиск и замену текста, а также
     *  подсветку найденных строк
     *  \parent указатель на виджет-родитель
     */
    QuickFind( QTextEdit * textedit = 0, QWidget * parent = 0 );
};

#endif
