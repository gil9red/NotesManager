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

#ifndef QNM_TAGOWNERSCOLLECTION_H
#define QNM_TAGOWNERSCOLLECTION_H

#include <QObject>
#include <QList>
#include <QReadWriteLock>

class Tag;
class Note;

class TagOwnersCollection : public QObject
{
    Q_OBJECT

    friend class NoteTagsCollection;

private:
    QList<Note*> owners;

    void Add(Note* note);
    void Remove(Note* note);

public:
    explicit TagOwnersCollection();

    bool Contains(Note* note) const;
    void Clear();
    int Count() const;
    Note* ItemAt(int index) const;

signals:
    void sg_ItemAboutToBeAdded(Note*);
    void sg_ItemAdded(Note*);
    void sg_ItemAboutToBeRemoved(Note*);
    void sg_ItemRemoved(Note*);
    void sg_AboutToBeCleared();
    void sg_Cleared();
};


#endif // QNM_TAGOWNERSCOLLECTION_H
