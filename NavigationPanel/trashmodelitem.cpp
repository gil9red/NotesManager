#include "trashmodelitem.h"

TrashModelItem::TrashModelItem() : BaseModelItem()
{
    setType( BaseModelItem::TRASH );

    QFont font( BaseModelItem().font() );
    font.setBold( true );

    setIcon( QIcon( ":/NavigationPanel/trash" ) );
    setText( QObject::tr( "Trash" ) );
    setDragEnabled( false );
    setDropEnabled( true );
    setEditable( false );
    setFont( font );
}
