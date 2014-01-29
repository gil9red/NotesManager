#include "colortextbutton.h"

#include <QDebug>
#include <QPainter>

ColorTextButton::ColorTextButton( QWidget * parent )
    : ColorButton( parent )
{
    createGUI();
}
void ColorTextButton::createGUI()
{
    setIcon( QIcon( ":/color-text" ) );
    setDefaultColor( Qt::black );
}
void ColorTextButton::setColor( const QColor & color )
{
    d->color = color;

    QPixmap pixmap( ":/color-text" );

    QPainter painter( &pixmap );
    painter.setBrush( color );
    painter.setPen( color );
    painter.drawRect( 0, pixmap.height() - 25, pixmap.width(), pixmap.height() - 25 );

    setIcon( pixmap );
}
