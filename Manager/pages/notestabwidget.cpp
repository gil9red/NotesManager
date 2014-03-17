#include "notestabwidget.h"
#include "noteeditor.h"
#include "utils/func.h"

QString cropString( QString text, int max = 13 )
{
    if ( text.length() > max )
        text = text.mid( 0, max ).append( "..." );

    return text;
}

NotesTabWidget::NotesTabWidget( QWidget * parent )
    : QTabWidget( parent )
{
    connect( this, SIGNAL( tabCloseRequested(int) ), SLOT( closeTab(int) ) );
}
NoteEditor * NotesTabWidget::widget( int index ) const
{
    return static_cast < NoteEditor * > ( QTabWidget::widget( index ) );
}

void NotesTabWidget::openTab( NoteModelItem * noteItem )
{
    if ( !noteItem )
    {
        WARNING( "null pointer!" );
        return;
    }

    NoteEditor * editor = hashNoteItemTab.value( noteItem, 0 );
    if ( editor )
        setCurrentWidget( editor );
    else
    {
        editor = new NoteEditor();
        editor->setNote( noteItem->note() );

        addTab( editor, noteItem->icon(), cropString( noteItem->text() ) );
        setCurrentWidget( editor );

        hashNoteItemTab.insert( noteItem, editor );
    }
}
void NotesTabWidget::closeTab( int index )
{
    NoteEditor * tab = widget( index );
    hashNoteItemTab.remove( hashNoteItemTab.key( tab ) );
    tab->deleteLater();
    removeTab( index );
}
void NotesTabWidget::closeAll()
{
    while( count() )
        closeTab(0);
}

void NotesTabWidget::onChangeNoteItems( QStandardItem * item )
{
    NoteModelItem * noteItem = static_cast < NoteModelItem * > ( item );
    if ( !noteItem )
    {
        WARNING( "null pointer!" );
        return;
    }

    // Если нет такой открытой вкладки, то вернется 0
    QWidget * tab = hashNoteItemTab.value( noteItem, 0 );
    if ( tab )
    {
        int index = indexOf( tab );
        setTabText( index, cropString( noteItem->text() ) );
        setTabIcon( index, noteItem->icon() );
    }
}
