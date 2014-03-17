#include "foldermodelitem.h"

FolderModelItem::FolderModelItem( const QString & text )
    : BaseModelItem( text )
{
    setIcon( QIcon( ":/folder" ) );
    setType( FOLDER );

    setDropEnabled( true );
    setDragEnabled( true );
    setEditable( true );
}
