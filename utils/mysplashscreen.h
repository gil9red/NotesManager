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

#ifndef UANIMATEDSPLASHSCREEN_H
#define UANIMATEDSPLASHSCREEN_H

#include <QSplashScreen>
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <QMouseEvent>

//! Класс, который создает заставку.
/*! Наследник QSplashScreen, адаптированный под текущий проект. */
class MySplashScreen: public QSplashScreen
{
    Q_OBJECT

public:
    MySplashScreen( const QPixmap & pixmap = QPixmap(), Qt::WindowFlags f = 0 );

private:
    QLabel * labelMessage; //!< Текущая информацию на экранной заставке.
    QPoint oldPosition;    //!< Старая позиция окна.
    bool buttonMousePress; //!< Текущее состояние левой кнопки мыши.

public slots:
    //! Установка сообщения на экранной заставке.
    /*! \param message текст сообщения
     *  \param font шрифт текста
     *  \param color цвет текста
     */
    void setMessage( const QString & message, const QFont & font, const QColor & color = Qt::black );

protected:
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
};

#endif // UANIMATEDSPLASHSCREEN_H
