#include "colorbackgroundbutton.h"

#include <QDebug>
#include <QPainter>


ColorBackgroundButton::ColorBackgroundButton( QWidget * parent )
    : ColorButton( parent )
{
    createGUI();
}
void ColorBackgroundButton::createGUI()
{
    setIcon( QIcon( ":/color-background" ) );
    setDefaultColor( Qt::transparent );

    ColorButton::disconnect( this, SIGNAL( clicked() ), this, SLOT( defaultColor() ) );
    ColorButton::disconnect( d->tButtonDefaultColor, SIGNAL( clicked() ), this, SLOT( defaultColor() ) );

    connect( this, SIGNAL( clicked() ), SLOT( defaultColor() ) );
    connect( d->tButtonDefaultColor, SIGNAL( clicked() ), SLOT( defaultColor() ) );
}
void ColorBackgroundButton::setColor( const QColor & color )
{
    d->color = color;

    QPixmap pixmap( ":/color-background" );

    QPainter painter( &pixmap );
    painter.setBrush( color );
    painter.setPen( color );
    painter.drawRect( 0, pixmap.height() - 25, pixmap.width(), pixmap.height() - 25 );

    setIcon( pixmap );
}
void ColorBackgroundButton::defaultColor()
{
    emit clearBackground();
}
