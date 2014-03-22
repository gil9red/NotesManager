#include "basemodelitem.h"

QString stringType( BaseModelItem * item )
{
    QString name;
    if ( item->isBase() )
        name = "Base";
    else if ( item->isNote() )
        name = "Note";
    else if ( item->isFolder() )
        name = "Folder";
    else if ( item->isTrash() )
        name = "Trash";
    return name;
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
