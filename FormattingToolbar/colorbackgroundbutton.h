#ifndef COLORBACKGROUNDBUTTON_H
#define COLORBACKGROUNDBUTTON_H

#include "ColorButton.h"

//! Кнопка выбора цвета заднего фона.
class ColorBackgroundButton: public ColorButton
{
    Q_OBJECT

public:
    ColorBackgroundButton( QWidget *parent = 0 );

private:
    void createGUI(); //!< Создание внешнего вида.

public slots:
    //! Выбор текущего цвета.
    void setColor( const QColor & color );

private slots:
    //! Вызов цвета по умолчанию.
    void defaultColor();

signals:
    //! Вызов сигнала, символизирующий о том, что нужно очистить задний фон.
    void clearBackground();
};

#endif // COLORBACKGROUNDBUTTON_H
