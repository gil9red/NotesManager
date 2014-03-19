#include "WidgetSelectionTableSizes.h"


WidgetSelectionTableSizes::WidgetSelectionTableSizes( QWidget *parent )
    : QWidget( parent ),
      rowCount( 8 ),
      columnCount( 6 ),
      size( 20 ),
      indent( 5 ),
      selectedRow( -1 ),
      selectedColumn( -1 )
{
    int width = (indent + size ) * columnCount + indent;
    int height = (indent + size ) * rowCount + indent;

    setFixedSize( width, height );

    setMouseTracking( true );
}

void WidgetSelectionTableSizes::setRowCount( int rowCount )
{
    this->rowCount = rowCount;
}
void WidgetSelectionTableSizes::setColumnCount( int columnCount )
{
    this->columnCount = columnCount;
}
void WidgetSelectionTableSizes::setSizeCell( float size )
{
    this->size = size;
}
void WidgetSelectionTableSizes::setIndent( float indent )
{
    this->indent = indent;
}


void WidgetSelectionTableSizes::mouseMoveEvent( QMouseEvent *event )
{
    int x = event->pos().x() + indent;
    int y = event->pos().y() + indent;

    selectedColumn = x / (size + indent );
    selectedRow = y / (size + indent );

    if( selectedColumn <= 0 )
        selectedColumn = 0;

    if( selectedRow <= 0 )
        selectedRow = 0;

    selectedRow++;
    selectedColumn++;

    if( selectedColumn > columnCount )
        selectedColumn = columnCount;

    if( selectedRow > rowCount )
        selectedRow = rowCount;

    emit changeSelected( selectedRow, selectedColumn );

    update();
}
void WidgetSelectionTableSizes::paintEvent( QPaintEvent * )
{
    QPainter painter( this );

    QPen pen;

    // Отрисуем основную таблицу
    pen.setColor( Qt::darkGray );
    painter.setPen( pen );

    for( int column = 0; column < columnCount; column++ )
    {
        int x = (indent + size ) * column + indent;

        for( int row = 0; row < rowCount; row++ )
        {
            int y = (indent + size ) * row + indent;
            painter.drawRect( x, y, size, size );
        }
    }

    // Отрисуем поверх основной таблицы, таблицу выделенную мышкой
    pen.setColor( QColor( "#F4A460" ) );
    pen.setJoinStyle( Qt::RoundJoin );
    pen.setWidthF( 2.0 );

    painter.setPen( pen );

    for( int column = 0; column < selectedColumn; column++ )
    {
        int x = (indent + size ) * column + indent;

        for( int row = 0; row < selectedRow; row++ )
        {
            int y = (indent + size ) * row + indent;

            painter.save();
            painter.setBrush( Qt::white );
            float factor = 1.5;
            painter.drawRect( x - factor, y - factor, size + factor, size + factor )                        ;
            painter.restore();
        }
    }
}
void WidgetSelectionTableSizes::mouseReleaseEvent( QMouseEvent *event )
{
    if( event->button() == Qt::LeftButton )
        emit selected( selectedRow, selectedColumn );
}
void WidgetSelectionTableSizes::leaveEvent( QEvent * )
{
    selectedRow = -1;
    selectedColumn = -1;

    emit changeSelected( selectedRow, selectedColumn );

    update();
}
