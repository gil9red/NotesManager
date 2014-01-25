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

#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <QStringListModel>
#include <QFileInfo>

//! Класс-словарь.
/*! Хранит список слов, умеет списки слов загружать из файлов.
 *  \sa Completer
 */
class Dictionary: public QObject
{
  Q_OBJECT

public:
    Dictionary( QObject * parent = 0 );

    //! Функция загружает из файла список слов.
    /*! \param fileName путь к файлу
     *  \return true, если загрузка прошла удачно, иначе false
     */
    bool load( const QString & fileName );

    //! Функция вернет true, если загрузка из файла прошла успешно.
    bool isLoad();

    //! Функция вернет указатель на модель словаря
    QStringListModel * model();

    //! Функция вернет название языка словаря
    QString language();

private:
    QStringListModel d_model; //!< Модель
    QString d_language;       //!< Название языка
    bool d_isLoad;            //!< true - если загрузка прошла удачно
};

#endif // DICTIONARY_H
