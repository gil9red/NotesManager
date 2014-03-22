#ifndef NOTESTABWIDGET_H
#define NOTESTABWIDGET_H

#include <QTabWidget>
#include <QHash>
#include "NavigationPanel/notemodelitem.h"

class NoteEditor;

class NotesTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    NotesTabWidget( QWidget * parent = 0 );
    NoteEditor * widget( int index );

    NoteModelItem * noteItem( int index );
    RichTextNote * note( int index );

    NoteModelItem * currentNoteItem();
    RichTextNote * currentNote();

private:
    QHash < NoteModelItem * , NoteEditor * > hashNoteItemTab;

public slots:
    void openTab( NoteModelItem * noteItem );
    void closeTab( int index );
    void closeAllTabs(); //!< Закрыть все вкладки
    void closeAllTabsToLeftOfCurrent(); //!< Закрыть все вкладки слева от текущей
    void closeAllTabsToRightOfCurrent(); //!< Закрыть все вкладки справа от текущей
    void closeAllTabsExceptCurrent(); //!< Закрыть все вкладки, кроме текущей
    void closeCurrentTab(); //!< Закрыть текущую вкладку

    void onChangeNoteItems( QStandardItem * item );      

protected:
    void contextMenuEvent( QContextMenuEvent * event );
};

#endif // NOTESTABWIDGET_H
