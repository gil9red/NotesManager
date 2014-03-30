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

#include "datesmodel.h"

#include "note.h"
#include "datemodelitem.h"
#include "notemodelitem.h"
#include "notebook.h"

#include <QDebug>

DatesModel::DatesModel(LookupField field, QObject * parent) :
        BaseModel(parent),
        lookupField(field)
{
	BaseModelItem* root = new BaseModelItem();
	root->SetSorted(true);
	SetRootItem(root);

    QObject::connect(Notebook::instance(), SIGNAL(sg_ItemRegistered(Note*)), SLOT(sl_NoteRegistered(Note*)));
    QObject::connect(Notebook::instance(), SIGNAL(sg_ItemUnregistered(Note*)), SLOT(sl_NoteUnregistered(Note*)));

    const QList<Note*> notes = Notebook::instance()->notesList();
    foreach( Note* n, notes )
		sl_NoteRegistered(n);	
}

void DatesModel::sl_Item_DataChanged(BaseModelItem* item) {
	QModelIndex noteItemIndex = createIndex(item->parent()->IndexOfChild(item), 0, item);
	emit dataChanged(noteItemIndex, noteItemIndex);
}

void DatesModel::sl_Note_DateChanged()
{
	Note* note = qobject_cast<Note*>(QObject::sender());
    if (note == 0)
    {
		WARNING("Casting error");
		return;
	}

	QDate noteDate;
    switch (lookupField)
    {
	case CreationDate:
		noteDate = note->GetCreationDate().date();
		break;
	case ModifyDate:
		noteDate = note->GetModificationDate().date();
		break;
	default:
		return;
	}

    if (!notesBridge.contains(note) && !noteDate.isValid())
		return;


	BaseModelItem* noteItem = 0;

	// Check if date didn't changed
    if (notesBridge.contains(note) && noteDate.isValid())
    {
		noteItem = notesBridge.value(note);
		DateModelItem* dayItem = dynamic_cast<DateModelItem*>(noteItem->parent());
		quint32 newDateID = GenerateDateID(noteDate.year(), noteDate.month(), noteDate.day());
		quint32 oldDateID = GenerateDateID(dayItem);
        if (oldDateID == newDateID) return;
	}

    if (notesBridge.contains(note))
    {
		noteItem = notesBridge.value(note);
		removeNoteFromTree(dynamic_cast<NoteModelItem*>(noteItem));
		qDebug() << "note item removed from tree";

        if (!noteDate.isValid())
        {
			notesBridge.remove(note);
			delete noteItem;
			qDebug() << "new date is invalid, note item deleted";
			return;
		}
	}

    if (noteDate.isValid())
    {
        if (!notesBridge.contains(note))
        {
			noteItem = new NoteModelItem(note);
			QObject::connect(noteItem, SIGNAL(sg_DataChanged(BaseModelItem*)), SLOT(sl_Item_DataChanged(BaseModelItem*)));
			notesBridge.insert(note, noteItem);
			qDebug() << "new note item created";
		}

		qDebug() << "trying to add item to tree";
		addNoteToTree(dynamic_cast<NoteModelItem*>(noteItem));
		qDebug() << "note item added to tree";
	}
}

void DatesModel::sl_NoteRegistered(Note* note) {
	if (!note) {
		WARNING("Null pointer recieved");
		return;
	}
	if (notesBridge.contains(note)) {
		WARNING("Note already registered");
		return;
	}


	QDateTime (Note::*fp)() const;
	fp = 0;

    switch (lookupField)
    {
	case CreationDate:
		fp = &Note::GetCreationDate;
		break;

	case ModifyDate:
//		QObject::connect(note, SIGNAL(sg_ModifyDateChanged()), this, SLOT(sl_Note_DateChanged()));
		fp = &Note::GetModificationDate;
		break;

	default:
		return;
	}

    if (!((note->*fp)().isValid())) return; // if note has invalid date then just subscribe to signals

	NoteModelItem* noteItem = new NoteModelItem(note);
    QObject::connect(noteItem, SIGNAL(sg_DataChanged(BaseModelItem*)), SLOT(sl_Item_DataChanged(BaseModelItem*)));
	addNoteToTree(noteItem);
	notesBridge.insert(note, noteItem);
}

void DatesModel::sl_NoteUnregistered(Note* note) {
	if (!note) {
		WARNING("Null pointer recieved");
		return;
	}

	if (!notesBridge.contains(note)) {return;}

	// delete items
	BaseModelItem* noteItem = notesBridge.value(note);
	QObject::disconnect(noteItem, 0, this, 0);
	removeNoteFromTree(dynamic_cast<NoteModelItem*>(noteItem));
	notesBridge.remove(note);
	delete noteItem;
}

