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

#include "foldermodelitem.h"

#include "folder.h"
#include "utils/func.h"

#include <QBrush>
#include <QApplication>
#include <QPalette>
#include <QPainter>


FolderModelItem::FolderModelItem(Folder* _folder) :
		BaseModelItem(BaseModelItem::folder),
        folder(_folder)
{
    if (!folder)
    {
		CRITICAL("Null pointer recieved");
    }else
        QObject::connect(folder, SIGNAL(sg_VisualPropertiesChanged()), SLOT(sl_Folder_PropertiesChanged()));
}

QVariant FolderModelItem::data(int role) const {
    if (!folder)
		return QVariant();

    if (role == Qt::DecorationRole)
			return folder->GetIcon();

    else if (role == Qt::DisplayRole)
    {
        QString childrenCount = QString(" (%1)").arg( folder->Items.Count() );
        QString returnValue = folder->GetName();
// TODO: if (Application::I()->Settings.GetShowNumberOfItems())
			returnValue.append(childrenCount);

		return returnValue;

    } else if (role == Qt::ToolTipRole)
		return folder->GetName();

    else if (role == Qt::EditRole)
		return folder->GetName();

    else if (role == Qt::BackgroundRole)
        return QBrush(folder->GetNameBackColor());

    else if (role == Qt::ForegroundRole)
        return QBrush(folder->GetNameForeColor());

    else if ( role == Qt::FontRole )
    {
        QFont font( qApp->font() );
        // Системные папки должны хоть немного отличаться от создаваемых пользователем
        if ( folder->GetType() == Folder::TempFolder || folder->GetType() == Folder::TrashFolder )
            font.setBold( true );

        return font;

    } else
		return QVariant();	
}

bool FolderModelItem::setData(const QVariant& value, int role) {
    if (!value.isValid())
        return false;

    if (role == Qt::EditRole)
    {
		QString newName = value.toString();
		newName.replace(QRegExp("[\a\e\f\n\r\t\v]"), " ");
		folder->SetName(newName);
		return true;
	}
	return false;
}

Qt::ItemFlags FolderModelItem::flags () const
{
	Qt::ItemFlags flags = BaseModelItem::flags();
    if ( folder->GetType() == Folder::UserFolder )
        return flags | Qt::ItemIsEditable;

	return flags;
}

Folder* FolderModelItem::GetStoredData() const {
	return folder;
}

void FolderModelItem::sl_Folder_PropertiesChanged() {
	emit sg_DataChanged(this);
}

bool FolderModelItem::LessThan(const BaseModelItem* item) const
{
    if (item->DataType() != BaseModelItem::folder)
		return BaseModelItem::LessThan(item);

	const FolderModelItem* folderItem = dynamic_cast<const FolderModelItem*>(item);
	return folder->GetName() < folderItem->folder->GetName();
}
