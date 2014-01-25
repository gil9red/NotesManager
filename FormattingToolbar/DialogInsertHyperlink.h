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


#ifndef DIALOGINSERTHYPERLINK_H
#define DIALOGINSERTHYPERLINK_H

#include <QDialog>

namespace Ui
{
    class DialogInsertHyperlink;
}

//! Диалог вставки гиперссылки.
class DialogInsertHyperlink: public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogInsertHyperlink( QWidget * parent = 0 );
    ~DialogInsertHyperlink();

    QString hyperlink(); //!< Функция возвращает строку с гиперссылкой.
    QString text();      //!< Функция возвращает строку текста гиперссылки.
    
private:
    Ui::DialogInsertHyperlink * ui; //!< Форма UI класса.

private slots:
    void updateStates(); //!< Обновление состояния.
};

#endif // DIALOGINSERTHYPERLINK_H
