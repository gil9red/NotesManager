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
#include "ScriptModule/scriptsmanager.h"

//! Главный класс. Создает и управляет заметками.
class Manager: public QMainWindow
{
    Q_OBJECT   

public:
    explicit Manager( QWidget * parent = 0 );
    ~Manager();

    //! Загрузка заметок в менеджер.
    Q_SCRIPTABLE void loadNotes();

    //! Загрузка сценариев в менеджер сценариев.
    Q_SCRIPTABLE void loadScripts();

    //! В функции указываем с каким классом настроек будем работать.
    void setSettings( QSettings * s );
    
    //! Функция возвращает статичный указатель на "себя".
    static Manager * instance();

    void nowReadyPhase();

public:
    static Manager * self;                //!< Статичный указатель на "себя".
    Ui::Manager * ui;                     //!< Форма UI.
    Page_Notes * pageNotes;               //!< Страница менеджера, которая отображает список заметок.
    Page_Settings * pageSettings;         //!< Страница менеджера, которая отображает настройки программы.
    Page_About * pageAbout;               //!< Страница менеджера, которая отображает "О программе".

    QSystemTrayIcon * tray;                 //!< Класс системного трея.

    QSettings * settings;                 //!< Указатель на класс настроек.
    QTimer autoSaveTimer;                 //!< Таймер автосохранения.

    ScriptsManager * scriptsManager;
    QDockWidget * dockScriptsManager;

public slots:
    //! Вызывается, когда на иконки панели кликают.
    void buttonSidebarClicked( int index );

    //! Функция принимает сообщения, которые отправляются при создании дупликатов программы.
    void messageReceived( const QString & );

    //! Функция принимает сигнал, отсылаемый треем.
    void messageReceived( QSystemTrayIcon::ActivationReason );

    void acceptChangeSettings();

    Q_SCRIPTABLE void showPageNotes();  //!< Показать страницу с заметками.
    Q_SCRIPTABLE void showPageSettings(); //!< Показать страницу настроек.
    Q_SCRIPTABLE void showPageAbout(); //!< Показать страницу "О программе".
    Q_SCRIPTABLE void showPageDocumentation(); //!< Показать страницу "Документация".

    void updateStates();

public slots:
    Q_SCRIPTABLE void showManager();  //!< Показать менеджер.
    Q_SCRIPTABLE void setFullScreen( bool );
    Q_SCRIPTABLE void quit(); //!< Закрыть программу.

    Q_SCRIPTABLE void readSettings();  //!< Считывание настроек менеджера.
    Q_SCRIPTABLE void writeSettings(); //!< Запись настроек менеджера.

    Q_SCRIPTABLE void openDictionary();
    Q_SCRIPTABLE void closeDictionary();

    Q_SCRIPTABLE void setShowSidebar( bool );
    Q_SCRIPTABLE bool isShowSidebar();
    Q_SCRIPTABLE void setShowStatusBar( bool );
    Q_SCRIPTABLE bool isShowStatusBar();
    Q_SCRIPTABLE void setActivateTimerAutosave( bool ); //!< Активация таймера автосохранения.
    Q_SCRIPTABLE bool isActivateTimerAutosave();  //!< Функция вернет true, если таймер автосохранения активен.
    Q_SCRIPTABLE void setIntervalAutosave( quint64 minutes ); //!< Установка интервала таймера автосохранения.
    Q_SCRIPTABLE int intervalAutosave(); //!< Функция вернет интервал таймера автосохранения.

protected:
    void closeEvent( QCloseEvent * );
    void changeEvent( QEvent * );
};

#endif // MANAGER_H
