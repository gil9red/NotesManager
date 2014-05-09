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

#include "tagownerscollection.h"
#include "utils/func.h"



TagOwnersCollection::TagOwnersCollection() {
}

void TagOwnersCollection::Add(Note* note) {
	if (!note) {
		WARNING("Null pointer recieved");
		return;
	}
	if (owners.contains(note)) {
		WARNING("Item is already in the list");
		return;
	}

	emit sg_ItemAboutToBeAdded(note);
	owners.append(note);
	emit sg_ItemAdded(note);
}

void TagOwnersCollection::Remove(Note* note) {
	if (!note) {
		WARNING("Null pointer recieved");
		return;
	}
	if (!owners.contains(note)) {
		WARNING("Item is not in the list");
		return;
	}

	emit sg_ItemAboutToBeRemoved(note);
	owners.removeAll(note);
	emit sg_ItemRemoved(note);
}

bool TagOwnersCollection::Contains(Note* note) const {
	if (!note) {
		WARNING("Null pointer recieved");
		return false;
	}

	return owners.contains(note);
}

void TagOwnersCollection::Clear() {
	emit sg_AboutToBeCleared();

	owners.clear();

	emit sg_Cleared();
}

int TagOwnersCollection::Count() const {
	return owners.count();
}

Note* TagOwnersCollection::ItemAt(int index) const {
	if (index < 0 || index >= owners.count()) {
		WARNING("Index is out of bounds");
		return 0;
	}

	return owners.at(index);
}
