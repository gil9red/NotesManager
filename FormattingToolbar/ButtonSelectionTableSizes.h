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


#ifndef BUTTONSELECTIONTABLESIZES_H
#define BUTTONSELECTIONTABLESIZES_H

#include <QToolButton>
#include <QLabel>

#include "WidgetSelectionTableSizes.h"

//! Кнопка с возможностью выбрать размер таблицы.
class ButtonSelectionTableSizes: public QToolButton
{
    Q_OBJECT

public:
    explicit ButtonSelectionTableSizes( QWidget * parent = 0 );

private:
    //! Отображение информации, например текущий выбранный размер таблицы.
    QLabel label;

    //! Виджет, который дает возможность курсором мыши выбрать размер таблицы.
    WidgetSelectionTableSizes widgetSelectionTableSizes;

private slots:
    //! Слот вызывается при изменении выбора размера таблицы у widgetSelectionTableSizes
    void changeSelected( int rows, int cols );

signals:
    //! Сигнал вызывается, при выборе размера таблицы
    void selected( int rows, int cols );
};

#endif // BUTTONSELECTIONTABLESIZES_H
