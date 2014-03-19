#ifndef UCOLORBUTTON_H
#define UCOLORBUTTON_H

#include <QToolButton>
#include <QLabel>
#include <QBitmap>
#include <QMouseEvent>

#include "colorpalette.h"
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

#endif
