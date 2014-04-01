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

#ifndef QNM_NOTETAGSCOLLECTION_H
#define QNM_NOTETAGSCOLLECTION_H

#include <QObject>
#include <QList>
#include <QReadWriteLock>

class Tag;
class Note;

class NoteTagsCollection : public QObject
{
    Q_OBJECT

private:
    QList<Tag*> tags;
    Note*	owner;

    NoteTagsCollection(const NoteTagsCollection&) : QObject(0) {} // Prevent construction by copying
    NoteTagsCollection& operator=(const NoteTagsCollection&){return *this;} // Prevent assignment

public:
    explicit NoteTagsCollection(Note*);

    void Add(Tag* tag);
    void Remove(Tag* tag);
    bool Contains(Tag* tag) const;
    void Clear();
    int Count() const;
    Tag* ItemAt(int index) const;


signals:
    void sg_ItemAboutToBeAdded(Tag*);
    void sg_ItemAdded(Tag*);
    void sg_ItemAboutToBeRemoved(Tag*);
    void sg_ItemRemoved(Tag*);
};


#endif // QNM_NOTETAGSCOLLECTION_H
