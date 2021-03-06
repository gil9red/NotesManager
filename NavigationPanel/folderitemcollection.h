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

#ifndef QNM_FOLDERITEMCOLLECTION_H
#define QNM_FOLDERITEMCOLLECTION_H

#include <QObject>
#include <QList>

/*
FolderItemCollection is a collection of AbstractFolderItem items owned by a folder
*/

class AbstractFolderItem;
class Folder;

class FolderItemCollection : public QObject
{
    Q_OBJECT

public:
    explicit FolderItemCollection(Folder* parent);

private:
    QList<AbstractFolderItem*> items;
    Folder* const owner;

public:
    void add(AbstractFolderItem* item);
    void addTo(AbstractFolderItem* item, int index);
    void remove(AbstractFolderItem* item);
    bool contains(AbstractFolderItem* item) const;
    void clear(); // clears collection, DOES NOT delete items
    void move(int from, int to);
    void move(AbstractFolderItem* item, int to);
    void move(AbstractFolderItem* item, int to, Folder* newParent);
    void move(AbstractFolderItem* item, Folder* newParent);
    int count() const;
    AbstractFolderItem* itemAt(int index) const;
    int indexOf(AbstractFolderItem* const) const;

signals:
    void sg_ItemAboutToBeAdded(AbstractFolderItem* const, int to);
    void sg_ItemAdded(AbstractFolderItem* const, int to);
    void sg_ItemAboutToBeRemoved(AbstractFolderItem* const);
    void sg_ItemRemoved(AbstractFolderItem* const);
    void sg_ItemAboutToBeMoved(AbstractFolderItem* const, int newPosition, Folder*);
    void sg_ItemMoved(AbstractFolderItem* const, int newPosition, Folder*);
    void sg_AboutToClear();
    void sg_Cleared();
};


#endif // QNM_FOLDERITEMCOLLECTION_H
