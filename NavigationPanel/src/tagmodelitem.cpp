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

#include "tagmodelitem.h"

#include "tag.h"
#include "notebook.h"
#include "utils/func.h"

#include <QPixmap>

TagModelItem::TagModelItem(const Tag* t)
    : BaseModelItem(BaseModelItem::tag),
      tag(t)
{
    if (!tag)
    {
		WARNING("Null pointer recieved");
	}
}

QVariant TagModelItem::data(int role) const
{
    if (!tag)
    {
		WARNING("Null pointer recieved");
		return QVariant();
	}

    if (role == Qt::DecorationRole)
        return QPixmap(":/gui/tag");// NOTE: this icon

    else if (role == Qt::DisplayRole)
    {
        QString childrenCount = QString(" (%1)").arg(QString::number(tag->Owners.Count()));
        QString returnValue = tag->GetName();
//		if (Application::I()->Settings.GetShowNumberOfItems())
			returnValue.append(childrenCount);

		return returnValue;

    } else if (role == Qt::ToolTipRole)
        return tag->GetName();

    else
		return QVariant();	
}

const Tag* TagModelItem::GetTag() const
{
    return tag;
}

bool TagModelItem::LessThan(const BaseModelItem* item) const
{
    if (item->DataType() != BaseModelItem::tag)
		return BaseModelItem::LessThan(item);

	const TagModelItem* tagItem = dynamic_cast<const TagModelItem*>(item);
    return tag->GetName() < tagItem->tag->GetName();
}
