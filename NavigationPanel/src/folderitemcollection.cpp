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



FolderItemCollection::FolderItemCollection(Folder* owner) : QObject(owner), _owner(owner) {
	if (!owner) {
		WARNING("Null pointer recieved");
	}
}

void FolderItemCollection::Add(AbstractFolderItem* item) {
	AddTo(item, _items.size());
}

void FolderItemCollection::AddTo(AbstractFolderItem* item, int index) {
	if (!item) {
		WARNING("Null pointer recieved");
		return;
	}
	if (item == _owner) {
		WARNING("Well, this is awkward, but you tried to make an item it's own parent");
		return;
	}
	if (_items.contains(item)) {
		WARNING("Item is already in the list");
		return;
	}
	if (item->GetParent() != 0) {
		WARNING("Item already has a parent");
		return;
	}
	if (index < 0 || index > _items.size()) {
		WARNING("Index is out of bounds");
		return;
	}

	emit sg_ItemAboutToBeAdded(item, index);

	_items.insert(index, item);
	item->SetParent(_owner);

	emit sg_ItemAdded(item, index);
}

void FolderItemCollection::Remove(AbstractFolderItem* item) {
	if (!item) {
		WARNING("Null pointer recieved");
		return;
	}
	if (!_items.contains(item)) {
		WARNING("Item is not in the list");
		return;
	}
	if (item->GetParent() != this->_owner) {
		WARNING("Item is in children list, but has wrong parent");
		return;
	}

	emit sg_ItemAboutToBeRemoved(item);

	_items.removeOne(item);
	item->SetParent(0);

	emit sg_ItemRemoved(item);
}

bool FolderItemCollection::Contains(AbstractFolderItem* item) const {
	if (!item) {
		WARNING("Null pointer recieved");
		return false;
	}

	return _items.contains(item);
}

void FolderItemCollection::Clear() {
	for (int i = 0; i < _items.count(); i++) {
		_items.at(i)->SetParent(0);
	}

	emit sg_AboutToClear();
	_items.clear();
	emit sg_Cleared();
}

void FolderItemCollection::Move(int from, int to) {
	if (from < 0 || from >= _items.size()) {
		WARNING("'from' index is out of bounds");
		return;
	}
	if (to < 0 || to >= _items.size()) {
		WARNING("'to' index is out of bounds");
		return;
	}

	Move(_items.at(from), to, _owner);
}

void FolderItemCollection::Move(AbstractFolderItem* item, int to) {
	if (to < 0 || to >= _items.size()) {
		WARNING("'to' index is out of bounds");
		return;
	}

	Move(item, to, _owner);
}

void FolderItemCollection::Move(AbstractFolderItem* item, Folder* newParent) {
	if (!newParent) {
		WARNING("Null pointer recieved");
		return;
	}

	int to = newParent->Items.Count();
	if (_owner == newParent) {
		to = _items.count() - 1;
	}
	Move(item, to, newParent);
}

void FolderItemCollection::Move(AbstractFolderItem* item, int to, Folder* newParent) {
	if (!item) {
		WARNING("Null pointer recieved");
		return;
	}
	if (!_items.contains(item)) {
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

	if (_owner == newParent ) {
		if ((_items.count() == 0 && to != 0)
			||
			(_items.count() > 0 && to >= _items.count())) {
			WARNING("'to' index is out of bounds");
			return;
		}
		_items.move(_items.indexOf(item), to);
	} else {
		if (to > newParent->Items.Count()) {
			WARNING("'to' index is out of bounds");
			return;
		}
		_items.removeAll(item);
		newParent ->Items._items.insert(to, item);
		item->SetParent(newParent);
	}

	emit sg_ItemMoved(item, to, newParent);
}

int FolderItemCollection::Count() const {
	return _items.count();
}

AbstractFolderItem* FolderItemCollection::ItemAt(int index) const {
	if (index < 0 || index >= _items.count()) {
		WARNING("Index is out of bounds");
		return 0;
	}

	return _items.at(index);
}

int FolderItemCollection::IndexOf(AbstractFolderItem* const item) const {
	if (!item) {
		WARNING("Null pointer recieved");
		return -1;
	}
	if (!_items.contains(item)) {
		WARNING("The list doesn't contain specified item");
		return -1;
	}

	return _items.indexOf(item);
}
