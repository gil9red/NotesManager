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


#ifndef PAGE_ABOUT_H
#define PAGE_ABOUT_H

#include <QMainWindow>

namespace Ui
{
    class Page_About;
}

//! Класс, реализующий виджет "О программе".
class Page_About: public QMainWindow
{
    Q_OBJECT

public:
    //! Вкладки окна
    enum Tab
    {
        About,           //!< О программе
        RevisionHistory, //!< История изменений
        License,         //!< Лицензия
        Documentation    //!< Документация
    };

public:
    explicit Page_About(QWidget *parent = 0);
    ~Page_About();
    
public slots:
    void setCurrentTab( int index ); //!< Установить текущую вкладку
    int currentTab(); //!< Индекс текущей вкладки

private:
    Ui::Page_About * ui; //!< Форма UI.
};

#endif // PAGE_ABOUT_H
