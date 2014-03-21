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

class TrashModelItem;

//! Класс, реализующий виджет "Заметки".
class Page_Notes: public QMainWindow
{
    Q_OBJECT
    
public:
    explicit Page_Notes( QWidget * parent = 0 );
    ~Page_Notes();

    //! В функции указываем с каким классом настроек будем работать.
    void setSettings( QSettings * s );

private:
    BaseModelItem * createItemOfDomElement( const QDomElement & element );
    void parseDomElement( BaseModelItem * node, QDomElement & element );
    QDomElement createDomElementOfItem( BaseModelItem * item, QDomDocument & xmlDomDocument );
    void parseItem( BaseModelItem * node, QDomElement & element, QDomDocument & xmlDomDocument );
    void removeItem( BaseModelItem * item, BaseModelItem * parent );

public:
    Ui::Page_notes * ui;
    QSettings * settings;
    QStandardItemModel model;
    TrashModelItem * itemTrash;

    QHash < QStandardItem * , RichTextNote * > hashItemNote;

private slots:
    void noteChanged( int event );
    void noteChanged( QStandardItem * item );

public slots:
    bool read( QIODevice * device );
    bool write( QIODevice * device );

    void readSettings();
    void writeSettings();

    void addTopLevelNote();
    void addTopLevelFolder();
    void addFolder();
    void addNote();
    void rename();
    void open();
    void removeToTrash();
    void removeAllToTrash();
    void removeFromTrash();
    void clearTrash();

    void textColor();
    void defaultTextColor();
    void backColor();
    void defaultBackColor();

    void showContextMenu( const QPoint & pos );

protected:
    bool eventFilter( QObject * object, QEvent * event );
};

#endif // PAGE_NOTES_H
