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


#ifndef PAGE_NOTES_H
#define PAGE_NOTES_H

#include <QMainWindow>
#include <QStandardItemModel>

namespace Ui
{
    class Page_notes;
}

#include "ui_page_notes.h"
#include "Note/RichTextNote.h"
#include "Note/AttachPanel.h"

//! Класс, реализующий виджет "Заметки".
class Page_Notes: public QMainWindow
{
    Q_OBJECT
    
public:
    explicit Page_Notes( QWidget * parent = 0, QAbstractItemModel * m = 0 );
    ~Page_Notes();

    //! Сортировка указанного столбца.
    void sort( int column, Qt::SortOrder sort = Qt::AscendingOrder );

    //! Индекс отсортированного столбца.
    int sortColumn();

    //! Порядок сортировки.
    Qt::SortOrder sortOrder();

    //! Функция возвращает текущий индекс.
    QModelIndex currentIndex() const;

    //! Функция возвращает текущий выделенную строку.
    int currentRow();

    //! Функция возвращает true, если есть выделенные ячейки.
    bool hasSelection();

    //! Отобразить содержимое заметки, находящаяся в строке row.
    void displayContentNote( int row );

    //! Очистить с дисплея-виджета содержимое заметки.
    void clearContentNote();

    //! В функции указываем с каким классом настроек будем работать.
    void setSettings( QSettings * s );

public:
    Ui::Page_notes * ui;                  //!< Форма UI.
    QAbstractItemModel * model;           //!< Модель хранит заметки.
    QItemSelectionModel * selectionModel; //!< Модель выделения элементов.
    QSettings * settings; //!< Класс настройки.

public slots:
    void readSettings();  //!< Считывание настроек.
    void writeSettings(); //!< Запись настроек.

private slots:
    //! Произошел клик на элемент в таблице/списке.
    void clicked( const QModelIndex &index );

    //! Обновление состояния.
    void updateStates();

    //! Изменение выделения элементов.
    void selectionChanged(QItemSelection,QItemSelection);

    //! Вызывается при срабатывании AbstractNote::changed().
    void noteChange( int index );

signals:
    //! Вызывается при изменении выделения элементов.
    void changeSelection();
};

#endif // PAGE_NOTES_H
