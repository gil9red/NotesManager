#include "func.h"

namespace Create
{
    namespace Button
    {
        QToolButton * get( QWidget * parent, const QString & text, const QString & toolTip, const QString & statusTip, const QString & whatsThis, const QIcon & icon, bool checkable )
        {
            QToolButton * button = new QToolButton( parent );
            button->setText( text );
            button->setToolTip( toolTip );
            button->setWhatsThis( whatsThis );
            button->setStatusTip( statusTip );
            button->setCheckable( checkable );
            if ( !icon.isNull() )
                button->setIcon( icon );

            return button;
        }

        QToolButton * get( QWidget * parent, const QString & text, const QIcon & icon, bool checkable )
        {
            QToolButton * button = new QToolButton( parent );
            button->setText( text );
            button->setToolTip( text );
            button->setWhatsThis( text );
            button->setStatusTip( text );
            button->setCheckable( checkable );
            if ( !icon.isNull() )
                button->setIcon( icon );

            return button;
        }

        QToolButton * get( const QSize & size, const QSize & iconSize, const QIcon & icon, const bool checkable )
        {
            QToolButton * button = new QToolButton();
            button->setCheckable( checkable );
            button->setFixedSize( size );
            button->setIconSize( iconSize );
            button->setIcon( icon );
            return button;
        }

        QToolButton * clicked( const QString & text, const QString & toolTip, const QString & statusTip, const QString & whatsThis, const QIcon & icon, QWidget * receiver, const char * member )
        {
            QToolButton * button = get( receiver, text, toolTip, statusTip, whatsThis, icon );
            receiver->connect( button, SIGNAL( clicked() ), member );
            return button;
        }

        QToolButton * clicked( const QString & text, QWidget * receiver, const char * member )
        {
            return clicked( text, text, text, member, QIcon(), receiver, member );

        }

        QToolButton * clicked( const QString & text, const QIcon & icon, QWidget * receiver, const char * member )
        {
            return clicked( text, text, text, member, icon, receiver, member );

        }

        QToolButton * bClicked( const QString & text, const QString & toolTip, const QString & statusTip, const QString & whatsThis, const QIcon & icon, QWidget * receiver, const char * member )
        {
            QToolButton * button = get( receiver, text, toolTip, statusTip, whatsThis, icon, true );
            receiver->connect( button, SIGNAL( clicked( bool ) ), member );
            return button;
        }

        QToolButton * bClicked( const QString & text, QWidget * receiver, const char * member )
        {
            return bClicked( text, text, text, member, QIcon(), receiver, member );
        }

        QToolButton * bClicked( const QString & text, const QIcon & icon, QWidget * receiver, const char * member )
        {
            return bClicked( text, text, text, member, icon, receiver, member );
        }
    }

    namespace Action
    {
        QAction * get( QObject * parent, const QString & text, const QString & toolTip, const QString & whatsThis, const QIcon & icon, const QKeySequence & key, bool checkable )
        {
            QAction * action = new QAction( parent );
            action->setText( text );
            action->setToolTip( toolTip );
            action->setWhatsThis( whatsThis );
            action->setShortcut( key );
            action->setCheckable( checkable );
            if ( !icon.isNull() )
                action->setIcon( icon );

            return action;
        }

        QAction * get( const QString & text, const QIcon & icon, QVariant data )
        {
            QAction * action = get( 0, text, text, text, icon, 0 );
            action->setData( data );
            return action;
        }

        QAction * get( const QString & text, const QIcon & icon, bool checkable )
        {
            return get( 0, text, text, text, icon, 0, checkable );
        }

        QAction * triggered( const QString & text, const QString & toolTip, const QString & whatsThis, const QIcon & icon, const QKeySequence & key, QObject * receiver, const char * member )
        {
            QAction * action = get( receiver, text, toolTip, whatsThis, icon, key );
            receiver->connect( action, SIGNAL( triggered() ), member );
            return action;
        }

        QAction * triggered( const QString & text, QObject * receiver, const char * member )
        {
            return triggered( text, text, text, QIcon(), 0, receiver, member );
        }

