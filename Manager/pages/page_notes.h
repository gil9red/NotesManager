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

class QDomElement;
class QDomDocument;

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

public:
    Ui::Page_notes * ui;
    QSettings * settings;
};

#endif // PAGE_NOTES_H