void DatesModel::addNoteToTree(NoteModelItem* noteItem) {
	if (!noteItem) {
		WARNING("Null pointer recieved");
		return;
	}
	if (noteItem->GetStoredData() == 0) {
		WARNING("Item has no internal data");
		return;
	}

	QDate noteDate;

	switch (lookupField) {
	case CreationDate:
		noteDate = noteItem->GetStoredData()->GetCreationDate().date();
		break;
	case ModifyDate:
		noteDate = noteItem->GetStoredData()->GetModificationDate().date();
		break;
	default:
		return;
	}

	BaseModelItem* newParentItem = 0;

	qint32 dayID =		GenerateDateID(noteDate.year(), noteDate.month(), noteDate.day());
	qint32 monthID =	GenerateDateID(noteDate.year(), noteDate.month());
	qint32 yearID =		GenerateDateID(noteDate.year());
	int newElementPosition = 0;

	if (datesBridge.contains(dayID)) { // Search for day item
		newParentItem = datesBridge.value(dayID);
	} else if (datesBridge.contains(monthID)) { // search for month
		BaseModelItem* monthItem = datesBridge.value(monthID);
		DateModelItem* dayItem = new DateModelItem(DateModelItem::Day, noteDate.day());
		dayItem->SetSorted(true);
		QModelIndex monthIndex = createIndex(monthItem->parent()->IndexOfChild(monthItem), 0, monthItem);

		newElementPosition = monthItem->FindInsertIndex(dayItem);

		beginInsertRows(monthIndex, newElementPosition, newElementPosition); // find out new position, items sorting MUST be considered
			monthItem->AddChildTo(dayItem, newElementPosition);
		endInsertRows();

		datesBridge.insert(dayID, dayItem);
		newParentItem = dayItem;

	} else if (datesBridge.contains(yearID)) { // search for year
		BaseModelItem* yearItem = datesBridge.value(yearID);
		DateModelItem* monthItem = new DateModelItem(DateModelItem::Month, noteDate.month());
		monthItem->SetSorted(true);
		QModelIndex yearIndex = createIndex(GetRootItem()->IndexOfChild(yearItem), 0, yearItem);

		newElementPosition = yearItem->FindInsertIndex(monthItem);

		beginInsertRows(yearIndex, newElementPosition, newElementPosition);
			yearItem->AddChildTo(monthItem, newElementPosition);
		endInsertRows();

		datesBridge.insert(monthID, monthItem);

		DateModelItem* dayItem = new DateModelItem(DateModelItem::Day, noteDate.day());
		dayItem->SetSorted(true);
		QModelIndex monthIndex = createIndex(monthItem->parent()->IndexOfChild(monthItem), 0, monthItem);

		newElementPosition = monthItem->FindInsertIndex(dayItem);

		beginInsertRows(monthIndex, newElementPosition, newElementPosition); // find out new position, items sorting MUST be considered
			monthItem->AddChildTo(dayItem, newElementPosition);
		endInsertRows();

		datesBridge.insert(dayID, dayItem);
		newParentItem = dayItem;

	} else { // even year item was not found
		DateModelItem* yearItem = new DateModelItem(DateModelItem::Year, noteDate.year());
		yearItem->SetSorted(true);

		newElementPosition = GetRootItem()->FindInsertIndex(yearItem);

		beginInsertRows(QModelIndex(), newElementPosition, newElementPosition);
			GetRootItem()->AddChildTo(yearItem, newElementPosition);
		endInsertRows();

		datesBridge.insert(yearID, yearItem);

		DateModelItem* monthItem = new DateModelItem(DateModelItem::Month, noteDate.month());
		monthItem->SetSorted(true);
		QModelIndex yearIndex = createIndex(GetRootItem()->IndexOfChild(yearItem), 0, yearItem);

		newElementPosition = yearItem->FindInsertIndex(monthItem);

		beginInsertRows(yearIndex, newElementPosition, newElementPosition);
			yearItem->AddChildTo(monthItem, newElementPosition);
		endInsertRows();

		datesBridge.insert(monthID, monthItem);

		DateModelItem* dayItem = new DateModelItem(DateModelItem::Day, noteDate.day());
		dayItem->SetSorted(true);
		QModelIndex monthIndex = createIndex(monthItem->parent()->IndexOfChild(monthItem), 0, monthItem);

		newElementPosition = monthItem->FindInsertIndex(dayItem);

		beginInsertRows(monthIndex, newElementPosition, newElementPosition); // find out new position, items sorting MUST be considered
			monthItem->AddChildTo(dayItem, newElementPosition);
		endInsertRows();

		datesBridge.insert(dayID, dayItem);
		newParentItem = dayItem;
	}

	int newPosition = newParentItem->FindInsertIndex(noteItem);
	QModelIndex newParentIndex = createIndex(newParentItem->parent()->IndexOfChild(newParentItem),
											 0, newParentItem);

	beginInsertRows(newParentIndex, newPosition, newPosition);
		newParentItem->AddChildTo(noteItem, newPosition);
	endInsertRows();
}

