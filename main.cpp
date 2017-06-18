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

#include "Manager/Manager.h"
#include "utils/func.h"
#include "utils/mysplashscreen.h"
#include "appinfo.h"
#include "NoteShared.h"

QString nm_Note::style = "";

int main( int argc, char * argv[] )
{
    qsrand( QTime( 0, 0, 0 ).secsTo( QTime::currentTime() ) );
    QTextCodec::setCodecForCStrings( QTextCodec::codecForName( codec ) );    

    QApplication app( argc, argv );

    app.setApplicationName( App::name );
    app.setApplicationVersion( App::version );
    app.setQuitOnLastWindowClosed( false ); // Приложение не завершится, даже если все окна закрыты/скрыты    
    // Подгружаемые библиотеки, файлы и прочее, будет располагаться в следующих путях
    app.setLibraryPaths( QStringList() << "plugins" );

    QFile file( ":/Note/Note.qss" );
    if ( file.open( QIODevice::ReadOnly ) )
        nm_Note::style = file.readAll();

    QSettings * settings = createSettings();

    MySplashScreen * splashScreen = new MySplashScreen();
    splashScreen->setVersion( qApp->applicationVersion(), QFont( "Times New Roman", 16, QFont::Bold ) );
    const QPixmap & pixmap = QPixmap( ":/App/splash" );
    splashScreen->setPixmap( pixmap );
    splashScreen->show();

    initThreadCount();
    loadTranslations( settings );

    QFont font( "Times New Roman", 14, QFont::Bold );
    splashScreen->setMessage( QTranslator::tr( "Loading fonts" ), font );
    loadFonts();

    splashScreen->setMessage( QTranslator::tr( "Loading dictionaries" ), font );
    loadDictionaries();

    splashScreen->setMessage( QTranslator::tr( "Creation" ), font );

    Manager manager;
    QObject::connect( &app, SIGNAL(aboutToQuit()), &manager, SLOT(quit()) );

    splashScreen->finish( &manager );

    manager.setSettings( settings );
    manager.setWindowTitle( getWindowTitle() );

    splashScreen->setMessage( QTranslator::tr( "Loading settings" ), font );
    manager.readSettings();

    splashScreen->setMessage( QTranslator::tr( "Now ready" ), font );
    manager.nowReadyPhase();

    app.setActiveWindow( &manager );
    delete splashScreen;

    manager.loadNotes();

    return app.exec();
}
