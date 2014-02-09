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

#include <QTextCodec>
#include <QThreadPool>
#include <QSettings>
#include <QStringList>
#include <QDir>
#include <QMessageBox>
#include <QTranslator>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QFontDatabase>
#include <QPluginLoader>
#include <QSplashScreen>
#include <QTranslator>
#include <QDebug>
#include <QDateTime>

static const char codec[] = "utf8";

#include "Manager/Manager.h"
#include "utils/func.h"
#include "utils/mysplashscreen.h"
#include "version.h"
#include "QtSingleApplication"


// TODO: изменение таблицы, списка и гиперссылок через меню редактора заметки
// TODO: может добавить в строку состояния заметок кол-во строк/символов и т.п.?
// TODO: добавить режим чтения (а может и редактирования?)
// заметок: полноэкранный режим (goodword1.04)
/// TODO: может корзину сделать частью группирования заметок?
/// TODO: удаленные заметки не отображаются, не редактируются, но их можно
/// просматривать через мереджер
/// TODO: добавить возможность группировки заметок по блокнотам
/// TODO: у корзины и блокнота на иконке рисовать кол-во элементов в них
// TODO: напоминания. думаю разумно напоминалки различать через полный путь заметок
// TODO: может добавитьв  менеджер лог, в котором отображать все события. лог будет слушать sendMessage
// TODO: прикрепленные картинки можно вставлять
// TODO: где то у заметок походу происходит утечка памяти покрайней мере, это фиксируется в диспетчее задач при создании и удалении заметки
// TODO: незаконченный клик вызывает только показ правой панели, пусть она появляется тоже при клике
// TODO: "а отображение можно будет отключить?..вдруг будет слишком большой текст в заметке..и тогда отображение будет тормозить(ну я так думаю)"
// TODO: панель справа и снизу, я на время уберу
// TODO: выделять память только под видимые заметки
// TODO: панель форматирования подчеркивание - не правильно сохраняет
// TODO: вставку картинок из буфера обмена
// TODO: настройка: активность автодополнения
// TODO: настройка: кнопка восстановления значений по умолчанию
// TODO: при сохранении заметки дата изменения меняется (
// TODO: ЗАМЕТКА: выбор цвета для: заголовка и тела заметки (взять из панели кнопку)
// TODO: гиперссылки у заметок
// TODO: НАстройки: "думаю самое простое убрать кнопку применить)) и сделать автоматом))"
// TODO: настройки: добавить возможность у заметок убирать лишние компоненты: быстрый поиск и панель инструментов.
// TODO: заметка/настройки: возможность сделать прозрачным фон редактора заметок, как в старых.
// TODO: панель форматирования - нижние виджеты не прижаты пружиной
// TODO: проверить работу дублирования
// TODO: может подправить скроллы у заметок, сделав как у старых.
// TODO: Заметка -> При сохранить как вставлять в качестве имени заголовок заметки
// (учитывать, что в ос не все символы в названиях файлов разрешены)
// TODO: загрузку заметок делать не во время инициализации, а после. прогресс показывать в строке состояния.

static void loadTranslations( QSettings * settings );

int main( int argc, char *argv[] )
{
    qsrand( QDateTime().toMSecsSinceEpoch() );
    QTextCodec::setCodecForCStrings( QTextCodec::codecForName( codec ) );

    QtSingleApplication app( argc, argv );
    qApp->setApplicationName( App::name );
    qApp->setApplicationVersion( App::version );
    qApp->setQuitOnLastWindowClosed( false );

    if ( app.isRunning() )
    {
        app.sendMessage( "-show -message" );
        return 0;
    }

    QSettings * settings = createSettings();

    MySplashScreen * splashScreen = new MySplashScreen();
    QPixmap pixmap = QPixmap( ":/splash" );
    splashScreen->setPixmap( pixmap );
    splashScreen->show();

    initThreadCount();
    loadTranslations( settings );

    QFont font( "Times New Roman", 9, QFont::Bold );
    splashScreen->setMessage( QTranslator::tr( "Loading fonts" ), font );
    loadFonts();

    splashScreen->setMessage( QTranslator::tr( "Loading dictionaries" ), font );
    loadDictionaries();

    splashScreen->setMessage( QTranslator::tr( "Creation" ), font );
    Manager manager;
    splashScreen->finish( &manager );

    manager.setSettings( settings );
    manager.setWindowTitle( getWindowTitle() );

    splashScreen->setMessage( QTranslator::tr( "Loading settings" ), font );
    manager.readSettings();

    splashScreen->setMessage( QTranslator::tr( "Loading notes" ), font );
    manager.loadNotes();

    splashScreen->setMessage( QTranslator::tr( "Now ready" ), font );
    manager.nowReadyPhase();

    qApp->setActiveWindow( &manager );
    splashScreen->deleteLater();

    QObject::connect( &app, SIGNAL( messageReceived(QString) ), &manager, SLOT( messageReceived(QString) ) );

    return app.exec();
}

static void loadTranslations( QSettings * settings )
{
    QString language = Page_Settings::getLanguage( settings );

    // по умолчанию весь перевод на английском, поэтому не ищем файлы перевода
    if ( language.contains( "en", Qt::CaseInsensitive ) )
        return;

    if ( language == Page_Settings::getDefaultLanguage() )
        language = QLocale::system().name();

    QStringList lang = language.split( '_' );

    foreach ( const QFileInfo & fileInfo, QDir( getTrPath() ).entryInfoList( QStringList() << "*.qminfo" ) )
    {
        // Если название файла схоже с языком, который хотим подгрузить
        bool successfull = false;
        foreach ( const QString & l , lang )
            if ( fileInfo.baseName().contains( l, Qt::CaseInsensitive ) )
            {
                successfull = true;
                break;
            }

        if ( successfull )
        {
            QSettings ini( fileInfo.absoluteFilePath(), QSettings::IniFormat );
            ini.setIniCodec( "utf8" );

            // подгружаем список файлов переводов, определенных в файле *.qminfo
            foreach ( const QString & fileName, ini.value( "Files" ).toStringList() )
            {
                QTranslator * translator = new QTranslator();
                translator->load( fileInfo.dir().path() + "/" + fileName );
                qApp->installTranslator( translator );
            }

            // одновременно перевод может быть только для одного языка,
            // поэтому нет смысла продолжать выполнение и мы выходим из функции
            return;
        }
    }
}
