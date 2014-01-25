#include "colorpalette.h"

#include <QPixmap>
#include <QBitmap>

ColorPalette::ColorPalette( const QString & path, QWidget * parent )
    : QLabel( parent )
{
    setAutoFillBackground( true );
    setMouseTracking( true );

    setImage( path );
}
ColorPalette::ColorPalette( QWidget * parent )
    : QLabel( parent )
{
    setAutoFillBackground(true);
    setMouseTracking(true);
}

void ColorPalette::setImage( const QString & path )
{
    QPixmap pixmap( path );
    setMask( pixmap.mask() );

    QPalette palette;
    palette.setBrush( backgroundRole(), QBrush( pixmap ) );

    setPalette( palette );
    setFixedSize( pixmap.size() );
}


void ColorPalette::mousePressEvent( QMouseEvent * event )
{
    QRect onePixRect( event->pos(), QSize( 1, 1 ));

    QPixmap pix = QPixmap::grabWidget( this, onePixRect );
    QImage img = pix.toImage();

    emit selectedColor( QColor( img.pixel( 0, 0 ) ) );
}
