#ifndef NOTEEDITWIDGET_H
#define NOTEEDITWIDGET_H

#include <QMainWindow>

namespace Ui
{
    class NoteEditWidget;
}

#include "NavigationPanel/src/note.h"
#include <QByteArray>

class NoteEditWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit NoteEditWidget( QWidget * parent = 0 );
    ~NoteEditWidget();

    void setNote( Note * note );
    Note * note();

    QString saveStateNoteEdit();
    void restoreStateNoteEdit( const QString & state );

private slots:
    void noteChange( int event );
    void titleChange();

private:
    Ui::NoteEditWidget * ui;
    Note * d_note;
};

#endif // NOTEEDITWIDGET_H
