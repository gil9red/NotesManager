#include "RegisterNote.h"

RegisterNote * RegisterNote::self = 0;

RegisterNote::RegisterNote()
{
    self = this;
}
RegisterNote::~RegisterNote()
{
    self = 0;
}

RegisterNote * RegisterNote::instance()
{
    return self;
}

void RegisterNote::add( RichTextNote * note )
{
    const QString & fileName = note->fileName();
    // имя папки заметок и так являются уникальными, поэтому можно и не использовать полный путь
    const QString & name_id = QDir( fileName ).dirName();

    fileHashNote.insert( name_id, note );
}
int RegisterNote::remove( RichTextNote * note )
{
    const QString & fileName = note->fileName();
    // имя папки заметок и так являются уникальными, поэтому можно и не использовать полный путь
    const QString & name_id = QDir( fileName ).dirName();

    return fileHashNote.remove( name_id );
}
RichTextNote * RegisterNote::get( const QString & fileName )
{
    return fileHashNote.value( fileName, 0 );
}
QString RegisterNote::getIdName( RichTextNote * note )
{
    return fileHashNote.key( note );
}