        QAction * triggered( const QString & text, const QIcon & icon, QObject * receiver, const char * member )
        {
            return triggered( text, text, text, icon, 0, receiver, member );
        }

        QAction * triggered( const QIcon & icon,const QString & text, QObject * receiver, const char * member )
        {
            return triggered( text, text, text, icon, 0, receiver, member );
        }

        QAction * triggered( QString text, const QIcon & icon, const QKeySequence & key, QObject * receiver, const char * member )
        {
            return triggered( text, text, text, icon, key, receiver, member );
        }

        QAction * bTriggered( const QString & text, const QString & toolTip, const QString & whatsThis, const QIcon & icon, const QKeySequence & key, QObject * receiver, const char * member )
        {
            QAction * action = get( receiver, text, toolTip, whatsThis, icon, key, true );
            receiver->connect( action, SIGNAL( triggered( bool ) ), member );
            return action;
        }

        QAction * bTriggered( QString text, QObject * receiver, const char * member )
        {
            return bTriggered( text, text, text, QIcon(), 0, receiver, member );
        }

        QAction * bTriggered( QString text, const QIcon & icon, QObject * receiver, const char * member )
        {
            return bTriggered( text, text, text, icon, 0, receiver, member );
        }

        QAction * bTriggered( QString text, const QIcon & icon, const QKeySequence & key, QObject * receiver, const char * member )
        {
            return bTriggered( text, text, text, icon, key, receiver, member );
        }
    }
}


#include <QFileInfo>
#include <QDir>
void copyPath( const QString & oldPath, const QString & newPath )
{
    if ( QFileInfo( oldPath ).isDir() )
    {
        QDir().mkdir( newPath );
        foreach ( const QString & entry, QDir( oldPath ).entryList( QDir::AllDirs | QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot ) )
            copyPath( oldPath + QDir::separator() + entry, newPath + QDir::separator() + entry );
    } else
        QFile::copy( oldPath, newPath );
}
bool copyPath2( const QString & oldPath, const QString & newPath )
{
    if ( QFileInfo( oldPath ).isDir() )
    {
        if ( !QDir().mkdir( newPath ) )
            return false;

        foreach ( const QString & entry, QDir( oldPath ).entryList( QDir::AllDirs | QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot ) )
            if ( !copyPath2( oldPath + QDir::separator() + entry, newPath + QDir::separator() + entry ) )
                return false;
    } else
        if ( !QFile::copy( oldPath, newPath ) )
            return false;

    return true;
}
bool copyPathE( const QString & oldPath, const QString & newPath, bool interruptCopyingWhenError, bool logError )
{
    bool result = true;

    if ( QFileInfo( oldPath ).isDir() )
    {
        QDir().mkdir( newPath );
        foreach ( const QString & entry, QDir( oldPath ).entryList( QDir::AllDirs | QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot ) )
        {
            const QString & entryOldPath = oldPath + QDir::separator() + entry;
            const QString & entryNewPath = newPath + QDir::separator() + entry;
            result = copyPathE( entryOldPath, entryNewPath );

            if ( !result && interruptCopyingWhenError )
            {
                if ( logError )
                    qDebug() << "Error copying: " << entryOldPath << " -> " << entryNewPath;
                break;
            }
        }

    } else
    {
        result = QFile::copy( oldPath, newPath );

        if ( !result && interruptCopyingWhenError && logError )
            qDebug() << "Error copying: " << oldPath << " -> " << newPath;
    }

    return result;
}


QString getSettingsPath()
{
    return QDir::toNativeSeparators( qApp->applicationDirPath() + "/settings.ini" );
}
QString getTrPath()
{
    return QDir::toNativeSeparators( qApp->applicationDirPath() + "/translations" );
}
QString getFontsPath()
{
    return QDir::toNativeSeparators( qApp->applicationDirPath() + "/fonts" );
}
QString getDictionariesPath()
{
    return QDir::toNativeSeparators( qApp->applicationDirPath() + "/dictionaries" );
}

#include <QTranslator>
QString getWindowTitle()
{
    QString title;
    title += qApp->applicationName();
    title += " - " + QTranslator::tr( "The program creates notes" );
    title += " - " + QTranslator::tr( "version" ) + " " + qApp->applicationVersion();
    return title;
}

