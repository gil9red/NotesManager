#include "ButtonSelectionTableSizes.h"

#include <QMenu>
#include <QVBoxLayout>


ButtonSelectionTableSizes::ButtonSelectionTableSizes( QWidget * parent )
    : QToolButton( parent )
{    
    setPopupMode( QToolButton::InstantPopup );

    QFont font = label.font();
    font.setBold( true );
    label.setFont( font );

    QVBoxLayout * layout = new QVBoxLayout();
    layout->addWidget( &label );
    layout->addWidget( &widgetSelectionTableSizes );
    layout->addStretch();

    QMenu * menu = new QMenu();
    menu->setLayout( layout );

    setMenu( menu );

    connect( &widgetSelectionTableSizes, SIGNAL( changeSelected(int,int) ), SLOT( changeSelected( int, int ) ) );
    connect( &widgetSelectionTableSizes, SIGNAL( selected(int,int) ), SIGNAL( selected( int, int ) ) );

    changeSelected( -1, -1 );
}


void ButtonSelectionTableSizes::changeSelected( int rows, int cols )
{
    if ( rows > 0 && cols > 0 )
        label.setText( tr( "%1x%2 table" ).arg( rows ).arg( cols ) );
    else
        label.setText( tr( "Insert table" ) );
}
