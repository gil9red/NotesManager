/*************************************************************************
**
** Copyright (C) 2013-2014 by Ilya Petrash
** All rights reserved.
** Contact: gil9red@gmail.com
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
#include "pages/page_trash.h"
#include "utils/func.h"

#include <QSystemTrayIcon>
#include <QDir>
#include <QDateTime>
#include <QTranslator>
#include <QSortFilterProxyModel>

//! Столбцы, используемые в таблице менеджера.
namespace Columns
{
    enum Columns
    {
        Title,      //!< Заголовок (название)
        ReadOnly,   //!< Только чтение
        Visibility, //!< Видимость
        Created,    //!< Дата создания
        Modified,   //!< Дата модификации
        Top,        //!< Поверх всех окон
        Attachments //!< Количество прикрепленных файлов
    };
}

//! Функция создает объект QStandardItem, и в его данные сохраняет указатель на заметку.
static QStandardItem * toStandardItem( RichTextNote * note, int role = Qt::UserRole + 1 )
{
    QVariant data;
    data.setValue( note );

    QStandardItem * item = new QStandardItem();
    item->setData( data, role );

    return item;
}

//! Функция возвращает указатель на заметку, которая хранилась в данной ячейке модели.
static RichTextNote * toNote( const QStandardItem * item, int role = Qt::UserRole + 1 )
{
    return item->data( role ).value < RichTextNote * > ();
}

/*! Перегруженная функция. */
static RichTextNote * toNote( const QModelIndex index, int role = Qt::UserRole + 1 )
{
    return index.data( role ).value < RichTextNote * > ();
}

//! Функция возвращает список ячеек, которые хранят информацию о заметке.
static QList < QStandardItem * > toStandardItems( RichTextNote * note )
{
    QStandardItem * title = toStandardItem( note );
    title->setText( note->title() );
    title->setToolTip( title->text() );

    QStandardItem * readOnly = new QStandardItem();
    readOnly->setText( note->isReadOnly() ? QTranslator::tr( "yes" ) : QTranslator::tr( "no" ) );
    readOnly->setTextAlignment( Qt::AlignCenter );

    QStandardItem * visibility = new QStandardItem();
    visibility->setText( note->isVisible() ? QTranslator::tr( "yes" ) : QTranslator::tr( "no" ) );
    visibility->setTextAlignment( Qt::AlignCenter );

    QStandardItem * created = new QStandardItem();
    created->setText( note->created().toString( Qt::SystemLocaleLongDate ) );

    QStandardItem * modified = new QStandardItem();
    modified->setText( note->modified().toString( Qt::SystemLocaleLongDate ) );

    QStandardItem * top = new QStandardItem();
    top->setText( note->isTop() ? QTranslator::tr( "yes" ) : QTranslator::tr( "no" ) );
    top->setTextAlignment( Qt::AlignCenter );

    QStandardItem * attachments = new QStandardItem();
    attachments->setText( QString::number( note->numberOfAttachments() ) );
    attachments->setTextAlignment( Qt::AlignCenter );    

    return QList < QStandardItem * > () << title << readOnly << visibility
                                        << created << modified << top << attachments;
}
//! Функция возвращает список ячеек, которые хранят информацию о заметке.
static QList < QStandardItem * > toStandardItems( const QString & path )
{
    QSettings ini( path + "/" + "settings.ini", QSettings::IniFormat );
    ini.setIniCodec( "utf8" );

    QStandardItem * title = new QStandardItem();
    title->setText( ini.value( "Title" ).toString() );
    title->setToolTip( title->text() );

    QStandardItem * readOnly = new QStandardItem();
    readOnly->setText( ini.value( "ReadOnly" ).toBool() ? QTranslator::tr( "yes" ) : QTranslator::tr( "no" ) );
    readOnly->setTextAlignment( Qt::AlignCenter );

    QStandardItem * visibility = new QStandardItem();
    visibility->setText( ini.value( "Visible" ).toBool() ? QTranslator::tr( "yes" ) : QTranslator::tr( "no" ) );
    visibility->setTextAlignment( Qt::AlignCenter );

    QStandardItem * created = new QStandardItem();
    created->setText( ini.value( "Created" ).toDateTime().toString( Qt::SystemLocaleLongDate ) );

    QStandardItem * modified = new QStandardItem();
    modified->setText( ini.value( "Modified" ).toDateTime().toString( Qt::SystemLocaleLongDate ) );

    QStandardItem * top = new QStandardItem();
    top->setText( ini.value( "Top" ).toBool() ? QTranslator::tr( "yes" ) : QTranslator::tr( "no" ) );
    top->setTextAlignment( Qt::AlignCenter );

    QStandardItem * attachments = new QStandardItem();
    attachments->setText( QString::number( QDir( path + "/" + "attach" ).entryList( QDir::Files ).size() ) );
    attachments->setTextAlignment( Qt::AlignCenter );

    return QList < QStandardItem * > () << title << readOnly << visibility
                                        << created << modified << top << attachments;
}

