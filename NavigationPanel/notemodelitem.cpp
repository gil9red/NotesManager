#include "notemodelitem.h"

#include "RegisterNote.h"

NoteModelItem::NoteModelItem( const QString & text )
    : BaseModelItem( text )
{
    setIcon( QIcon( ":/NavigationPanel/note" ) );
    setType( NOTE );

    setDragEnabled( true );
    setEditable( true );
}
void NoteModelItem::setNote( RichTextNote * note )
{
    RegisterNote::instance()->add( note );

    const QString & fileName = note->fileName();
    // имя папки заметок и так являются уникальными, поэтому можно и не использовать полный путь
    const QString & name_id = QDir( fileName ).dirName();

    setData( name_id, NOTE_TYPE );
}
RichTextNote * NoteModelItem::note()
{
    return RegisterNote::instance()->get( data( NOTE_TYPE ).toString() );
}
