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

#ifndef QNM_NOTE_H
#define QNM_NOTE_H

#include "abstractfolderitem.h"
#include "notetagscollection.h"
#include "Note/RichTextNote.h"

#include <QObject>
#include <QDateTime>
#include <QIcon>
#include <QReadWriteLock>
#include <QTimer>
#include <QHash>


class Tag;

class Note : public AbstractFolderItem
{
    Q_OBJECT

public:
    explicit Note();
    ~Note();

    void setRichTextNote( RichTextNote * );
    RichTextNote * getRichTextNote();

    void SetName (const QString &s);

    NoteTagsCollection Tags;

private:
    RichTextNote * p_RichTextNote;

signals:
    void sg_TagAboutToBeAdded(Tag*);
    void sg_TagAdded(Tag*);
    void sg_TagAboutToBeRemoved(Tag*);
    void sg_TagRemoved(Tag*);

private slots:
    void sl_TagsCollectionModified(Tag*);
    void noteChange( int );
};


#endif // QNM_NOTE_H