QSettings * createSettings()
{
    return new QSettings( getSettingsPath(), QSettings::IniFormat );
}

#include <QFontDatabase>
void loadFonts()
{
    foreach( const QFileInfo & info, QDir( getFontsPath() ).entryInfoList( QDir::Files ) )
        QFontDatabase::addApplicationFont( info.absoluteFilePath() );
}

#include <Note/completer.h>
void loadDictionaries()
{
    Completer * completer = new Completer();
    foreach( const QFileInfo & info, QDir( getDictionariesPath() ).entryInfoList( QDir::Files ) )
    {
        Dictionary * dictionary = new Dictionary();
        dictionary->load( info.filePath() );

        completer->addDictionary( dictionary );
    }
}

#include "pages/page_settings.h"
void loadTranslations( QSettings * settings )
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

#include <QThreadPool>
void initThreadCount()
{
    const int threadCount = QThreadPool::globalInstance()->maxThreadCount();
    QThreadPool::globalInstance()->setMaxThreadCount( qMax( 4, 2 * threadCount ) );
}

QString getNotesPath()
{
    return QDir::toNativeSeparators( qApp->applicationDirPath() + "/notes" );
}
QString getTrashPath()
{
    return QDir::toNativeSeparators( qApp->applicationDirPath() + "/trash" );
}

QString getNoteFormat()
{
    return "filenotes";
}

bool removePath( const QString & path )
{
    bool result = true;
    QFileInfo info( path );
    if ( info.isDir() )
    {
        QDir dir( path );
        foreach ( const QString & entry, dir.entryList( QDir::AllDirs | QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot ) )
            result &= removePath( dir.absoluteFilePath( entry ) );

        if ( !info.dir().rmdir( info.fileName() ) )
            return false;
    } else
        result = QFile::remove( path );

    return result;
}

#ifdef Q_WS_WIN
#include <windows.h>
#endif
QString getTheCurrentLanguageKeyboardLayouts()
{
    const int hexKeyLanguageRussian = 0x419;
    const int hexKeyLanguageEnglish = 0x409;

    QString name = QApplication::keyboardInputLocale().name();
#ifdef Q_WS_WIN
    switch( LOWORD ( GetKeyboardLayout( 0 ) ) )
    {
    case hexKeyLanguageRussian:
        name = "ru";
        break;
    case hexKeyLanguageEnglish:
        name = "en";
        break;
    }
#endif

    return name;
}

#include <QDialogButtonBox>
#include <QPushButton>
QMessageBox::StandardButton showNewMessageBox( QWidget * parent, QMessageBox::Icon icon, const QString& title, const QString& text,
                                               QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton,
                                               Qt::WindowFlags f )
{
    QMessageBox msgBox( icon, title, text, QMessageBox::NoButton, parent, f );
    QDialogButtonBox * buttonBox = msgBox.findChild < QDialogButtonBox * > ();
    Q_ASSERT(buttonBox != 0);

    uint mask = QMessageBox::FirstButton;
    while (mask <= QMessageBox::LastButton)
    {
        uint sb = buttons & mask;
        mask <<= 1;
        if (!sb)
            continue;
        QPushButton *button = msgBox.addButton((QMessageBox::StandardButton)sb);
        // Choose the first accept role as the default
        if (msgBox.defaultButton())
            continue;
        if ((defaultButton == QMessageBox::NoButton && buttonBox->buttonRole(button) == QDialogButtonBox::AcceptRole)
                || (defaultButton != QMessageBox::NoButton && sb == uint(defaultButton)))
            msgBox.setDefaultButton(button);
    }
    if (msgBox.exec() == -1)
        return QMessageBox::Cancel;

    return msgBox.standardButton(msgBox.clickedButton());
}


int indexChild( QStandardItem * parent, QStandardItem * child  )
{
    for ( int i = 0; i < parent->rowCount(); i++ )
        if ( parent->child(i) == child )
            return i;
    return -1;
}

QString cropString( QString text, int max )
{
    if ( text.length() > max )
        text = text.mid( 0, max ).append( "..." );

    return text;
}
