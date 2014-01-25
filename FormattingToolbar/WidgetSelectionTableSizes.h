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


#ifndef WIDGETSELECTROWSCOLUMNSTABLE_H
#define WIDGETSELECTROWSCOLUMNSTABLE_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>

//! Виджет графического выбора размера таблицы.
class WidgetSelectionTableSizes: public QWidget
{
    Q_OBJECT

public:
    WidgetSelectionTableSizes( QWidget *parent = 0 );

    void setRowCount( int rowCount );       //!< Установка количества строк.
    void setColumnCount( int columnCount ); //!< Установка количества столбцов.
    void setSizeCell( float size );         //!< Установка размера ячейки таблицы.
    void setIndent( float indent );         //!< Установка отступа между ячейками.

private:
    int rowCount;       //!< Количество строк.
    int columnCount;    //!< Количество столбцов.
    float size;         //!< Установка количества строк.
    float indent;       //!< Отступ между ячейками.
    int selectedRow;    //!< Индекс выбранной строки.
    int selectedColumn; //!< Индекс выбранного столбца.

signals:
    //! Вызывается при изменении размера таблицы.
    void changeSelected( int row, int column );

    //! Вызывается при выборе размера таблицы.
    void selected( int row, int column );

protected:
    void mouseMoveEvent( QMouseEvent *event );
    void paintEvent( QPaintEvent * );
    void mouseReleaseEvent( QMouseEvent *event );
    void leaveEvent( QEvent * );
};

#endif
