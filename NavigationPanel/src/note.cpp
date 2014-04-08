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
#include "folder.h"
#include "utils/func.h"

#include <QDebug>

Note::Note()
    : AbstractFolderItem( AbstractFolderItem::Type_Note ),
      Tags( this ),
      p_RichTextNote(0)
{
    QObject::connect(&Tags, SIGNAL(sg_ItemAboutToBeAdded(Tag*)), SIGNAL(sg_TagAboutToBeAdded(Tag*)));
    QObject::connect(&Tags, SIGNAL(sg_ItemAdded(Tag*)), SIGNAL(sg_TagAdded(Tag*)));
    QObject::connect(&Tags, SIGNAL(sg_ItemAboutToBeRemoved(Tag*)), SIGNAL(sg_TagAboutToBeRemoved(Tag*)));
    QObject::connect(&Tags, SIGNAL(sg_ItemRemoved(Tag*)), SIGNAL(sg_TagRemoved(Tag*)));

    QObject::connect(&Tags, SIGNAL(sg_ItemAdded(Tag*)), SLOT(sl_TagsCollectionModified(Tag*)));
    QObject::connect(&Tags, SIGNAL(sg_ItemRemoved(Tag*)), SLOT(sl_TagsCollectionModified(Tag*)));

    setIcon( QIcon( ":/fugue-icons/document" ) );
}

Note::~Note()
{
    // Удаления списка тэгов
    Tags.Clear();

    // Отсылаем сигнал о том, что мы все таки удалились. Скорее всего, его будет подхватывать только экземпляр QTabWidget, и то, если в нем
    // есть открытая вкладка, ассоциированная с данным экземпляром заметки
    emit sg_Removed();

    // Если нет указателя на заметку - странно, он должен быть
    if ( !p_RichTextNote )
    {
        WARNING( "null pointer!" );
        return;
    }
    WARNING( "Delete pointer RichTextNote and dir" );

    // Пусть заметка перед удалением сотрет себя с жесткого диска
    p_RichTextNote->removeDir();

    // Очищение памяти, выделенной под заметку
    delete p_RichTextNote;

    // На всякий случай, обнуление указателя
    p_RichTextNote = 0;
}

void Note::setRichTextNote( RichTextNote * richTextNote )
{
    if ( !richTextNote )
    {
        WARNING( "null pointer!" );
        return;
    }

    p_RichTextNote = richTextNote;
    QObject::connect(richTextNote, SIGNAL(changed(int)), SLOT(noteChange(int)));

    setName( p_RichTextNote->title() );
}
RichTextNote * Note::getRichTextNote()
{
    return p_RichTextNote;
}

void Note::setName(const QString & s)
{
    if ( !p_RichTextNote )
    {
        WARNING( "null pointer!" );
        return;
    }

    p_RichTextNote->setTitle(s);
    AbstractFolderItem::setName(s);
}

void Note::sl_TagsCollectionModified(Tag*)
{
    onChange();
}
void Note::noteChange( int event )
{
    if ( !p_RichTextNote )
    {
        WARNING( "null pointer!" );
        return;
    }

    switch ( event )
    {
    case EventsNote::ChangeTitle:
        setName( p_RichTextNote->title() );
        break;

    case EventsNote::Remove:
        // Если заметка будет удалена, то будет также удалена из модели ее элемент-обертка
        Folder * parent = this->getParent();
        // Если еще состоим в "родстве", удаляем
        if ( parent )
            parent->child.Remove( this );
        // Вызов удаления себя - увидимся в деструкторе
        this->deleteLater();
        break;
    }
}
