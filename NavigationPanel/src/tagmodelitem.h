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

#ifndef TAGMODELITEM_H
#define TAGMODELITEM_H

#include "basemodelitem.h"

class Tag;

class TagModelItem : public BaseModelItem
{
private:
    const Tag*	tag;

public:
    TagModelItem(const Tag* tag);

    QVariant data(int role) const;
    bool LessThan(const BaseModelItem*) const;

    const Tag* GetTag() const;
};

#endif // TAGMODELITEM_H
