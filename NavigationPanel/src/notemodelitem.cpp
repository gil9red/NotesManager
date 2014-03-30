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

#include "notemodelitem.h"

#include "basemodel.h"
#include "note.h"
#include "utils/func.h"

#include <QBrush>
#include <QApplication>
#include <QPalette>
#include <QPainter>



NoteModelItem::NoteModelItem(Note* note) : BaseModelItem(BaseModelItem::note), _storedData(note) {
	if (!note) {
		WARNING("Null pointer recieved");
	} else {
        QObject::connect(note, SIGNAL(sg_VisualPropertiesChanged()), SLOT(sl_Note_PropertiesChanged()));
	}
}

QVariant NoteModelItem::data(int role) const {
    if (!_storedData)
        return QVariant();

    if (role == Qt::DecorationRole)
        return _storedData->GetIcon();

    else if (role == Qt::DisplayRole)
        return _storedData->GetName();

    else if (role == Qt::ToolTipRole)
        return _storedData->GetName();

    else if (role == Qt::EditRole)
        return _storedData->GetName();

    else if (role == Qt::BackgroundRole)
        return QBrush(_storedData->GetNameBackColor());

    else if (role == Qt::ForegroundRole)
        return QBrush(_storedData->GetNameForeColor());

    else
        return QVariant();
}

bool NoteModelItem::setData(const QVariant& value, int role) {
	if (!value.isValid()) {return false;}

	if (role == Qt::EditRole) {
		QString newName = value.toString();
		newName.replace(QRegExp("[\a\e\f\n\r\t\v]"), " ");
		_storedData->SetName(newName);
		return true;
	}
	return false;
}

Qt::ItemFlags NoteModelItem::flags () const {
    return BaseModelItem::flags() | Qt::ItemIsEditable;
}

Note* NoteModelItem::GetStoredData() const {
	return _storedData;
}

void NoteModelItem::sl_Note_PropertiesChanged() {
	emit sg_DataChanged(this);
}

bool NoteModelItem::LessThan(const BaseModelItem* item) const {
	if (item->DataType() != BaseModelItem::note) {
		return BaseModelItem::LessThan(item);
	}
	const NoteModelItem* noteItem = dynamic_cast<const NoteModelItem*>(item);
	return _storedData->GetName() < noteItem->_storedData->GetName();
}
