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

#ifndef DATESMODEL_H
#define DATESMODEL_H

#include "basemodel.h"

#include <QHash>

class Note;
class NoteModelItem;
class DateModelItem;

class DatesModel : public BaseModel
{
    Q_OBJECT

public:
    enum LookupField
    {
        CreationDate,
        ModifyDate
    };

public:
    explicit DatesModel(LookupField field, QObject * parent = 0);

private:
    const LookupField lookupField;
    QHash<const Note*, BaseModelItem*> notesBridge;
    QHash<qint32, BaseModelItem*> datesBridge;
    qint32 GenerateDateID(const int year, const int month = 0, const int day = 0) const;
    qint32 GenerateDateID(const BaseModelItem*) const;

    void addNoteToTree(NoteModelItem*);
    void removeNoteFromTree(NoteModelItem*);

private slots:
    void sl_Note_DateChanged();

    void sl_NoteRegistered(Note*);
    void sl_NoteUnregistered(Note*);

    void sl_Item_DataChanged(BaseModelItem*);
};

#endif // DATESMODEL_H
