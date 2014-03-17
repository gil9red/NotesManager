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
    NoteEditor * widget( int index ) const;

public slots:
    void openTab( NoteModelItem * noteItem );
    void closeTab( int index );
    void closeAll();

    void onChangeNoteItems( QStandardItem * item );

private:
    QHash < NoteModelItem * , NoteEditor * > hashNoteItemTab;
};

#endif // NOTESTABWIDGET_H
