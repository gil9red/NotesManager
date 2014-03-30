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

#ifndef TAGSMODEL_H
#define TAGSMODEL_H

#include "basemodel.h"
#include <QHash>
#include <QMultiHash>

class Tag;
class Note;
class TagModelItem;
class NoteModelItem;

class TagsModel : public BaseModel
{
    Q_OBJECT

public:
    explicit TagsModel(QObject * parent = 0);

private:
    QHash<const Tag*, TagModelItem*> tagsBridge;
    QMultiHash<const Note*, BaseModelItem*> notesBridge;

private slots:
    void sl_Tag_OwnerAdded(Note*);
    void sl_Tag_OwnerRemoved(Note*);
    void sl_Tag_OwnersRemoved();

    void sl_Document_TagRegistered(Tag*);
    void sl_Document_TagUnregistered(Tag*);

    void sl_Item_DataChanged(BaseModelItem*);
};


#endif // TAGSMODEL_H
