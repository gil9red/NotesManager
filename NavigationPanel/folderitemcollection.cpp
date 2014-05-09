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

#include "folderitemcollection.h"

#include "abstractfolderitem.h"
#include "folder.h"
#include "utils/func.h"

FolderItemCollection::FolderItemCollection(Folder* parent) :
    QObject(parent), owner(parent) {
	if (!owner) {
		WARNING("Null pointer recieved");
	}
}

void FolderItemCollection::Add(AbstractFolderItem* item) {
    AddTo(item, items.size());
}

void FolderItemCollection::AddTo(AbstractFolderItem* item, int index) {
	if (!item) {
		WARNING("Null pointer recieved");
		return;
	}
    if (item == owner) {
		WARNING("Well, this is awkward, but you tried to make an item it's own parent");
		return;
	}
    if (items.contains(item)) {
		WARNING("Item is already in the list");
		return;
	}
    if (item->getParent() != 0) {
		WARNING("Item already has a parent");
		return;
	}
    if (index < 0 || index > items.size()) {
		WARNING("Index is out of bounds");
		return;
	}

	emit sg_ItemAboutToBeAdded(item, index);

    items.insert(index, item);
    item->setParent(owner);

	emit sg_ItemAdded(item, index);
}

void FolderItemCollection::Remove(AbstractFolderItem* item) {
	if (!item) {
		WARNING("Null pointer recieved");
		return;
	}
    if (!items.contains(item)) {
		WARNING("Item is not in the list");
		return;
	}
    if (item->getParent() != this->owner) {
		WARNING("Item is in children list, but has wrong parent");
		return;
	}

	emit sg_ItemAboutToBeRemoved(item);

    items.removeOne(item);
    item->setParent(0);

	emit sg_ItemRemoved(item);
}

bool FolderItemCollection::Contains(AbstractFolderItem* item) const {
	if (!item) {
		WARNING("Null pointer recieved");
		return false;
	}

    return items.contains(item);
}

void FolderItemCollection::Clear() {
    for (int i = 0; i < items.count(); i++) {
        items.at(i)->setParent(0);
	}

	emit sg_AboutToClear();
    items.clear();
	emit sg_Cleared();
}

void FolderItemCollection::Move(int from, int to) {
    if (from < 0 || from >= items.size()) {
		WARNING("'from' index is out of bounds");
		return;
	}
    if (to < 0 || to >= items.size()) {
		WARNING("'to' index is out of bounds");
		return;
	}

    Move(items.at(from), to, owner);
}

void FolderItemCollection::Move(AbstractFolderItem* item, int to) {
    if (to < 0 || to >= items.size()) {
		WARNING("'to' index is out of bounds");
		return;
	}

    Move(item, to, owner);
}

void FolderItemCollection::Move(AbstractFolderItem* item, Folder* newParent) {
	if (!newParent) {
		WARNING("Null pointer recieved");
		return;
	}

    int to = newParent->child.Count();
    if (owner == newParent) {
        to = items.count() - 1;
	}
	Move(item, to, newParent);
}

void FolderItemCollection::Move(AbstractFolderItem* item, int to, Folder* newParent) {
	if (!item) {
		WARNING("Null pointer recieved");
		return;
	}
    if (!items.contains(item)) {
		WARNING("The list doesn't contain specified item");
		return;
	}
	if (to < 0) {
		WARNING("'to' index is out of bounds");
		return;
	}
	if (!newParent) {
		WARNING("Null pointer recieved");
		return;
	}

	emit sg_ItemAboutToBeMoved(item, to, newParent);

    if (owner == newParent ) {
        if ((items.count() == 0 && to != 0)
			||
            (items.count() > 0 && to >= items.count())) {
			WARNING("'to' index is out of bounds");
			return;
		}
        items.move(items.indexOf(item), to);
	} else {
        if (to > newParent->child.Count()) {
			WARNING("'to' index is out of bounds");
			return;
		}
        items.removeAll(item);
        newParent->child.items.insert(to, item);
        item->setParent(newParent);
	}

	emit sg_ItemMoved(item, to, newParent);
}

int FolderItemCollection::Count() const {
    return items.count();
}

AbstractFolderItem* FolderItemCollection::ItemAt(int index) const {
    if (index < 0 || index >= items.count()) {
		WARNING("Index is out of bounds");
		return 0;
	}

    return items.at(index);
}

int FolderItemCollection::IndexOf(AbstractFolderItem* const item) const {
	if (!item) {
		WARNING("Null pointer recieved");
		return -1;
	}
    if (!items.contains(item)) {
		WARNING("The list doesn't contain specified item");
		return -1;
	}

    return items.indexOf(item);
}
