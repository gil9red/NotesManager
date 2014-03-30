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

#include "abstractfolderitem.h"
#include "folder.h"
#include "utils/func.h"

AbstractFolderItem::AbstractFolderItem(ItemType type, const QString & n )
    : QObject(0),
      itemType(type),
      name(n),
      icon( QIcon() ),
      defaultForeColor( Qt::black ),
      defaultBackColor( Qt::white ),
      nameForeColor(defaultForeColor),
      nameBackColor(defaultBackColor),
      creationDate(QDateTime::currentDateTime()),
      modificationDate(QDateTime::currentDateTime())
{
	parent = 0;
}

AbstractFolderItem::~AbstractFolderItem() {
}

void AbstractFolderItem::SetParent(Folder* newParent) {
    if (newParent == this)
    {
		WARNING("Trying to set item as it's parent");
		return;
	}

    if (newParent == parent)
        return;

	parent = newParent;
	setParent(newParent);  // QObject's parentship
	emit sg_ParentChanged(newParent);
}
void AbstractFolderItem::onChange()
{
    modificationDate = QDateTime::currentDateTime();
    emit sg_DataChanged();
}

AbstractFolderItem::ItemType AbstractFolderItem::GetItemType() const {
	return itemType;
}

Folder* AbstractFolderItem::GetParent() const {
	return parent;
}

bool AbstractFolderItem::IsOffspringOf(const Folder* folder) const {
	if (folder == 0) {
		WARNING("Null reference");
		return false;
	}

    if (folder == this)
        return false;

	Folder* f = parent;
    while (f != 0)
    {
        if (f == folder)
            return true;
		f = f->GetParent();
	}
	return false;
}


QString AbstractFolderItem::GetName() const {
    return name;
}

void AbstractFolderItem::SetName(const QString &s) {
    if (name == s)
        return;
    name = s;

    emit sg_VisualPropertiesChanged();
    onChange();
}

void AbstractFolderItem::SetIcon(const QIcon & ic) {
    if (ic.isNull())
    {
        WARNING("New icon is null");
        return;
    }

    icon = ic;

    emit sg_VisualPropertiesChanged();
    onChange();
}

QIcon AbstractFolderItem::GetIcon() const {
    return icon;
}

QDateTime AbstractFolderItem::GetCreationDate() const {
    return creationDate;
}

QDateTime AbstractFolderItem::GetModificationDate() const {
    return modificationDate;
}

QColor AbstractFolderItem::GetNameForeColor() const {
    return nameForeColor;
}

void AbstractFolderItem::SetNameForeColor(const QColor & c) {
    if (!c.isValid())
        return;
    if (nameForeColor == c)
        return;

    nameForeColor = c;

    emit sg_VisualPropertiesChanged();
    onChange();
}

QColor AbstractFolderItem::GetNameBackColor() const {
    return nameBackColor;
}

void AbstractFolderItem::SetNameBackColor(const QColor & c) {
    if (!c.isValid())
        return;
    if (nameBackColor == c)
        return;

    nameBackColor = c;

    emit sg_VisualPropertiesChanged();
    onChange();
}

QColor AbstractFolderItem::GetDefaultForeColor() const {
    return defaultForeColor;
}

QColor AbstractFolderItem::GetDefaultBackColor() const {
    return defaultBackColor;
}
