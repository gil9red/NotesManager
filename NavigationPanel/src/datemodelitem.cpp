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

#include "datemodelitem.h"
#include "notebook.h"
#include "utils/func.h"

#include <QDate>
#include <QIcon>

DateModelItem::DateModelItem(DateComponent c, int v) :
		BaseModelItem(BaseModelItem::date),
		value(v),
        component(c)
{
    if (v < 0)
        v = 0;

    if (component == Month && v > 11)
        v = 11;
}

QVariant DateModelItem::data(int role) const
{
    if (role == Qt::DecorationRole)
    {
        // Иконка под определенную дату: год, месяц, день
        QString stringIcon;
        switch (component)
        {
        case Year:
            stringIcon = ":/fugue-icons/calendar";
            break;

        case Month:
            stringIcon = ":/fugue-icons/calendar-month";
            break;

        case Day:
            stringIcon = ":/fugue-icons/calendar-day";
            break;

        case MonthAndDay:
            WARNING( "Wrong component type" );
            break;

        default:
            WARNING( "Wrong component type" );
            break;
        }

        return QIcon( stringIcon );

    }else if (role == Qt::DisplayRole)
    {
        QString childrenCount = QString(" (%1)").arg(QString::number(ChildrenCount()));
        QString returnValue = "";
        switch (component)
        {
        case Year:
            returnValue = QString::number(value);
            returnValue.append(childrenCount);
            break;

        case Month:
            returnValue = QDate( 2000, value, 1 ).toString("MMMM");
            returnValue.append(childrenCount);
            break;

        case Day:
            returnValue = QString::number(value).rightJustified(2, '0');
            returnValue.append(childrenCount);
            break;

        case MonthAndDay:
            WARNING("Wrong component type");
            break;

        default:
            WARNING("Wrong component type");
            break;
        }
        return returnValue;

    } else
        return QVariant();

    return QVariant();
}

bool DateModelItem::LessThan(const BaseModelItem* item) const {
    if (item->DataType() != BaseModelItem::date)
		return BaseModelItem::LessThan(item);

	const DateModelItem* dateItem = dynamic_cast<const DateModelItem*>(item);
	return value < dateItem->value;
}
