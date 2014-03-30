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

#include "note.h"
#include "utils/func.h"

#include <QDebug>



Note::Note()
    : AbstractFolderItem(AbstractFolderItem::Type_Note),
      Tags(this),
      p_RichTextNote(0)
{
    QObject::connect(&Tags, SIGNAL(sg_ItemAboutToBeAdded(Tag*)), SIGNAL(sg_TagAboutToBeAdded(Tag*)));
    QObject::connect(&Tags, SIGNAL(sg_ItemAdded(Tag*)), SIGNAL(sg_TagAdded(Tag*)));
    QObject::connect(&Tags, SIGNAL(sg_ItemAboutToBeRemoved(Tag*)), SIGNAL(sg_TagAboutToBeRemoved(Tag*)));
    QObject::connect(&Tags, SIGNAL(sg_ItemRemoved(Tag*)), SIGNAL(sg_TagRemoved(Tag*)));

    QObject::connect(&Tags, SIGNAL(sg_ItemAdded(Tag*)), SLOT(sl_TagsCollectionModified(Tag*)));
    QObject::connect(&Tags, SIGNAL(sg_ItemRemoved(Tag*)), SLOT(sl_TagsCollectionModified(Tag*)));
}

Note::~Note() {
    Tags.Clear();
}

void Note::setRichTextNote( RichTextNote * richTextNote )
{
    p_RichTextNote = richTextNote;
    SetName( p_RichTextNote->title() );
}
RichTextNote * Note::getRichTextNote()
{
    return p_RichTextNote;
}

void Note::SetName (const QString & s)
{
    getRichTextNote()->setTitle(s);
    AbstractFolderItem::SetName(s);
}

void Note::sl_TagsCollectionModified(Tag*)
{
    onChange();
}
