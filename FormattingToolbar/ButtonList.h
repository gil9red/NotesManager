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


#ifndef BUTTONLIST_H
#define BUTTONLIST_H

#include <QToolButton>
#include <QButtonGroup>
#include "utils/func.h"

//! Кнопка, которая объединяет другие кнопки.
class ButtonList: public QToolButton
{
    Q_OBJECT

public:
    ButtonList( QWidget * parent = 0 );

    //! Добавление связи с кнопкой и указание ее id.
    void addButton( QAbstractButton * button, int id );

    //! Установление id по умолчанию.
    /*! \sa defaultId() */
    void setDefaultId( int id );

    //! Возврат стиля (id) по умолчанию.
    /*! \sa setDefaultId() */
    int defaultId();

private:
    QButtonGroup buttonGroup; //!< Группа кнопок.
    int d_defaultId;          //!< id по умолчанию.

public slots:
    //! Функция возвращает указатель на кнопку, которая является контрольной.
    /*! \param style идентификатор кнопки */
    QAbstractButton * checkedButton( int id );

private slots:
    //! Вызов значения по умолчанию.
    /*! \sa selected() */
    void clickDefault();

signals:
    //! Вызывается при клике кнопки из группы.
    /*! Передает id кнопки */
    void selected( int );
};

#endif
