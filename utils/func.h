#ifndef FUNC_H
#define FUNC_H

#include <QToolButton>
#include <QAction>
#include <QDebug>
#include <QApplication>

#define OUT_MESSAGE "\"%s\": in file \"%s\", func \"%s\", line %i"
#define WARNING( msg ) qWarning( OUT_MESSAGE, msg, __FILE__, __FUNCTION__, __LINE__ );
#define CRITICAL( msg ) qCritical( OUT_MESSAGE, msg, __FUNCTION__, __FILE__, __LINE__ );
#define FATAL( msg ) qFatal( OUT_MESSAGE, msg, __FUNCTION__, __FILE__, __LINE__ );

//! Содержит классы, облегчающие создание объектов, имеющих множество свойств (параметров).
namespace Create
{
    //! Содержит статические методы создания кнопок.
    namespace Button
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
        QToolButton * get( QWidget * parent, const QString & text, const QString & toolTip, const QString & statusTip, const QString & whatsThis, const QIcon & icon, bool checkable = false );

        /*! Перегруженная функция. */
        QToolButton * get( QWidget * parent, const QString & text, const QIcon & icon, bool checkable = false );

        /*! Перегруженная функция. */
        QToolButton * get( const QSize & size, const QSize & iconSize, const QIcon & icon, const bool checkable = true );

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
        QToolButton * clicked( const QString & text, const QString & toolTip, const QString & statusTip, const QString & whatsThis, const QIcon & icon, QWidget * receiver, const char * member );

        /*! Перегруженная функция.
         *  \sa get() и bClicked()
         */
        QToolButton * clicked( const QString & text, QWidget * receiver, const char * member );

        /*! Перегруженная функция.
         *  \sa get() и bClicked()
         */
        QToolButton * clicked( const QString & text, const QIcon & icon, QWidget * receiver, const char * member );

        //! Функция создает кнопку, соединяет ее сигнал clicked(bool) к переданному слоту или сигналу, и возвращает указатель на нее.
        /*! Такая же функция как и clicked, только отправляет сигнал clicked(bool), и является контрольной.
         *  \sa get() и clicked()
         */
        QToolButton * bClicked( const QString & text, const QString & toolTip,const QString & statusTip, const QString & whatsThis, const QIcon & icon, QWidget * receiver, const char * member );

        /*! Перегруженная функция.
         *  \sa get() и clicked()
         */
        QToolButton * bClicked( const QString & text, QWidget * receiver, const char * member );

        /*! Перегруженная функция.
         *  \sa get() и clicked()
         */
        QToolButton * bClicked( const QString & text, const QIcon & icon, QWidget * receiver, const char * member );
    }

    //! Содержит статические методы создания действий.
    namespace Action
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
        QAction * get( QObject * parent, const QString & text, const QString & toolTip, const QString & whatsThis, const QIcon & icon, const QKeySequence & key, bool checkable = false );

        /*! Перегруженная функция. */
        QAction * get( const QString & text, const QIcon & icon, QVariant data );

        /*! Перегруженная функция. */
        QAction * get( const QString & text, const QIcon & icon, bool checkable = false );

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
        QAction * triggered( const QString & text, const QString & toolTip, const QString & whatsThis, const QIcon & icon, const QKeySequence & key, QObject * receiver, const char * member );

        /*! Перегруженная функция. */
        QAction * triggered( const QString & text, QObject * receiver, const char * member );

        /*! Перегруженная функция. */
        QAction * triggered( const QString & text, const QIcon & icon, QObject * receiver, const char * member );

        /*! Перегруженная функция. */
        QAction * triggered( const QIcon & icon,const QString & text, QObject * receiver, const char * member );

        /*! Перегруженная функция. */
        QAction * triggered( QString text, const QIcon & icon, const QKeySequence & key, QObject * receiver, const char * member );

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
        QAction * bTriggered( const QString & text, const QString & toolTip, const QString & whatsThis, const QIcon & icon, const QKeySequence & key, QObject * receiver, const char * member );

        /*! Перегруженная функция. */
        QAction * bTriggered( QString text, QObject * receiver, const char * member );

        /*! Перегруженная функция. */
        QAction * bTriggered( QString text, const QIcon & icon, QObject * receiver, const char * member );

        /*! Перегруженная функция. */
        QAction * bTriggered( QString text, const QIcon & icon, const QKeySequence & key, QObject * receiver, const char * member );
    }
}

void copyPath( const QString & oldPath, const QString & newPath );
bool copyPath2( const QString & oldPath, const QString & newPath );
bool copyPathE( const QString & oldPath, const QString & newPath, bool interruptCopyingWhenError = false, bool logError = true );

QString getPluginsPath();
QString getDocumentationPath();
QString getSettingsPath();
QString getTrPath();
QString getFontsPath();
QString getDictionariesPath();

QString getWindowTitle();

#include <QSettings>
QSettings * createSettings();

void loadFonts();
void loadDictionaries();
void loadTranslations( QSettings * settings );
void initThreadCount();

//! Функция возвращает путь до папки с заметками
QString getNotesPath();

//! Функция возвращает путь до папки с удаленными заметками
QString getTrashPath();

//! Функция возвращает формат файла заметки
QString getNoteFormat();

bool removePath( const QString & path );

QString getTheCurrentLanguageKeyboardLayouts();

#include <QMessageBox>
QMessageBox::StandardButton showNewMessageBox( QWidget * parent, QMessageBox::Icon icon, const QString& title, const QString& text,
                                               QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton,
                                               Qt::WindowFlags f = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowStaysOnTopHint );

//! Обрезание текста, если превышен лимит, то лишний текст обрезается и добавляется троеточие "..."
QString cropString( QString text, int max = 15 );


#include <QDateTime>
//! Функция вернет дату и время сборки
QDateTime getBuildDateTime();

#endif // FUNC_H
