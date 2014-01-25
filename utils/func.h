#ifndef FUNC_H
#define FUNC_H

#include <QToolButton>
#include <QAction>
#include <QDebug>
#include <QApplication>


//! Содержит классы, облегчающие создание объектов, имеющих множество свойств (параметров).
namespace Create
{
    //! Содержит статические методы создания кнопок.
    struct Button
    {
        //! Функция создает кнопку и возвращает указатель на нее.
        /*! Происходит создание кнопки и установка ей свойств, переданных в параметрах.
         *  Вот так будет создана кнопка:
         *  \code
         *  QToolButton * button = new QToolButton( 0 );
         *  button->setText( "Нажми на меня" );
         *  button->setToolTip( "Подсказка" );
         *  button->setStatusTip( "Состояние" );
         *  button->setWhatsThis( "Кнопка" );
         *  button->setIcon( QIcon( ":/icon" ) );
         *  button->setCheckable( true );
         *  \endcode
         *  Тоже самое, но с использование данной функции:
         *  \code
         *  QToolButton * button = Create::Button::get( 0, "Нажми на меня", "Подсказка", "Состояние", "Кнопка", QIcon( ":/icon" ), true );
         *  \endcode
         *  \param parent указатель на виджет-родитель
         *  \param text текст на кнопке
         *  \param toolTip текст всплывающей подсказки
         *  \param statusTip состояние виджета
         *  \param whatsThis текст подсказки What's This виджета
         *  \param icon иконка на кнопке
         *  \param checkable данное свойство указывает, является-ли кнопка контрольной
         *  \return указатель на созданную в функции кнопку
         */
        static QToolButton * get( QWidget * parent, const QString & text, const QString & toolTip, const QString & statusTip, const QString & whatsThis, const QIcon & icon, bool checkable = false )
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

        /*! Перегруженная функция. */
        static QToolButton * get( QWidget * parent, const QString & text, const QIcon & icon, bool checkable = false )
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

        /*! Перегруженная функция. */
        static QToolButton * get( const QSize & size, const QSize & iconSize, const QIcon & icon, const bool checkable = true )
        {
            QToolButton * button = new QToolButton();
            button->setCheckable( checkable );
            button->setFixedSize( size );
            button->setIconSize( iconSize );
            button->setIcon( icon );
            return button;
        }

        //! Функция создает кнопку, соединяет ее сигнал clicked(void) к переданному слоту или сигналу, и возвращает указатель на нее.
        /*! Происходит создание кнопки и установка ей свойств, переданных в параметрах.
         *  \code
         *  QWidget * widget = new QWidget();
         *  widget->show();
         *  QToolButton * button = Create::Button::clicked( "Закрыть", "Закрыть это окно", "", "", QIcon( "" ), widget, SLOT( close() ) );
         *  \endcode
         *  \param text текст на кнопке
         *  \param toolTip текст всплывающей подсказки
         *  \param statusTip состояние виджета
         *  \param whatsThis текст подсказки What's This виджета
         *  \param icon иконка на кнопке
         *  \param receiver указатель на родителя, а также объект, с которым кнопка связана сигналом clicked(void)
         *  \param member слот или сигнал, задается через макросы SLOT() и SIGNAL()
         *  \return указатель на созданную в функции кнопку
         *  \sa get() и bClicked()
         */
        static QToolButton * clicked( const QString & text, const QString & toolTip, const QString & statusTip, const QString & whatsThis, const QIcon & icon, QWidget * receiver, const char * member )
        {
            QToolButton * button = get( receiver, text, toolTip, statusTip, whatsThis, icon );
            receiver->connect( button, SIGNAL( clicked() ), member );
            return button;
        }

        /*! Перегруженная функция.
         *  \sa get() и bClicked()
         */
        static QToolButton * clicked( const QString & text, QWidget * receiver, const char * member )
        {
            return clicked( text, text, text, member, QIcon(), receiver, member );

        }

        /*! Перегруженная функция.
         *  \sa get() и bClicked()
         */
        static QToolButton * clicked( const QString & text, const QIcon & icon, QWidget * receiver, const char * member )
        {
            return clicked( text, text, text, member, icon, receiver, member );

        }

        //! Функция создает кнопку, соединяет ее сигнал clicked(bool) к переданному слоту или сигналу, и возвращает указатель на нее.
        /*! Такая же функция как и clicked, только отправляет сигнал clicked(bool), и является контрольной.
         *  \sa get() и clicked()
         */
        static QToolButton * bClicked( const QString & text, const QString & toolTip,const QString & statusTip, const QString & whatsThis, const QIcon & icon, QWidget * receiver, const char * member )
        {
            QToolButton * button = get( receiver, text, toolTip, statusTip, whatsThis, icon, true );
            receiver->connect( button, SIGNAL( clicked( bool ) ), member );
            return button;
        }

