#ifndef NOTEEDITOR_H
#define NOTEEDITOR_H

#include <QMainWindow>

namespace Ui {
    class NoteEditor;
}

#include "ui_noteeditor.h"
#include "RichTextNote.h"

class NoteEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit NoteEditor( QWidget * parent = 0 );
    ~NoteEditor();

    void setNote( RichTextNote * note );
    RichTextNote * note();

public slots:
    void noteChange( int event );

private slots:
    void titleChange();

public:
    Ui::NoteEditor * ui;
    RichTextNote * d_note;
};

#endif // NOTEEDITOR_H
