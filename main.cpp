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
// TODO: панель форматирования подчеркивание - не правильно сохраняет
// TODO: вставку картинок из буфера обмена
// TODO: настройка: активность автодополнения и сохранение значения автодополнения
// TODO: настройка: кнопка восстановления значений по умолчанию
// TODO: при сохранении заметки дата изменения меняется
// TODO: У заметок, выбор цвета для: заголовка и тела заметки (взять из панели кнопку)
// TODO: гиперссылки у заметок
// TODO: Вставка текстовых смайлов в редактор заметок
//
///
//// TODO: добавить выполнение скриптов
///
/// TODO: возможность добавления тэгов
/// TODO: Добавить "Выделить текущую вкладку на дереве"
/// TODO: допилить словарь автодополнений
///
/// NOTE: git после выпуска стабильных версий, создавать ветку develop для новой версии

QString nm_Note::style = "";

int main( int argc, char *argv[] )
{
    qsrand( QTime( 0, 0, 0 ).secsTo( QTime::currentTime() ) );
    QTextCodec::setCodecForCStrings( QTextCodec::codecForName( codec ) );    

    QtSingleApplication app( argc, argv );
    app.setApplicationName( App::name );
    app.setApplicationVersion( App::version );
    app.setQuitOnLastWindowClosed( false ); // Приложение не завершится, даже если все окна закрыты/скрыты

    // Если копия приложения уже запущена, тогда отсылаем сообщение той копии и заканчиваем процесс
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
    QObject::connect( &app, SIGNAL( messageReceived(QString) ), &manager, SLOT( messageReceived(QString) ) );

    splashScreen->finish( &manager );

    manager.setSettings( settings );
    manager.setWindowTitle( getWindowTitle() );

    splashScreen->setMessage( QTranslator::tr( "Loading settings" ), font );
    manager.readSettings();

    splashScreen->setMessage( QTranslator::tr( "Now ready" ), font );
    manager.nowReadyPhase();

    app.setActiveWindow( &manager );
    splashScreen->deleteLater();

    manager.loadNotes();


    int code = app.exec();
    printf( "Exit code: %i\n", code );
    return code;
}
