#include "abstractnotehead.h"

#include <QVBoxLayout>
#include <QPainter>

AbstractNoteHead::AbstractNoteHead( QWidget * parent )
    : QWidget( parent ),
      d( new d_AbstractHead ),
      label( new QLabel )
{
    d->isMousePress = false;

    setFixedHeight( Note::topBorder );

    QVBoxLayout * mainLayout = new QVBoxLayout();
    mainLayout->setSpacing( 0 );
    mainLayout->setContentsMargins( 5, 5, 5, 5 );
    setLayout( mainLayout );

    mainLayout->addWidget( label, 0, Qt::AlignCenter );
}
AbstractNoteHead::~AbstractNoteHead()
{
    delete d;
}

void AbstractNoteHead::setTitle( const QString & str )
{
    label->setText( str );
}
QString AbstractNoteHead::title()
{
    return label->text();
}

void AbstractNoteHead::setFont( const QFont & f )
{
    label->setFont( f );
}
QFont AbstractNoteHead::font()
{
    return label->font();
}

void AbstractNoteHead::setColor( const QColor & color )
{
    d->color = color;
}
QColor AbstractNoteHead::color()
{
    return d->color;
}

void AbstractNoteHead::paintEvent( QPaintEvent * )
{
    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing );

    painter.setBrush( d->color );
    painter.setPen( d->color );
    painter.drawRect( rect() );
}
void AbstractNoteHead::mouseDoubleClickEvent(QMouseEvent *)
{
    emit doubleClick();
}
void AbstractNoteHead::mousePressEvent( QMouseEvent * event )
{
    if ( event->button() == Qt::LeftButton )
    {
        d->oldPosition = event->pos();
        d->isMousePress = true;
    }
}
void AbstractNoteHead::mouseReleaseEvent( QMouseEvent * )
{
    d->isMousePress = false;
}
void AbstractNoteHead::mouseMoveEvent( QMouseEvent * event )
{
    if ( d->isMousePress )
    {
        QPoint delta = event->pos() - d->oldPosition;       
        if ( !parentWidget()->parentWidget() )
            move( pos() + delta );
        else
            parentWidget()->parentWidget()->move( mapToGlobal( pos() ) + delta );
    }
}
