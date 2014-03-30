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

#include "notetagscollection.h"

#include "tag.h"
#include "note.h"
#include "utils/func.h"

#include <QDebug>



NoteTagsCollection::NoteTagsCollection(Note* owner) : owner(owner) {
	if (!owner) {
		WARNING("Null pointer recieved");
	}
}

void NoteTagsCollection::Add(Tag* tag) {
	if (!tag) {
		WARNING("Null pointer recieved");
		return;
	}
	if (tags.contains(tag)) {
		WARNING("Tag already in the list");
		return;
	}

	emit sg_ItemAboutToBeAdded(tag);
	tags.append(tag);
	tag->Owners.Add(owner);
	emit sg_ItemAdded(tag);
}

void NoteTagsCollection::Remove(Tag* tag) {
	if (!tag) {
		WARNING("Null pointer recieved");
		return;
	}
	if (!tags.contains(tag)) {
		WARNING("Tag is not in the list");
		return;
	}

	emit sg_ItemAboutToBeRemoved(tag);
	tags.removeOne(tag);
	tag->Owners.Remove(owner);
	emit sg_ItemRemoved(tag);
}

bool NoteTagsCollection::Contains(Tag* tag) const {
	if (!tag) {
		WARNING("Null pointer recieved");
		return false;
	}
	
	return tags.contains(tag);
}

void NoteTagsCollection::Clear() {
	foreach (Tag* tag, tags) {
		Remove(tag);
	}
}

int NoteTagsCollection::Count() const {
	return tags.count();
}

Tag* NoteTagsCollection::ItemAt(int index) const {
	if (index < 0 || index >= tags.count()) {
		WARNING("Index is out of bounds");
		return 0;
	}

	return tags.at(index);
}
