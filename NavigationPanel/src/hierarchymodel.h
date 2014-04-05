/*
This file is part of qNotesManager.

qNotesManager is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

qNotesManager is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with qNotesManager. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QNM_HIERARCHYMODEL_H
#define QNM_HIERARCHYMODEL_H

#include <QHash>
#include <QMimeData>

#include "basemodel.h"

class Folder;
class Note;
class AbstractFolderItem;

class HierarchyModel : public BaseModel
{
    Q_OBJECT

public:
    explicit HierarchyModel( QObject * parent = 0 );

    virtual Qt::DropActions supportedDropActions () const;
    virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;

    virtual bool dropMimeData ( const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent );
    virtual QMimeData* mimeData ( const QModelIndexList & indexes ) const;
    virtual QStringList mimeTypes () const;

    void setPinnedFolder(Folder*);
    Folder* getPinnedFolder() const;

    BaseModelItem * getModelItem( AbstractFolderItem * item ) { return bridge.value( item, 0 ); }
    AbstractFolderItem * getItem( BaseModelItem * modelItem ) { return bridge.key( modelItem, 0 ); }

private:
    QHash <AbstractFolderItem*, BaseModelItem*> bridge;

    void registerItem(Folder* folder);
    void registerItem(Note* note);

    void unregisterItem(Folder* folder);
    void unregisterItem(Note* note);

private slots:
    void sl_Folder_ItemAdded(AbstractFolderItem* const, int);
    void sl_Folder_ItemAboutToBeRemoved(AbstractFolderItem* const);
    void sl_Folder_ItemAboutToBeMoved(AbstractFolderItem* const, int, Folder*);
    void sl_Folder_ItemsCollectionCleared();

    void sl_Item_DataChanged(BaseModelItem*);

};


#endif // QNM_HIERARCHYMODEL_H
