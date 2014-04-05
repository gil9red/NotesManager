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

#include "folder.h"

#include "note.h"
#include "utils/func.h"

#include <QDebug>

Folder::Folder(QString _name, FolderType _type) :
        AbstractFolderItem(AbstractFolderItem::Type_Folder, _name),
		type(_type),
		expanded(false),
        child(this)
{
    QObject::connect(&child, SIGNAL(sg_ItemAdded(AbstractFolderItem* const, int)), SIGNAL(sg_ItemAdded(AbstractFolderItem* const, int)));
    QObject::connect(&child, SIGNAL(sg_ItemMoved(AbstractFolderItem* const,int, Folder*)), SIGNAL(sg_ItemMoved(AbstractFolderItem* const,int, Folder*)));
    QObject::connect(&child, SIGNAL(sg_ItemRemoved(AbstractFolderItem* const)), SIGNAL(sg_ItemRemoved(AbstractFolderItem* const)));
    QObject::connect(&child, SIGNAL(sg_Cleared()), SIGNAL(sg_ItemsCollectionCleared()));
    QObject::connect(&child, SIGNAL(sg_ItemAboutToBeAdded(AbstractFolderItem*const, int)), SIGNAL(sg_ItemAboutToBeAdded(AbstractFolderItem*const, int)));
    QObject::connect(&child, SIGNAL(sg_ItemAboutToBeMoved(AbstractFolderItem*const,int, Folder*)), SIGNAL(sg_ItemAboutToBeMoved(AbstractFolderItem*const,int, Folder*)));
    QObject::connect(&child, SIGNAL(sg_ItemAboutToBeRemoved(AbstractFolderItem*const)), SIGNAL(sg_ItemAboutToBeRemoved(AbstractFolderItem*const)));
    QObject::connect(&child, SIGNAL(sg_AboutToClear()), SIGNAL(sg_ItemsCollectionAboutToClear()));
    QObject::connect(&child, SIGNAL(sg_ItemAdded(AbstractFolderItem*const,int)), SIGNAL(sg_DataChanged()));
    QObject::connect(&child, SIGNAL(sg_ItemMoved(AbstractFolderItem*const,int,Folder*)), SIGNAL(sg_DataChanged()));
    QObject::connect(&child, SIGNAL(sg_ItemRemoved(AbstractFolderItem*const)), SIGNAL(sg_DataChanged()));
    QObject::connect(&child, SIGNAL(sg_Cleared()), SIGNAL(sg_DataChanged()));


    if (type == TrashFolder)
    {
        name = tr( "Trash bin" );
        setIcon( QIcon( ":/fugue-icons/bin" ) );

    }else if ( type == UserFolder )
    {
        if (name.isEmpty())
            name = tr( "New folder" );
        setIcon( QIcon( ":/fugue-icons/folder-horizontal" ) );
    }
}

Folder::~Folder()
{
    for (int i = child.Count() - 1; i >= 0 ; i--)
    {
        AbstractFolderItem* item = child.ItemAt(i);
        child.Remove(item);
		delete item;
	}
}

Folder::FolderType Folder::getType() const
{
	return type;
}

void Folder::setType(Folder::FolderType _type)
{
	type = _type;
}

bool Folder::isExpanded() const
{
	return expanded;
}

void Folder::setExpanded(bool value)
{
	expanded = value;
}

QString Folder::getPath() const
{
	QString path = name;

    Folder* f = getParent();
    while(f != 0 && f->getParent() != 0)
    {
		path.prepend(f->name + "/");
        f = f->getParent();
	}

	return path;
}