//! Функция ищет заметку в модели и возвращает указатель на ячейку, которая хранит эту заметку, или вернет 0.
static QStandardItem * findItem( RichTextNote * note, QStandardItemModel * model, int column = 0 )
{
    for ( int row = 0; row < model->rowCount(); row++ )
    {
        QStandardItem * item = model->item( row, column );
        if ( note == toNote( item ) )
            return item;
    }

    return 0;
}


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
    Page_Trash * pageTrash;               //!< Страница менеджера, которая отображает список удаленных заметок.
    Page_Settings * pageSettings;         //!< Страница менеджера, которая отображает настройки программы.
    Page_About * pageAbout;               //!< Страница менеджера, которая отображает "О программе".
    QSystemTrayIcon tray;                 //!< Класс системного трея.
    QAction * actionOpenManager;          //!< Показ окна менеджера. Действие в меню трея.
    QAction * actionAddNote;              //!< Создать заметку. Действие в меню трея.
    QAction * actionAddNoteFromClipboard; //!< Создать заметку и содержимое брать из буфера обмена. Действие в меню трея.
    QAction * actionAddNoteFromScreen;    //!< Создать заметку и содержимое брать из скриншота. Действие в меню трея.
    QAction * actionShowAllNotes;         //!< Показать все заметки. Действие в меню трея.
    QAction * actionHideAllNotes;         //!< Скрыть все заметки. Действие в меню трея.
    QAction * actionSaveAllNotes;         //!< Сохранить все заметки. Действие в меню трея.
    QAction * actionRemoveAllNotes;       //!< Удалить все заметки. Действие в меню трея.
    QAction * actionSettings;             //!< Показать настройки менеджера. Действие в меню трея.
    QAction * actionAbout;                //!< Показ окна "О программе". Действие в меню трея.
    QAction * actionQuit;                 //!< Сохранить все заметки и закрыть программу. Действие в меню трея.
    QSettings * settings;                 //!< Указатель на класс настроек.
    QTimer autoSaveTimer;                 //!< Таймер автосохранения.
    QStandardItemModel model;             //!< Модель, в которой описание всех заметок и указатели на них.
    QSortFilterProxyModel * sortModel;    //!< Отсортированная модель заметок.

private slots:
    //! Вызывается, когда на иконки панели кликают.
    void buttonLeftPanelClicked( int index );

    //! Функция принимает сообщения, которые отправляются при создании дупликатов программы.
    void messageReceived( const QString & text );

    //! Функция принимает сигнал, отсылаемый треем.
    void messageReceived( QSystemTrayIcon::ActivationReason );

    void acceptChangeSettings();

    void updateStates();         //!< Обновление состояния.
    void show_page_notes();      //!< Показать страницу с заметками.
    void show_page_trash();      //!< Показать страницу удаленных заметок.
    void show_page_settings();   //!< Показать страницу настроек.
    void show_page_about();      //!< Показать страницу "О программе".
    void show_page_documentation(); //!< Показать страницу "Документация".
    void open();                 //!< Выбрать файл заметки, который будет добавлен в менеджер.
    void addNote();              //!< Добавить заметку в менеджер.
    void addNoteFromClipboard(); //!< Добавить заметку с содержимым буфера обмена в менеджер.
    void addNoteFromScreen();    //!< Добавить заметку с скриншотом в менеджер.
    void removeAllNotes();       //!< Удалить все заметки.
    void saveAllNotes();         //!< Сохранить все заметки.
    void showAllNotes();         //!< Показать все заметки.
    void hideAllNotes();         //!< Скрыть все заметки.
    void openDictionary();       //!< Использовать функцию автодополнения.
    void closeDictionary();      //!< Отключить функцию автодополнения.
    void removeNote();           //!< Удалить заметку.
    void duplicateNote();        //!< Создать дупликат заметки.
    void saveNote();             //!< Сохранить заметку.
    void saveAsNote();           //!< Сохранить заметку как.
    void showNote();             //!< Показать заметку.
    void hideNote();             //!< Скрыть заметку.
    void printNote();            //!< Распечатать заметку.
    void previewPrintNote();     //!< Предпросмотр перед печати заметки.
    void setTopNote( bool top ); //!< Установить заметку поверх всех окон.
    void setReadOnlyNote( bool readOnly ); //!< Установить в заметке режим "только для чтения".
    void noteChange( int index ); //!< Функция вызывается, когда происходят изменения с какой-нибудь заметкой.

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

//signals:
//    // Когда происходят какие то события, отсылается данный сигнал,
//    // в котором описывается, что случилось
//    void sendMessage( QString );
};

#endif // MANAGER_H
