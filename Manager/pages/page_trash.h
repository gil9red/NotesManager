#ifndef PAGE_TRASH_H
#define PAGE_TRASH_H

#include <QMainWindow>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QSettings>

namespace Ui
{
    class Page_Trash;
}

//! Класс, реализующий виджет "Корзина".
class Page_Trash: public QMainWindow
{
    Q_OBJECT
    
public:
    explicit Page_Trash( const QStringList & labels, QWidget * parent = 0 );
    ~Page_Trash();

    //! Загрузка.
    void loadNotes();
    
    //! В функции указываем с каким классом настроек будем работать.
    void setSettings( QSettings * s );

public:
    Ui::Page_Trash * ui;
    QStandardItemModel * model;
    QSortFilterProxyModel * sortModel;
    QSettings * settings; //!< Класс настройки.

public slots:
    void readSettings();  //!< Считывание настроек.
    void writeSettings(); //!< Запись настроек.
};

#endif // PAGE_TRASH_H
