#ifndef COLORTEXTBUTTON_H
#define COLORTEXTBUTTON_H

#include "ColorButton.h"

//! Кнопка выбора цвета текста.
class ColorTextButton: public ColorButton
{
    Q_OBJECT

public:
    ColorTextButton( QWidget *parent = 0 );

private:
    void createGUI(); //!< Создание внешнего вида.

public slots:
    //! Выбор текущего цвета.
    void setColor( const QColor & color );
};

#endif // COLORTEXTBUTTON_H
