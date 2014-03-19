#include "ButtonList.h"

ButtonList::ButtonList( QWidget * parent)
    : QToolButton( parent )
{
    connect( &buttonGroup, SIGNAL( buttonClicked(int) ), SIGNAL( selected(int) ) );
    connect( this, SIGNAL( clicked() ), SLOT( clickDefault() ) );
}
void ButtonList::addButton( QAbstractButton * button, int id )
{
    buttonGroup.addButton( button, id );
}
void ButtonList::setDefaultId( int id )
{
    d_defaultId = id;
}
int ButtonList::defaultId()
{
    return d_defaultId;
}

QAbstractButton * ButtonList::checkedButton(int id )
{
    QAbstractButton * button = buttonGroup.button( id );
    if ( button )
        button->setChecked( true );

    return button;
}

void ButtonList::clickDefault()
{
    emit selected( d_defaultId );
}
