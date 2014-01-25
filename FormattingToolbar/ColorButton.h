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


#ifndef UCOLORBUTTON_H
#define UCOLORBUTTON_H

#include <QToolButton>
#include <QLabel>
#include <QBitmap>
#include <QMouseEvent>

class ColorPalette;
class d_ColorButton;

//! Кнопка выбора цвета.
class ColorButton: public QToolButton
{
    Q_OBJECT

public:
    ColorButton( QWidget *parent = 0 );
    ~ColorButton();

    QColor color(); //!< Текущий цвет.

private:
    void createGUI(); //!< Создание гуи.

protected:
    d_ColorButton * d; //!< Данные.

private slots:
    void defaultColor(); //!< Вызов цвета по умолчанию.
    void anotherColor(); //!< Вызов диалога выбора цвета.

public slots:
    //! Выбор текущего цвета.
    void setColor( const QColor & color );

    //! Выбор цвета по умолчанию.
    void setDefaultColor( const QColor & color );

signals:
    //! При выборе цвета, отсылается сигнал.
    void selectedColor( QColor );
};

//! Класс данных кнопки выбора цвета.
/*! \sa ColorButton */
class d_ColorButton
{
public:
    QToolButton * tButtonDefaultColor; //!< Кнопка "Цвет по умолчанию".
    QToolButton * tButtonSelectColor;  //!< Кнопка "Другой цвет".
    ColorPalette * colorPalette;       //!< Палитра.
    QColor color;                      //!< Текущий цвет.
};

// TODO: создать отдельный h/cpp для палитры

//! Палитра цветов.
/*! Класс позволяет получить значение пикселя на своем окне. */
class ColorPalette: public QLabel
{
    Q_OBJECT

public:
    //! Конструктор.
    /*! \param path путь до файла картинки, которая и есть палитра
     *  \param parent указатель на виджет-родитель
     */
    ColorPalette( const QString & path, QWidget * parent = 0 );

    /*! Перегруженный конструктор. */
    ColorPalette( QWidget * parent = 0 );

public slots:
    //! Выбора картинки-палитры.
    /*! \param path путь до файла картинки, которая и есть палитра */
    void setImage( const QString & path );

protected:
    void mousePressEvent( QMouseEvent * event );

signals:
    //! При выборе цвета, отсылается сигнал.
    void selectedColor( QColor color );
};

#endif
