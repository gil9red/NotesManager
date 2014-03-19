#ifndef WIDGETSELECTROWSCOLUMNSTABLE_H
#define WIDGETSELECTROWSCOLUMNSTABLE_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>

//! Виджет графического выбора размера таблицы.
class WidgetSelectionTableSizes: public QWidget
{
    Q_OBJECT

public:
    WidgetSelectionTableSizes( QWidget *parent = 0 );

    void setRowCount( int rowCount );       //!< Установка количества строк.
    void setColumnCount( int columnCount ); //!< Установка количества столбцов.
    void setSizeCell( float size );         //!< Установка размера ячейки таблицы.
    void setIndent( float indent );         //!< Установка отступа между ячейками.

private:
    int rowCount;       //!< Количество строк.
    int columnCount;    //!< Количество столбцов.
    float size;         //!< Установка количества строк.
    float indent;       //!< Отступ между ячейками.
    int selectedRow;    //!< Индекс выбранной строки.
    int selectedColumn; //!< Индекс выбранного столбца.

signals:
    //! Вызывается при изменении размера таблицы.
    void changeSelected( int row, int column );

    //! Вызывается при выборе размера таблицы.
    void selected( int row, int column );

protected:
    void mouseMoveEvent( QMouseEvent *event );
    void paintEvent( QPaintEvent * );
    void mouseReleaseEvent( QMouseEvent *event );
    void leaveEvent( QEvent * );
};

#endif
