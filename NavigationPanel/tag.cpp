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

#include "tag.h"

#include <QDebug>

Tag::Tag(const QString& n) : name(n)
{
    QObject::connect(&Owners, SIGNAL(sg_ItemAboutToBeAdded(Note*)), SIGNAL(sg_OwnerAboutToBeAdded(Note*)));
    QObject::connect(&Owners, SIGNAL(sg_ItemAdded(Note*)), SIGNAL(sg_OwnerAdded(Note*)));
    QObject::connect(&Owners, SIGNAL(sg_ItemAboutToBeRemoved(Note*)), SIGNAL(sg_OwnerAboutToBeRemoved(Note*)));
    QObject::connect(&Owners, SIGNAL(sg_ItemRemoved(Note*)), SIGNAL(sg_OwnerRemoved(Note*)));
    QObject::connect(&Owners, SIGNAL(sg_AboutToBeCleared()), SIGNAL(sg_OwnersAboutToBeRemoved()));
    QObject::connect(&Owners, SIGNAL(sg_Cleared()), SIGNAL(sg_OwnersRemoved()));
}

QString Tag::getName() const
{
    return name;
}
