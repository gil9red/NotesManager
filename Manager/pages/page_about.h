#ifndef PAGE_ABOUT_H
#define PAGE_ABOUT_H

#include <QMainWindow>

namespace Ui
{
    class Page_About;
}

//! Класс, реализующий виджет "О программе".
class Page_About: public QMainWindow
{
    Q_OBJECT

public:
    //! Вкладки окна
    enum Tab
    {
        About,           //!< О программе
        RevisionHistory, //!< История изменений
        License,         //!< Лицензия
        Documentation    //!< Документация
    };

public:
    explicit Page_About(QWidget *parent = 0);
    ~Page_About();
    
public slots:
    void setCurrentTab( int index ); //!< Установить текущую вкладку
    int currentTab(); //!< Индекс текущей вкладки

private:
    Ui::Page_About * ui; //!< Форма UI.
};

#endif // PAGE_ABOUT_H
