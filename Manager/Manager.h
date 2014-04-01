#ifndef MANAGER_H
#define MANAGER_H

#include <QMainWindow>

namespace Ui
{
    class Manager;
}

#include "FancyTabBar/fancytabbar.h"
#include "pages/page_notes.h"
#include "pages/page_settings.h"
#include "pages/page_about.h"
#include "utils/func.h"

#include <QSystemTrayIcon>
#include <QDir>
#include <QDateTime>
#include <QTranslator>

//! Главный класс. Создает и управляет заметками.
class Manager: public QMainWindow
{
    Q_OBJECT   

public:
    explicit Manager( QWidget * parent = 0 );
    ~Manager();

    //! Загрузка заметок в менеджер.
    void loadNotes();

    //! В функции указываем с каким классом настроек будем работать.
    void setSettings( QSettings * s );
    
    //! Функция возвращает статичный указатель на "себя".
    static Manager * instance() { return self; }

    void nowReadyPhase();

private:
    void createToolBars(); //!< Создание панели инструментов.
    void createMenu();     //!< Создание меню.
    void createTray();     //!< Создание трея.

public:
    static Manager * self;                //!< Статичный указатель на "себя".
    Ui::Manager * ui;                     //!< Форма UI.
    Page_Notes * pageNotes;               //!< Страница менеджера, которая отображает список заметок.
    Page_Settings * pageSettings;         //!< Страница менеджера, которая отображает настройки программы.
    Page_About * pageAbout;               //!< Страница менеджера, которая отображает "О программе".

    QSystemTrayIcon tray;                 //!< Класс системного трея.
    QAction * trayActionOpenManager;          //!< Показ окна менеджера. Действие в меню трея.
    QAction * trayActionAddNote;              //!< Создать заметку. Действие в меню трея.
    QAction * trayActionAddNoteFromClipboard; //!< Создать заметку и содержимое брать из буфера обмена. Действие в меню трея.
    QAction * trayActionAddNoteFromScreen;    //!< Создать заметку и содержимое брать из скриншота. Действие в меню трея.
    QAction * trayActionShowAllNotes;         //!< Показать все заметки. Действие в меню трея.
    QAction * trayActionHideAllNotes;         //!< Скрыть все заметки. Действие в меню трея.
    QAction * trayActionSaveAllNotes;         //!< Сохранить все заметки. Действие в меню трея.
    QAction * trayActionRemoveAllNotes;       //!< Удалить все заметки. Действие в меню трея.
    QAction * trayActionSettings;             //!< Показать настройки менеджера. Действие в меню трея.
    QAction * trayActionAbout;                //!< Показ окна "О программе". Действие в меню трея.
    QAction * trayActionDocumentation;
    QAction * trayActionQuit;                 //!< Сохранить все заметки и закрыть программу. Действие в меню трея.

    QSettings * settings;                 //!< Указатель на класс настроек.
    QTimer autoSaveTimer;                 //!< Таймер автосохранения.

private slots:
    //! Вызывается, когда на иконки панели кликают.
    void buttonSidebarClicked( int index );

    //! Функция принимает сообщения, которые отправляются при создании дупликатов программы.
    void messageReceived( const QString & text );

    //! Функция принимает сигнал, отсылаемый треем.
    void messageReceived( QSystemTrayIcon::ActivationReason );

    void acceptChangeSettings();

    void show_page_notes();      //!< Показать страницу с заметками.
    void show_page_settings();   //!< Показать страницу настроек.
    void show_page_about();      //!< Показать страницу "О программе".
    void show_page_documentation(); //!< Показать страницу "Документация".

    void updateStates();

public slots:
    void show_Manager();  //!< Показать менеджер.
    void setFullScreen( bool fs );
    void quit();          //!< Закрыть программу.
    void readSettings();  //!< Считывание настроек менеджера.
    void writeSettings(); //!< Запись настроек менеджера.

    void openDictionary();
    void closeDictionary();

    void setShowSidebar( bool visible );
    bool isShowSidebar();
    void setShowStatusBar( bool show );
    bool isShowStatusBar();
    void setActivateTimerAutosave( bool activate ); //!< Активация таймера автосохранения.
    bool isActivateTimerAutosave();                 //!< Функция вернет true, если таймер автосохранения активен.
    void setIntervalAutosave( quint64 minutes );    //!< Установка интервала таймера автосохранения.
    int intervalAutosave();                         //!< Функция вернет интервал таймера автосохранения.

protected:
    void closeEvent( QCloseEvent * event );
    void changeEvent( QEvent * event );
};

#endif // MANAGER_H
