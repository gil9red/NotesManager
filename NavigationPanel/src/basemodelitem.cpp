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

#include "basemodelitem.h"

#include "basemodel.h"
#include "utils/func.h"

#include <QDebug>

BaseModelItem::BaseModelItem(ItemType type)
	: itemType(type)
{
	parentItem = 0;
	sorted = false;
	sortOrder = Qt::AscendingOrder;
}

BaseModelItem::~BaseModelItem() {
}

// Return item parent or 0 if there is none.
BaseModelItem* BaseModelItem::parent() const {
	return parentItem;
}

// Returns item flags
Qt::ItemFlags BaseModelItem::flags () const {
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

// Adds item 'item' into the end children list.
void BaseModelItem::AddChild(BaseModelItem* item) {
	AddChildTo(item, childrenList.size());
}

// Adds item 'item' into children list in specified position.
// NOTE: 'position' parameter is ignored when sorting is enabled
void BaseModelItem::AddChildTo(BaseModelItem* item, int position) {
	if (!item) {
		WARNING("Null pointer recieved");
		return;
	}
	if (item->parentItem != 0) {
		WARNING("Item already has a parent");
		return;
	}
	if (childrenList.contains(item)) {
		WARNING("Item is already in the list");
		return;
	}
	if (position < 0 || position > childrenList.size()) {
		WARNING("Wrong position");
		return;
	}

	childrenList.insert(position, item);
	item->parentItem = this;
	item->setParent(this); // QObject parentship
	insertIndexCache.Clear();
}

// Removes child item
void BaseModelItem::RemoveChild(BaseModelItem* item) {
	if (!item) {
		WARNING("Null pointer recieved");
		return;
	}
	if (!childrenList.contains(item)) {
		WARNING("Item is not in the list");
		return;
	}

	childrenList.removeAll(item);
	item->parentItem = 0;
	item->setParent(0); // QObject parentship
	insertIndexCache.Clear();
}

// Returns index of child item.
int BaseModelItem::IndexOfChild(BaseModelItem* item) const {
	if (!item) {
		WARNING("Null pointer recieved");
		return -1;
	}
	if (!childrenList.contains(item)) {
		WARNING("Item is not in the list");
		return -1;
	}

	return childrenList.indexOf(item);
}

// Returns children count
int BaseModelItem::ChildrenCount() const {
	return childrenList.count();
}

// Returns child item at position 'index'.
BaseModelItem* BaseModelItem::ChildAt(int index) const {
	if (index < 0 || index >= childrenList.count()) {
		WARNING("Item at wrong index requested");
		return 0;
	}

	return childrenList.at(index);
}

void BaseModelItem::ClearChildrenList() {
	childrenList.clear();
	insertIndexCache.Clear();
}

// Returns true if object is a child or a grandchild of item 'parent', otherwise returns false.
bool BaseModelItem::IsOffspringOf(const BaseModelItem* parent) const {
	if (!parent) {
		WARNING("Null pointer recieved");
		return false;
	}

	if (parent == this) {return false;}

	const BaseModelItem* f = this->parent();
	while (f != 0) {
		if (f == parent) {return true;}
		f = f->parent();
	}
	return false;
}

BaseModelItem::ItemType BaseModelItem::DataType() const {
	return itemType;
}

QVariant BaseModelItem::data(int role) const {
	return QVariant();
}

bool BaseModelItem::setData(const QVariant&, int) {
	return false;
}

// Returns index where a new item will be inserted with AddChild()
int BaseModelItem::FindInsertIndex(const BaseModelItem* item) const {
	if (!item) {
		WARNING("Null pointer recieved");
		return -1;
	}

	int newIndex = -1;

    if (sorted)
		newIndex = findInsertIndex_Sorted(item);
    else
        newIndex = findInsertIndex_Simple(item);

	return newIndex;
}

// Compares internal item data for sorting purpose. Returns true by default.
bool BaseModelItem::LessThan(const BaseModelItem*) const {
	return true;
}

// Returns index for a new item when list is sorted
int BaseModelItem::findInsertIndex_Sorted(const BaseModelItem* item) const {
	// TODO: add anchors handling
	// TODO: handle Qt::DescendingOrder
	if (!item) {
		WARNING("Null pointer recieved");
		return -1;
	}

	const int size = ChildrenCount();

	if (size == 0) {return 0;}
	if (item->LessThan(ChildAt(0))) {return 0;}
	if (ChildAt(size - 1)->LessThan(item)) {return size;}

	int index = 0;
	int left = 1;
	int right = size - 1;

	while (true) {
		index = (left + right) / 2;

		if (index == 0 || index == size) {
			break;
		}

		BaseModelItem* leftItem = ChildAt(index - 1);
		BaseModelItem* rightItem = ChildAt(index);

		if (leftItem->LessThan(item) && item->LessThan(rightItem)) {
			break;
		} else if (rightItem->LessThan(item)) {
			left = index + 1;
		} else if (item->LessThan(leftItem)) {
			right = index - 1;
		} else {
			break;
		}
	}

	return index;
}

// Returns index for a new item when list is not sorted
int BaseModelItem::findInsertIndex_Simple(const BaseModelItem* item) const {
	// TODO: add anchors handling
	return ChildrenCount();
}

// Returns true if children list is sorted, otherwise returns false
bool BaseModelItem::IsSorted() const {
	return sorted;
}

// Sets children list sorting
// NOTE: Sorting may be changed only when children list is empty
void BaseModelItem::SetSorted(const bool s) {
	if (ChildrenCount() == 0) {
		sorted = s;
	}
}

// Returns children list sorting order
Qt::SortOrder BaseModelItem::GetSortOrder() const {
	return sortOrder;
}

// Sets children list sorting order
// NOTE: Sort order may be changed only when children list is empty
void BaseModelItem::SetSortOrder(const Qt::SortOrder order) {
	if (ChildrenCount() == 0) {
		sortOrder = order;
	}
}
