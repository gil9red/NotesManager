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

#ifndef DATEMODELITEM_H
#define DATEMODELITEM_H

#include "basemodelitem.h"

class DateModelItem : public BaseModelItem {
public:
    enum DateComponent {
        Year,
        Month,
        Day,
        MonthAndDay // Not implemented yet
    };

    explicit DateModelItem(DateComponent, int);

    QVariant data(int role) const;
    bool LessThan(const BaseModelItem*) const;

    int value;
    DateComponent component;
};

#endif // DATEMODELITEM_H
