#include "trashmodelitem.h"

TrashModelItem::TrashModelItem() : BaseModelItem()
{
    setType( BaseModelItem::TRASH );

    QFont font( BaseModelItem().font() );
    font.setBold( true );

    setIcon( QIcon( ":/NavigationPanel/trash" ) );
    setText( QObject::tr( "Корзина" ) );
    setDragEnabled( false );
    setDropEnabled( true );
    setEditable( false );
    setFont( font );
}
