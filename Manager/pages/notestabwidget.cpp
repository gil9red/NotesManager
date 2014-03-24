#include "notestabwidget.h"
#include "noteeditor.h"
#include "utils/func.h"

NotesTabWidget::NotesTabWidget( QWidget * parent )
    : QTabWidget( parent )
{
    connect( this, SIGNAL( tabCloseRequested(int) ), SLOT( closeTab(int) ) );
}
NoteEditor * NotesTabWidget::widget( int index )
{
    return static_cast < NoteEditor * > ( QTabWidget::widget( index ) );
}

NoteModelItem * NotesTabWidget::noteItem( int index )
{
    NoteModelItem * noteItem = hashNoteItemTab.key( widget( index ) );
    return noteItem;
}
RichTextNote * NotesTabWidget::note( int index )
{
    return widget( index )->note();
}

NoteModelItem * NotesTabWidget::currentNoteItem()
{
    return noteItem( currentIndex() );
}
RichTextNote * NotesTabWidget::currentNote()
{
    return note( currentIndex() );
}

void NotesTabWidget::openTab( NoteModelItem * noteItem )
{
    qApp->setOverrideCursor( QCursor( Qt::WaitCursor ) );

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

    qApp->restoreOverrideCursor();
}
void NotesTabWidget::closeTab( int index )
{
    if ( index < 0 )
    {
        WARNING( "index < 0" );
        return;
    }

    NoteEditor * tab = widget( index );
    hashNoteItemTab.remove( hashNoteItemTab.key( tab ) );
    tab->deleteLater();
    removeTab( index );
}
void NotesTabWidget::closeAllTabs()
{
    qApp->setOverrideCursor( QCursor( Qt::WaitCursor ) );

    // Пока есть вкладки, закрываем первую
    while( count() )
        closeTab(0);

    qApp->restoreOverrideCursor();
}
void NotesTabWidget::closeAllTabsToLeftOfCurrent()
{
    qApp->setOverrideCursor( QCursor( Qt::WaitCursor ) );

    int index = 0;
    QWidget * tab = widget( index );
    QWidget * currentTab = currentWidget();

    // Пока указатели не совпали и индекс закрываемой вкладки меньше
    // индекса текущей
    while ( tab != currentTab && index < indexOf( currentTab ) )
    {
        closeTab( index );
        tab = widget( index );
    }

    qApp->restoreOverrideCursor();
}
void NotesTabWidget::closeAllTabsToRightOfCurrent()
{
    qApp->setOverrideCursor( QCursor( Qt::WaitCursor ) );

    int index = count() - 1;
    QWidget * tab = widget( index );
    QWidget * currentTab = currentWidget();

    // Пока указатели не совпали и индекс закрываемой вкладки меньше
    // индекса текущей
    while ( tab != currentTab && indexOf( currentTab ) < index )
    {
        closeTab( index );
        tab = widget( index );
        index = count() - 1;
    }

    qApp->restoreOverrideCursor();
}
void NotesTabWidget::closeAllTabsExceptCurrent()
{
    closeAllTabsToLeftOfCurrent();
    closeAllTabsToRightOfCurrent();
}
void NotesTabWidget::closeCurrentTab()
{
    qApp->setOverrideCursor( QCursor( Qt::WaitCursor ) );
    closeTab( currentIndex() );
    qApp->restoreOverrideCursor();
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
    if ( !tab )
    {
        WARNING( "null pointer!" );
        return;
    }

    int index = indexOf( tab );
    setTabText( index, cropString( noteItem->text() ) );
    setTabIcon( index, noteItem->icon() );
}

void NotesTabWidget::contextMenuEvent( QContextMenuEvent * event )
{
    // Если вкладок нет, не показываем меню
    if ( count() == 0 )
        return;

    QMenu menu( this );
    menu.addAction( QIcon( "" ), /*TODO*/ tr( "Показать текущую вкладку на дереве" ), this, SLOT( sendCurrentNoteModelItem() ), QKeySequence() );
    menu.addSeparator();
    menu.addAction( QIcon( "" ), tr( "Close current tab" ), this, SLOT( closeCurrentTab() ), QKeySequence() );
    QAction * actionCloseAll_Except = menu.addAction( QIcon( "" ), tr( "Close all tabs except the current" ), this, SLOT( closeAllTabsExceptCurrent() ), QKeySequence() );
    menu.addSeparator();
    QAction * actionCloseAll_Left = menu.addAction( QIcon( "" ), tr( "Close all tabs to the left of the current" ), this, SLOT( closeAllTabsToLeftOfCurrent() ), QKeySequence() );
    QAction * actionCloseAll_Right = menu.addAction( QIcon( "" ), tr( "Close all tabs to the right of the current" ), this, SLOT( closeAllTabsToRightOfCurrent() ), QKeySequence() );
    menu.addSeparator();
    menu.addAction( QIcon( "" ), tr( "Close all tabs" ), this, SLOT( closeAllTabs() ), QKeySequence() );

    actionCloseAll_Except->setEnabled( count() != 1 );
    actionCloseAll_Left->setEnabled( currentIndex() > 0 );
    actionCloseAll_Right->setEnabled( currentIndex() < count() - 1 );

    menu.exec( event->globalPos() );
}
