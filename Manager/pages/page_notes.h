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

#ifndef PAGE_NOTES_H
#define PAGE_NOTES_H

#include <QMainWindow>
#include <QStandardItemModel>

namespace Ui
{
    class Page_notes;
}

#include "ui_page_notes.h"

#include <QSettings>

class QDomElement;
class QDomDocument;

class TrashModelItem;

//! Класс, реализующий виджет "Заметки".
class Page_Notes: public QMainWindow
{
    Q_OBJECT
    
public:
    explicit Page_Notes( QWidget * parent = 0 );
    ~Page_Notes();

private:
    BaseModelItem * createItemOfDomElement( const QDomElement & element );
    void parseDomElement( BaseModelItem * node, QDomElement & element );
    QDomElement createDomElementOfItem( BaseModelItem * item, QDomDocument & xmlDomDocument );
    void parseItem( BaseModelItem * node, QDomElement & element, QDomDocument & xmlDomDocument );
    void removeItem( BaseModelItem * item, BaseModelItem * parent );

public:
    Ui::Page_notes * ui;
    QSettings * settings;
    QStandardItemModel model;
    TrashModelItem * itemTrash;

    QHash < QStandardItem * , RichTextNote * > hashItemNote;

private slots:
    void noteChanged( int event );
    void noteChanged( QStandardItem * item );

public slots:
    bool read( QIODevice * device );
    bool write( QIODevice * device );

    void readSettings();
    void writeSettings();

    void addTopLevelNote();
    void addTopLevelFolder();
    void addFolder();
    void addNote();
    void rename();
    void open();
    void removeToTrash();
    void removeAllToTrash();
    void removeFromTrash();
    void clearTrash();

    void textColor();
    void defaultTextColor();
    void backColor();
    void defaultBackColor();

    void showContextMenu( const QPoint & pos );
};

#endif // PAGE_NOTES_H
