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

#ifndef QNM_FOLDER_H
#define QNM_FOLDER_H

#include "abstractfolderitem.h"
#include "folderitemcollection.h"

#include <QDateTime>
#include <QIcon>

/*
  Folder is item, that may contain notes and another folders
*/

class Folder : public AbstractFolderItem
{

    Q_OBJECT
public:
    enum FolderType
    {
        UserFolder = 0,
        SystemFolder = 1,
        TrashFolder = 5
    };

public:
    explicit Folder(QString _name = QString(), FolderType _type = UserFolder);
    ~Folder();

    FolderType getType() const;
    void setType(FolderType _type);

    bool isExpanded() const;
    void setExpanded(bool);

    QString getPath() const;

    FolderItemCollection child;

private:
    FolderType type;
    bool expanded;

signals:
    void sg_ItemAboutToBeAdded(AbstractFolderItem* const, int);
    void sg_ItemAdded(AbstractFolderItem* const, int);
    void sg_ItemAboutToBeRemoved(AbstractFolderItem* const);
    void sg_ItemRemoved(AbstractFolderItem* const);
    void sg_ItemAboutToBeMoved(AbstractFolderItem* const, int, Folder*);
    void sg_ItemMoved(AbstractFolderItem* const, int, Folder*);
    void sg_ItemsCollectionAboutToClear();
    void sg_ItemsCollectionCleared();
};

#endif // QNM_FOLDER_H
