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


#ifndef PAGE_SETTINGS_H
#define PAGE_SETTINGS_H

#include <QMainWindow>
#include <QSettings>
#include <QTreeWidgetItem>
#include <QAbstractButton>
#include <QVariantMap>

namespace Ui
{
    class Page_Settings;
}

//! Класс, реализующий виджет "Настройки".
class Page_Settings: public QMainWindow
{
    Q_OBJECT
    
public:
    explicit Page_Settings( QWidget * parent = 0 );
    ~Page_Settings();
        
    //! В функции указываем с каким классом настроек будем работать.
    void setSettings( QSettings * s );

    void mapToSettings();
    void settingsToMap();

public:
    Ui::Page_Settings * ui; //!< Форма UI.
    QSettings * settings; //!< Класс настройки.
    QVariantMap mapSettings;

public slots:
    void readSettings();  //!< Считывание настроек.
    void writeSettings(); //!< Запись настроек.

private slots:
    void on_treeWidgetMenu_itemClicked(QTreeWidgetItem *item, int column);
    void on_buttonBox_clicked(QAbstractButton *button);

signals:
    void acceptChangeSettings();
    void message( const QString & text, int msec );
};

#endif // PAGE_SETTINGS_H
