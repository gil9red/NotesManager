#ifndef FOLDERMODELITEM_H
#define FOLDERMODELITEM_H

#include "basemodelitem.h"

class FolderModelItem: public BaseModelItem
{
public:
    explicit FolderModelItem( const QString & text = QString() );
};

#endif // FOLDERMODELITEM_H
