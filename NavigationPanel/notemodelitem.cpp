#include "notemodelitem.h"

#include "RegisterNote.h"

NoteModelItem::NoteModelItem( const QString & text )
    : BaseModelItem( text )
{
    setIcon( QIcon( ":/note" ) );
    setType( NOTE );

    setDragEnabled( true );
    setEditable( true );
}
void NoteModelItem::setNote( RichTextNote * note )
{
    RegisterNote::instance()->add( note );
    setData( note->fileName(), NOTE_TYPE );
}
RichTextNote * NoteModelItem::note()
{
    return RegisterNote::instance()->get( data( NOTE_TYPE ).toString() );
}