        /*! Перегруженная функция.
         *  \sa get() и clicked()
         */
        static QToolButton * bClicked( const QString & text, QWidget * receiver, const char * member )
        {
            return bClicked( text, text, text, member, QIcon(), receiver, member );
        }

        /*! Перегруженная функция.
         *  \sa get() и clicked()
         */
        static QToolButton * bClicked( const QString & text, const QIcon & icon, QWidget * receiver, const char * member )
        {
            return bClicked( text, text, text, member, icon, receiver, member );
        }
    };

    //! Содержит статические методы создания действий.
    struct Action
    {
        //! Функция создает действие и возвращает указатель на него.
        /*! Происходит создание действия и установка его свойств, переданных в параметрах.
         *  Вот так будет создана кнопка:
         *  \code
         *  QAction * action = new QAction( this );
         *  action->setText( "Открыть" );
         *  action->setToolTip( "Открытие ..." );
         *  action->setWhatsThis( "Действие открывает ..." );
         *  action->setShortcut( QKeySequence( QKeySequence::Open ) );
         *  action->setIcon( QIcon( ":/icon" ) );
         *  action->setCheckable( true );
         *  \endcode
         *  Тоже самое, но с использование данной функции:
         *  \code
         *  QAction * action = Create::Action::get( this, "Открыть", "Открытие ...", "Действие открывает ...", QIcon( ":/icon" ), QKeySequence( QKeySequence::Open ), true );
         *  \endcode
         *  \param text название действия
         *  \param toolTip текст всплывающей подсказки
         *  \param whatsThis текст подсказки What's This действия
         *  \param icon иконка на действие
         *  \param key комбинация "горячей клавиши"
         *  \param checkable данное свойство указывает, является-ли действие контрольным
         *  \return указатель на созданное в функции действие
         */
        static QAction * get( QObject * parent, const QString & text, const QString & toolTip, const QString & whatsThis, const QIcon & icon, const QKeySequence & key, bool checkable = false )
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

        /*! Перегруженная функция. */
        static QAction * get( const QString & text, const QIcon & icon, QVariant data )
        {
            QAction * action = get( 0, text, text, text, icon, 0 );
            action->setData( data );
            return action;
        }

        /*! Перегруженная функция. */
        static QAction * get( const QString & text, const QIcon & icon, bool checkable = false )
        {
            return get( 0, text, text, text, icon, 0, checkable );
        }

        //! Функция создает действие, соединяет его сигнал triggered(void) к переданному слоту или сигналу, и возвращает указатель на него.
        /*! Происходит создание действия и установка его свойств, переданных в параметрах.
         *  \param text название действия
         *  \param toolTip текст всплывающей подсказки
         *  \param whatsThis текст подсказки What's This действия
         *  \param icon иконка на действие
         *  \param key комбинация "горячей клавиши"
         *  \param receiver указатель на родителя, а также объект, с которым действие связано сигналом triggered(void)
         *  \param member слот или сигнал, задается через макросы SLOT() и SIGNAL()
         *  \return указатель на созданное в функции действие
         *  \sa get() и bTriggered()
         */
        static QAction * triggered( const QString & text, const QString & toolTip, const QString & whatsThis, const QIcon & icon, const QKeySequence & key, QObject * receiver, const char * member )
        {
            QAction * action = get( receiver, text, toolTip, whatsThis, icon, key );
            receiver->connect( action, SIGNAL( triggered() ), member );
            return action;
        }

        /*! Перегруженная функция. */
        static QAction * triggered( const QString & text, QObject * receiver, const char * member )
        {
            return triggered( text, text, text, QIcon(), 0, receiver, member );
        }

        /*! Перегруженная функция. */
        static QAction * triggered( const QString & text, const QIcon & icon, QObject * receiver, const char * member )
        {
            return triggered( text, text, text, icon, 0, receiver, member );
        }

        /*! Перегруженная функция. */
        static QAction * triggered( const QIcon & icon,const QString & text, QObject * receiver, const char * member )
        {
            return triggered( text, text, text, icon, 0, receiver, member );
        }

        /*! Перегруженная функция. */
        static QAction * triggered( QString text, const QIcon & icon, const QKeySequence & key, QObject * receiver, const char * member )
        {
            return triggered( text, text, text, icon, key, receiver, member );
        }

