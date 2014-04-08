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
#include "appinfo.h"
#include "QtSingleApplication"

#include "NoteShared.h"

// TODO: изменение таблицы, списка и гиперссылок через меню редактора заметки
// TODO: добавить режим чтения (а может и редактирования?) заметок: полноэкранный режим (goodword1.04)
// TODO: добавить напоминания
// TODO: прикрепленные картинки можно вставлять
// TODO: выделять память только под видимые заметки
// TODO: панель форматирования подчеркивание - не правильно сохраняет
// TODO: вставку картинок из буфера обмена
// TODO: настройка: активность автодополнения и сохранение значения автодополнения
// TODO: настройка: кнопка восстановления значений по умолчанию
// TODO: при сохранении заметки дата изменения меняется
// TODO: ЗАМЕТКА: выбор цвета для: заголовка и тела заметки (взять из панели кнопку)
// TODO: гиперссылки у заметок
// TODO: настройки: добавить возможность у заметок убирать лишние компоненты: быстрый поиск и панель инструментов.
// (учитывать, что в ос не все символы в названиях файлов разрешены)
/// TODO: Вставка текстовых смайлов в редактор заметок
// TODO: добавить заметку с снимком от вебкамеры (Думаю, лучше будет использовать opencv)

/// TODO: добавить кнопку "Развернуть все" для иерархических деревьев
/// TODO: настроить удаление: если заметка в корзине - удаление ее, если вне корзины - то добавить в корзину
/// TODO: foldernavigationwidget доработать добавление заметок/папок. Убрать ограничение на добавляемые объекты.
/// TODO: доработать логику контекстного меню иерархического дерева заметок. В качестве основы, взять ту, что используется в page_notes. В идеале, использовать одни QAction - для page_notes и для иерархического дерева
/// TODO: действие Открыть для множества выделенных заметок
/// TODO: Добавить "Выделить текущую вкладку на дереве"
/// TODO: допилить словарь автодополнений
//// TODO: добавить выполнение скриптов
/// NOTE: git после выпуска стабильных версий, создавать ветку develop для новой версии

QString nm_Note::style = "";

//#include "ScriptModule/foo.h"

int main( int argc, char *argv[] )
{
    qsrand( QTime( 0, 0, 0 ).secsTo( QTime::currentTime() ) );
    QTextCodec::setCodecForCStrings( QTextCodec::codecForName( codec ) );    

    QtSingleApplication app( argc, argv );
    qApp->setApplicationName( App::name );
    qApp->setApplicationVersion( App::version );
    qApp->setQuitOnLastWindowClosed( false );


//    /// Example Script
//    qDebug() << ScriptModule::evaluate( "(1+2+3-2*2)/2");
//    //
//    QScriptEngine * scriptEngine = new QScriptEngine();
//    QScriptValue fun = scriptEngine->evaluate("(function(a, b) { return a + b; })");
//    QScriptValueList args;
//    args << 1 << 2;
//    QScriptValue result = fun.call(QScriptValue(), args);
//    qDebug() << result.toString();
//    //
//    QScriptValue pow = scriptEngine->evaluate("(function(a, b) { var res = 1; while( b-- ) res *= a; return res; })");
//    qDebug() << pow.call( QScriptValue(), QScriptValueList() << 5 << 3 ).toString();
//    /// Example Script


    if ( app.isRunning() )
    {
        app.sendMessage( "-show -message" );
        return 0;
    }

    QFile file( ":/Note/Note.qss" );
    if ( file.open( QIODevice::ReadOnly ) )
        nm_Note::style = file.readAll();

    QSettings * settings = createSettings();

    MySplashScreen * splashScreen = new MySplashScreen();
    QPixmap pixmap = QPixmap( ":/App/splash" );
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

    splashScreen->setMessage( QTranslator::tr( "Now ready" ), font );
    manager.nowReadyPhase();

    qApp->setActiveWindow( &manager );
    splashScreen->deleteLater();

    manager.loadNotes();

    QObject::connect( &app, SIGNAL( messageReceived(QString) ), &manager, SLOT( messageReceived(QString) ) );


    int code = app.exec();
    printf( "Exit code: %i\n", code );
    return code;
}
