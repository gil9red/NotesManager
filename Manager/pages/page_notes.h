#ifndef PAGE_NOTES_H
#define PAGE_NOTES_H

#include <QMainWindow>
#include <QStandardItemModel>

namespace Ui
{
    class Page_notes;
}

#include "ui_page_notes.h"
#include <QSettings>

//! Класс, реализующий виджет "Заметки".
class Page_Notes: public QMainWindow
{
    Q_OBJECT
//    Q_PROPERTY( addFolder RESET )
//    Q_PROPERTY( addNote RESET )
//    Q_PROPERTY( addNoteFromClipboard RESET )
//    Q_PROPERTY( addNoteFromScreen RESET )
//    Q_PROPERTY( setVisibleAllNotes RESET )
//    Q_PROPERTY( showAllNotes RESET )
//    Q_PROPERTY( hideAllNotes RESET )
//    Q_PROPERTY( saveAllNotes RESET )

public:
    explicit Page_Notes( QWidget * parent = 0 );
    ~Page_Notes();

    //! В функции указываем с каким классом настроек будем работать.
    void setSettings( QSettings * s );

    void read( QIODevice * device );
    void write( QIODevice * device );
    void writeToXmlStateNotes();

    void readSettings();
    void writeSettings();

    bool hasCurrent();
    bool currentIsNote();
    bool currentIsTrash();
    bool currentNoteIsVisible();
    bool currentNoteIsTop();
    bool currentIsChildTrash();
    bool trashIsEmpty();

public:
    Ui::Page_notes * ui;

private:
    QSettings * settings;

public slots:
    Q_SCRIPTABLE void addFolder();
    Q_SCRIPTABLE void addNote();
    Q_SCRIPTABLE void addNoteFromClipboard();
    Q_SCRIPTABLE void addNoteFromScreen();
    void sl_Delete();
    void sl_ClearTrash();
    void sl_RemoveToTrash();

    void sl_SaveNote();
    void sl_SaveAsNote();
    void sl_ShowNote();
    void sl_HideNote();
    Q_SCRIPTABLE void setVisibleAllNotes( bool visible );
    Q_SCRIPTABLE void showAllNotes();
    Q_SCRIPTABLE void hideAllNotes();
    Q_SCRIPTABLE void saveAllNotes();
    void sl_SetTopNote( bool top );
    void sl_PrintNote();
    void sl_PreviewPrintNote();

private slots:
    void sl_UpdateStates();

signals:
    void sg_About_UpdateStates();
};

#endif // PAGE_NOTES_H
