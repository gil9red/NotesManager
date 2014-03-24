#include "basemodelitem.h"

QString stringType( BaseModelItem * item )
{
    if ( item->isBase() )
        return "Base";

    else if ( item->isNote() )
        return "Note";

    else if ( item->isFolder() )
        return "Folder";

    else if ( item->isTrash() )
        return "Trash";

    return QString();
}

BaseModelItem::BaseModelItem( const QString & text )
    : QStandardItem( text )
{
    setType( BASE );
    setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );

    setBackground( Qt::white );
    setForeground( Qt::black );
}

bool BaseModelItem::isEmpty()
{
    return ( rowCount() == 0 );
}
bool BaseModelItem::isBase()
{
    return ( type() == BASE );
}
bool BaseModelItem::isNote()
{
    return ( type() == NOTE );
}
bool BaseModelItem::isFolder()
{
    return ( type() == FOLDER );
}
bool BaseModelItem::isTrash()
{
    return ( type() == TRASH );
}

BaseModelItem::Type BaseModelItem::type()
{
    return ( BaseModelItem::Type ) data( ROLE_TYPE ).toInt();
}
void BaseModelItem::setType( Type t )
{
    setData( ( int ) t, ROLE_TYPE );
}
