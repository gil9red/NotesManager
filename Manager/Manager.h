/*************************************************************************
**
** Copyright (C) 2013-2014 by Ilya Petrash
** All rights reserved.
** Contact: gil9red@gmail.com, ip1992@inbox.ru
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the
** Free Software Foundation, Inc.,
** 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
**
**************************************************************************/


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
    FancyTabBar * leftPanel;              //!< Главная панель менеджера. Находится слева.
    Page_Notes * pageNotes;               //!< Страница менеджера, которая отображает список заметок.
    Page_Settings * pageSettings;         //!< Страница менеджера, которая отображает настройки программы.
    Page_About * pageAbout;               //!< Страница менеджера, которая отображает "О программе".
    QSystemTrayIcon tray;                 //!< Класс системного трея.
    QAction * actionOpenManager;          //!< Показ окна менеджера. Действие в меню трея.
    QAction * actionSettings;             //!< Показать настройки менеджера. Действие в меню трея.
    QAction * actionAbout;                //!< Показ окна "О программе". Действие в меню трея.
    QAction * actionQuit;                 //!< Сохранить все заметки и закрыть программу. Действие в меню трея.
    QSettings * settings;                 //!< Указатель на класс настроек.
    QTimer autoSaveTimer;                 //!< Таймер автосохранения.

private slots:
    //! Вызывается, когда на иконки панели кликают.
    void buttonLeftPanelClicked( int index );

    //! Функция принимает сообщения, которые отправляются при создании дупликатов программы.
    void messageReceived( const QString & text );

    //! Функция принимает сигнал, отсылаемый треем.
    void messageReceived( QSystemTrayIcon::ActivationReason );

    void acceptChangeSettings();

    void show_page_notes();      //!< Показать страницу с заметками.
    void show_page_settings();   //!< Показать страницу настроек.
    void show_page_about();      //!< Показать страницу "О программе".
    void show_page_documentation(); //!< Показать страницу "Документация".

public slots:
    void show_Manager();  //!< Показать менеджер.
    void quit();          //!< Закрыть программу.
    void readSettings();  //!< Считывание настроек менеджера.
    void writeSettings(); //!< Запись настроек менеджера.

    void setActivateTimerAutosave( bool activate ); //!< Активация таймера автосохранения.
    bool isActivateTimerAutosave();                 //!< Функция вернет true, если таймер автосохранения активен.
    void setIntervalAutosave( quint64 minutes );    //!< Установка интервала таймера автосохранения.
    int intervalAutosave();                         //!< Функция вернет интервал таймера автосохранения.

protected:
    void closeEvent( QCloseEvent * event );
    void changeEvent( QEvent * event );
};

#endif // MANAGER_H
