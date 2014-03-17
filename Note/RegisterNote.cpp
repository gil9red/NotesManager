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
    fileHashNote.insert( note->fileName(), note );
}
int RegisterNote::remove( RichTextNote * note )
{
    return fileHashNote.remove( note->fileName() );
}
RichTextNote * RegisterNote::get( const QString & fileName )
{
    return fileHashNote.value( fileName, 0 );
}
