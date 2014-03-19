#ifndef BUTTONSELECTIONTABLESIZES_H
#define BUTTONSELECTIONTABLESIZES_H

#include <QToolButton>
#include <QLabel>

#include "WidgetSelectionTableSizes.h"

//! Кнопка с возможностью выбрать размер таблицы.
class ButtonSelectionTableSizes: public QToolButton
{
    Q_OBJECT

public:
    explicit ButtonSelectionTableSizes( QWidget * parent = 0 );

private:
    //! Отображение информации, например текущий выбранный размер таблицы.
    QLabel label;

    //! Виджет, который дает возможность курсором мыши выбрать размер таблицы.
    WidgetSelectionTableSizes widgetSelectionTableSizes;

private slots:
    //! Слот вызывается при изменении выбора размера таблицы у widgetSelectionTableSizes
    void changeSelected( int rows, int cols );

signals:
    //! Сигнал вызывается, при выборе размера таблицы
    void selected( int rows, int cols );
};

#endif // BUTTONSELECTIONTABLESIZES_H
