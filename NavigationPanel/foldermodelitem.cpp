#include "foldermodelitem.h"

FolderModelItem::FolderModelItem( const QString & text )
    : BaseModelItem( text )
{
    setIcon( QIcon( ":/NavigationPanel/folder" ) );
    setType( FOLDER );

    setDropEnabled( true );
    setDragEnabled( true );
    setEditable( true );
}
