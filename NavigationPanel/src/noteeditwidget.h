#ifndef NOTEEDITWIDGET_H
#define NOTEEDITWIDGET_H

#include <QMainWindow>

namespace Ui
{
    class NoteEditWidget;
}

#include "NavigationPanel/src/note.h"

class NoteEditWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit NoteEditWidget( QWidget * parent = 0 );
    ~NoteEditWidget();

    void setNote( Note * note );
    Note * note();

private slots:
    void noteChange( int event );
    void titleChange();

public:
    Ui::NoteEditWidget * ui;
    Note * d_note;
};

#endif // NOTEEDITWIDGET_H
