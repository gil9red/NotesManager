#include "colorselectbutton.h"

#include <QDebug>
#include <QPainter>

ColorSelectButton::ColorSelectButton( QWidget * parent )
    : ColorButton( parent )
{
    createGUI();
}
void ColorSelectButton::createGUI()
{
    setPopupMode( QToolButton::InstantPopup );
    connect( this, SIGNAL( selectedColor(QColor) ), SLOT( setColor(QColor) ) );
    disconnect( this, SIGNAL( clicked() ), this, SLOT( defaultColor() ) );
    d->tButtonDefaultColor->deleteLater();
}
