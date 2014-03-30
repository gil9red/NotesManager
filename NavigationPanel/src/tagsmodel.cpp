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

#include "tagsmodel.h"

#include "tag.h"
#include "note.h"
#include "tagmodelitem.h"
#include "notemodelitem.h"
#include "notebook.h"
#include "utils/func.h"

#include <QList>



TagsModel::TagsModel(QObject *parent)
    : BaseModel(parent)
{
	BaseModelItem* root = new BaseModelItem();
	root->SetSorted(true);
	SetRootItem(root);

    QObject::connect(Notebook::instance(), SIGNAL(sg_ItemRegistered(Tag*)), SLOT(sl_Document_TagRegistered(Tag*)));
    QObject::connect(Notebook::instance(), SIGNAL(sg_ItemUnregistered(Tag*)), SLOT(sl_Document_TagUnregistered(Tag*)));

    const QList<Tag*> tags = Notebook::instance()->tagsList();
    foreach (Tag* tag, tags)
		sl_Document_TagRegistered(tag);	
}

void TagsModel::sl_Tag_OwnerAdded(Note* note)
{
	Tag* tag = qobject_cast<Tag*>(QObject::sender());

	if (!tag) {
		WARNING("Null pointer recieved");
		return;
	}
	if (!tagsBridge.contains(tag)) {
		WARNING("Got signal from unknown item");
		return;
	}

	BaseModelItem* tagItem = tagsBridge.value(tag);

	NoteModelItem* noteItem = new NoteModelItem(note);
	QObject::connect(noteItem, SIGNAL(sg_DataChanged(BaseModelItem*)),
					 this, SLOT(sl_Item_DataChanged(BaseModelItem*)));

	QModelIndex tagIndex = createIndex(GetRootItem()->IndexOfChild(tagItem), 0, tagItem);

	int newPosition = tagItem->FindInsertIndex(noteItem);
	beginInsertRows(tagIndex, newPosition, newPosition);
		tagItem->AddChild(noteItem);
	endInsertRows();

	notesBridge.insert(note, noteItem);
}

void TagsModel::sl_Tag_OwnerRemoved(Note* note) {
	Tag* tag = qobject_cast<Tag*>(QObject::sender());

	if (!tag) {
		WARNING("Null pointer recieved");
		return;
	}
	if (!tagsBridge.contains(tag)) {
		WARNING("Got signal from unknown item");
		return;
	}

	BaseModelItem* tagItem = tagsBridge.value(tag);

	// Searching for note item, that is a child of tagItem
	BaseModelItem* noteItem = 0;
	QList<BaseModelItem*> noteItems = notesBridge.values(note);
	foreach (BaseModelItem* i, noteItems) {
		if (i->parent() == tagItem) {
			noteItem = i;
			break;
		}
	}

	if (!noteItem) {
		WARNING("Corresponding item not found");
		return;
	}

	BaseModelItem* root = GetRootItem();

	QModelIndex tagIndex = createIndex(root->IndexOfChild(tagItem), 0, tagItem);

	beginRemoveRows(tagIndex, tagItem->IndexOfChild(noteItem), tagItem->IndexOfChild(noteItem));
		tagItem->RemoveChild(noteItem);
	endRemoveRows();

	notesBridge.remove(note, noteItem);

	delete noteItem;
}

void TagsModel::sl_Tag_OwnersRemoved() {
	Tag* tag = qobject_cast<Tag*>(QObject::sender());

	if (!tag) {
		WARNING("Null pointer recieved");
		return;
	}
	if (!tagsBridge.contains(tag)) {
		WARNING("Got signal from unknown item");
		return;
	}

	BaseModelItem* tagItem = tagsBridge.value(tag);


	BaseModelItem* root = GetRootItem();
	QModelIndex tagIndex = createIndex(root->IndexOfChild(tagItem), 0, tagItem);

	beginRemoveRows(tagIndex, 0, tagItem->ChildrenCount());
		BaseModelItem* childItem = 0;
		for (int i = 0; i < tagItem->ChildrenCount(); i++) {
			childItem = tagItem->ChildAt(i);
			if (childItem->DataType() != BaseModelItem::note) {
				WARNING("Item has wrong type");
				continue;
			}
			NoteModelItem* noteItem = dynamic_cast<NoteModelItem*>(childItem);
			Note* note = noteItem->GetStoredData();
			notesBridge.remove(note, noteItem);
			delete noteItem;
		}
		tagItem->ClearChildrenList();
	endRemoveRows();
}

void TagsModel::sl_Document_TagRegistered(Tag* tag) {
	if (!tag) {
		WARNING("Null pointer recieved");
		return;
	}
	if (tagsBridge.contains(tag)) {
		WARNING("Item is already registered");
		return;
	}

	QObject::connect(tag, SIGNAL(sg_OwnerAdded(Note*)),
					 this, SLOT(sl_Tag_OwnerAdded(Note*)));
	QObject::connect(tag, SIGNAL(sg_OwnerRemoved(Note*)),
					 this, SLOT(sl_Tag_OwnerRemoved(Note*)));
	QObject::connect(tag, SIGNAL(sg_OwnersRemoved()),
					 this, SLOT(sl_Tag_OwnersRemoved()));
	TagModelItem* item = new TagModelItem(tag);
	item->SetSorted(true);
	tagsBridge.insert(tag, item);

	int newPosition = 0;

	for (int i = 0; i < tag->Owners.Count(); i++) {
		NoteModelItem* noteItem = new NoteModelItem(tag->Owners.ItemAt(i));
		QObject::connect(noteItem, SIGNAL(sg_DataChanged(BaseModelItem*)),
						 this, SLOT(sl_Item_DataChanged(BaseModelItem*)));
		newPosition = item->FindInsertIndex(noteItem);
		item->AddChildTo(noteItem, newPosition);
		notesBridge.insert(tag->Owners.ItemAt(i), noteItem);
	}

	BaseModelItem* root = GetRootItem();

	newPosition = root->FindInsertIndex(item);//FindPositionForElement(root, item);

	beginInsertRows(QModelIndex(), newPosition, newPosition);
		root->AddChildTo(item, newPosition);
	endInsertRows();
}

void TagsModel::sl_Document_TagUnregistered(Tag* tag) {
	if (!tag) {
		WARNING("Null pointer recieved");
		return;
	}
	if (!tagsBridge.contains(tag)) {
		WARNING("Item is not registered");
		return;
	}
	if (tag->Owners.Count() > 0) {
		WARNING("Tag has owners");
	}

	QObject::disconnect(tag, 0, this, 0);

	TagModelItem* item = tagsBridge.value(tag);
	tagsBridge.remove(tag);

	BaseModelItem* root = GetRootItem();

	beginRemoveRows(QModelIndex(), root->IndexOfChild(item), root->IndexOfChild(item));
		root->RemoveChild(item);
	endRemoveRows();

	delete item;
}

void TagsModel::sl_Item_DataChanged(BaseModelItem* item) {
	const BaseModelItem* parentItem = item->parent();
	if (!parentItem) {
		WARNING("Could not find item parent");
		return;
	}
	QModelIndex itemIndex = createIndex(parentItem->IndexOfChild(item), 0, item);
	emit dataChanged(itemIndex, itemIndex);
}
