#ifndef NOTEMODELITEM_H
#define NOTEMODELITEM_H

#include "basemodelitem.h"
#include "Note/RichTextNote.h"

class NoteModelItem: public BaseModelItem
{
public:
    explicit NoteModelItem( const QString & text = QString() );    
    void setNote( RichTextNote * note );
    RichTextNote * note();
};

#endif // NOTEMODELITEM_H