        //! Функция создает действие, соединяет его сигнал triggered(bool) к переданному слоту или сигналу, и возвращает указатель на него.
        /*! Происходит создание действия и установка его свойств, переданных в параметрах.
         *  \param text название действия
         *  \param toolTip текст всплывающей подсказки
         *  \param whatsThis текст подсказки What's This действия
         *  \param icon иконка на действие
         *  \param key комбинация "горячей клавиши"
         *  \param receiver указатель на родителя, а также объект, с которым действие связано сигналом triggered(bool)
         *  \param member слот или сигнал, задается через макросы SLOT() и SIGNAL()
         *  \return указатель на созданное в функции действие
         *  \sa get() и triggered()
         */
        static QAction * bTriggered( const QString & text, const QString & toolTip, const QString & whatsThis, const QIcon & icon, const QKeySequence & key, QObject * receiver, const char * member )
        {
            QAction * action = get( receiver, text, toolTip, whatsThis, icon, key, true );
            receiver->connect( action, SIGNAL( triggered( bool ) ), member );
            return action;
        }

        /*! Перегруженная функция. */
        static QAction * bTriggered( QString text, QObject * receiver, const char * member )
        {
            return bTriggered( text, text, text, QIcon(), 0, receiver, member );
        }

        /*! Перегруженная функция. */
        static QAction * bTriggered( QString text, const QIcon & icon, QObject * receiver, const char * member )
        {
            return bTriggered( text, text, text, icon, 0, receiver, member );
        }

        /*! Перегруженная функция. */
        static QAction * bTriggered( QString text, const QIcon & icon, const QKeySequence & key, QObject * receiver, const char * member )
        {
            return bTriggered( text, text, text, icon, key, receiver, member );
        }
    };
}

#include <QDir>
static void copyPath( const QString & oldPath, const QString & newPath )
{
    if ( QFileInfo( oldPath ).isDir() )
    {
        QDir().mkdir( newPath );
        foreach ( const QString & entry, QDir( oldPath ).entryList( QDir::AllDirs | QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot ) )
            copyPath( oldPath + QDir::separator() + entry, newPath + QDir::separator() + entry );
    } else
        QFile::copy( oldPath, newPath );
}
static bool copyPath2( const QString & oldPath, const QString & newPath )
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
static bool copyPathE( const QString & oldPath, const QString & newPath, bool interruptCopyingWhenError = false, bool logError = true )
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


static QString getSettingsPath()
{
    return QDir::toNativeSeparators( qApp->applicationDirPath() + "/settings.ini" );
}
static QString getTrPath()
{
    return QDir::toNativeSeparators( qApp->applicationDirPath() + "/translations" );
}
static QString getFontsPath()
{
    return QDir::toNativeSeparators( qApp->applicationDirPath() + "/fonts" );
}
static QString getDictionariesPath()
{
    return QDir::toNativeSeparators( qApp->applicationDirPath() + "/dictionaries" );
}

#include <QTranslator>
static QString getWindowTitle()
{
    QString title;
    title += qApp->applicationName();
    title += " - " + QTranslator::tr( "The program creates notes" );
    title += " - " + QTranslator::tr( "version" ) + " " + qApp->applicationVersion();
    return title;
}

#include <QSettings>
static QSettings * createSettings()
{
    return new QSettings( getSettingsPath(), QSettings::IniFormat );
}

#include <QFontDatabase>
static void loadFonts()
{
    foreach( const QFileInfo & info, QDir( getFontsPath() ).entryInfoList( QDir::Files ) )
        QFontDatabase::addApplicationFont( info.absoluteFilePath() );
}

#include <Note/completer.h>
static void loadDictionaries()
{
    Completer * completer = new Completer();
    foreach( const QFileInfo & info, QDir( getDictionariesPath() ).entryInfoList( QDir::Files ) )
    {
        Dictionary * dictionary = new Dictionary();
        dictionary->load( info.filePath() );

        completer->addDictionary( dictionary );
    }
}

#include <QThreadPool>
static void initThreadCount()
{
    const int threadCount = QThreadPool::globalInstance()->maxThreadCount();
    QThreadPool::globalInstance()->setMaxThreadCount( qMax( 4, 2 * threadCount ) );
}


//! Функция возвращает путь до папки с заметками
static QString getNotesPath()
{
    return QDir::toNativeSeparators( qApp->applicationDirPath() + "/notes" );
}
//! Функция возвращает путь до папки с удаленными заметками
static QString getTrashPath()
{
    return QDir::toNativeSeparators( qApp->applicationDirPath() + "/trash" );
}

//! Функция возвращает формат файла заметки
static QString getNoteFormat()
{
    return "filenotes";
}


static bool removePath( const QString & path )
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


#include <windows.h>
static QString getTheCurrentLanguageKeyboardLayouts()
{
    static const int hexKeyLanguageRussian = 0x419;
    static const int hexKeyLanguageEnglish = 0x409;

    QString name = QApplication::keyboardInputLocale().name();
#ifdef Q_WS_WIN
    switch(LOWORD(GetKeyboardLayout(0)))
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

#endif // FUNC_H
