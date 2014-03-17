#ifndef REGISTERNOTE_H
#define REGISTERNOTE_H

#include "RichTextNote.h"

#include <QHash>

class RegisterNote
{
public:
    RegisterNote();
    ~RegisterNote();

    //! Возврат статичного указателя на "себя".
    static RegisterNote * instance();

    void add( RichTextNote * note );
    int remove( RichTextNote * note );
    RichTextNote * get( const QString & fileName );

private:
    QHash < QString, RichTextNote * > fileHashNote;
    static RegisterNote * self;
};

#endif // REGISTERNOTE_H
