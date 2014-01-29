#ifndef COLORSELECTBUTTON_H
#define COLORSELECTBUTTON_H

#include "ColorButton.h"

//! Кнопка выбора цвета.
class ColorSelectButton: public ColorButton
{
    Q_OBJECT

public:
    ColorSelectButton( QWidget *parent = 0 );

private:
    void createGUI(); //!< Создание внешнего вида.
};


#endif // COLORSELECTBUTTON_H