void DatesModel::removeNoteFromTree(NoteModelItem* noteItem) {
	// Remove noteItem from dayItem
	BaseModelItem* dayItem = noteItem->parent();
	QModelIndex dayIndex = createIndex(dayItem->parent()->IndexOfChild(dayItem), 0, dayItem);

	beginRemoveRows(dayIndex, dayItem->IndexOfChild(noteItem), dayItem->IndexOfChild(noteItem));
		dayItem->RemoveChild(noteItem);
	endRemoveRows();

	if (dayItem->ChildrenCount() > 0) {return;}

	BaseModelItem* monthItem = dayItem->parent();
	QModelIndex monthIndex = createIndex(monthItem->parent()->IndexOfChild(monthItem), 0, monthItem);
	qint32 dayItemID = GenerateDateID(dayItem);

	beginRemoveRows(monthIndex, monthItem->IndexOfChild(dayItem), monthItem->IndexOfChild(dayItem));
		monthItem->RemoveChild(dayItem);
	endRemoveRows();

	datesBridge.remove(dayItemID);
	delete dayItem;

	if (monthItem->ChildrenCount() > 0) {return;}

	BaseModelItem* yearItem = monthItem->parent();
	QModelIndex yearIndex = createIndex(GetRootItem()->IndexOfChild(yearItem), 0, yearItem);
	qint32 monthItemID = GenerateDateID(monthItem);

	beginRemoveRows(yearIndex, yearItem->IndexOfChild(monthItem), yearItem->IndexOfChild(monthItem));
		yearItem->RemoveChild(monthItem);
	endRemoveRows();

	datesBridge.remove(monthItemID);
	delete monthItem;

	if (yearItem->ChildrenCount() > 0) {return;}

	qint32 yearItemID = GenerateDateID(yearItem);

	beginRemoveRows(QModelIndex(), GetRootItem()->IndexOfChild(yearItem), GetRootItem()->IndexOfChild(yearItem));
		GetRootItem()->RemoveChild(yearItem);
	endRemoveRows();

	datesBridge.remove(yearItemID);
	delete yearItem;
}

qint32 DatesModel::GenerateDateID(const int year, const int month, const int day) const {
	qint32 value = year * 10000 + month * 100 + day;
	return value;
}

qint32 DatesModel::GenerateDateID(const BaseModelItem* item) const {
	if (!item) {
		WARNING("Null pointer recieved");
		return 0;
	}
	if (item->DataType() != BaseModelItem::date) {
		WARNING("Item has wrong data type");
		return 0;
	}

	const DateModelItem* ditem = dynamic_cast<const DateModelItem*>(item);
	const DateModelItem* year = 0;
	const DateModelItem* month = 0;

	switch (ditem->component) {
	case DateModelItem::Year:
		return GenerateDateID(ditem->value);
		break;

	case DateModelItem::Month:
		if (ditem->parent() == 0) {
			WARNING("Item has no parent");
			return 0;
		}
		year = dynamic_cast<const DateModelItem*>(item->parent());
		if (year == 0) {
			WARNING("Casting error");
			return 0;
		}
		return GenerateDateID(year->value, ditem->value);
		break;

	case DateModelItem::Day:
		if (ditem->parent() == 0) {
			WARNING("Item has no parent");
			return 0;
		}
		month = dynamic_cast<const DateModelItem*>(item->parent());
		if (month == 0) {
			WARNING("Casting error");
			return 0;
		}

		if (month->parent() == 0) {
			WARNING("Item has no parent");
			return 0;
		}
		year = dynamic_cast<const DateModelItem*>(month->parent());
		if (year == 0) {
			WARNING("Casting error");
			return 0;
		}

		return GenerateDateID(year->value, month->value, ditem->value);
		break;

	default:
		WARNING("Wrong case branch");
		return 0;
	}
}
