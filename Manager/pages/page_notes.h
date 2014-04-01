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
    
public:
    explicit Page_Notes( QWidget * parent = 0 );
    ~Page_Notes();

    //! В функции указываем с каким классом настроек будем работать.
    void setSettings( QSettings * s );

    void read( QIODevice * device );
    void write( QIODevice * device );

    void readSettings();
    void writeSettings();

    bool hasCurrent();
    bool currentIsNote();
    bool currentIsTrash();
    bool currentNoteIsVisible();
    bool currentNoteIsTop();
    bool currentIsChildTrash();
    bool trashIsEmpty();

private:
    Ui::Page_notes * ui;
    QSettings * settings;

public slots:
    void sl_AddFolder();
    void sl_AddNote();
    void sl_AddNoteFromClipboard();
    void sl_AddNoteFromScreen();
    void sl_Delete();
    void sl_ClearTrash();
    void sl_RemoveToTrash();

    void sl_SaveNote();
    void sl_SaveAsNote();
    void sl_ShowNote();
    void sl_HideNote();
    void sl_ShowAllNotes();
    void sl_HideAllNotes();
    void sl_SaveAllNotes();
    void sl_SetTopNote( bool top );
    void sl_PrintNote();
    void sl_PreviewPrintNote();

signals:
    void sg_About_UpdateStates();
};

#endif // PAGE_NOTES_H
