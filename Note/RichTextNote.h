#ifndef NOTE_H
#define NOTE_H

#include "abstractnote.h"
#include "texteditor.h"
#include "AttachPanel.h"

#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QEvent>

#include "quickfind.h"
#include "findandreplace.h"
#include "utils/func.h"

class d_RichTextNote;

//! Заметка с редактором rich-текста.
class RichTextNote: public AbstractNote
{
    Q_OBJECT

public:
    //! Конструктор.
    /*! \param path путь к файлу заметки
     *  \param parent указатель на родительский виджет
     */
    RichTextNote( const QString & path, QWidget * parent = 0 );

    /*! Перегруженный конструктор. */
    RichTextNote( QWidget * parent = 0 );
    ~RichTextNote();

    QTextDocument * document(); //!< Возврат указателя на модель-документ заметки.
    TextEditor * textEditor();  //!< Возврат указателя на текстовый редактор заметки.

    QDateTime created();  //!< Возврат даты создания заметки.
    QDateTime modified(); //!< Возврат даты последнего изменения заметки.

    //! Функция создает заметке папку и нужные для функционирования файлы.
    /*! \param bsave если true, тогда после инициализации значениями
     *  по умолчанию, заметка сделает сохранение.
     *  \sa save(), load()
     */
    void createNew( bool bsave = true );

    QString fileName();         //!< Функция возвращает путь до папки заметки.
    QString attachDirPath();    //!< Функция возвращает путь до папки прикрепленных файлов.
    QString contentFilePath();  //!< Функция возвращает путь до файла содержимого заметки.
    QString settingsFilePath(); //!< Функция возвращает путь до настроек заметки.

    //! В функцию передается путь до папки заметки.
    void setFileName(const QString & dirPath );

    static void setDefaultSettingsFromMap( const QVariantMap & s );

private:
    void init();         //!< Инициализация заметки.
    void setupActions(); //!< Установка действий.
    void setupGUI();     //!< Установка графического интерфейса.

public slots:
    void save();        //!< Сохранение заметки.
    void load();        //!< Загрузка заметки.
    void saveContent(); //!< Сохранение только содержимого заметки.
    void loadContent(); //!< Загрузка только содержимого заметки.

    //! Установка текста заметки.
    void setText( const QString & str );

    //! Возврат текста заметки.
    QString text();

    //! Удаление папки заметки.
    /*! \sa remove() */
    void removeDir();

    //! Удаление папки заметки, закрытие и освобождение памяти.
    /*! \sa removeDir() */
    void remove();

    //! Вызов события "Удалить".
    void invokeRemove();

    //! Устанавливает заметку поверх всех окон.
    /*! \sa isTop(), AbstractNote::setTop() */
    void setTop( bool b );

    void selectTitle();      //!< Вызов диалога выбора заголовка.
    void selectTitleFont();  //!< Вызов диалога выбора шрифта заголовка.
    void selectTitleColor(); //!< Вызов диалога выбора цвета заголовка (шапки).
    void selectColor();      //!< Вызов диалога выбора цвета тела.
    void selectOpacity();    //!< Вызов диалога выбора прозрачности.
    void saveAs();           //!< Вызов диалога сохранения заметки.
    void open();             //!< Вызов диалога открытия заметки.
    void print();            //!< Вызов диалога печати.
    void previewPrint();     //!< Вызов диалога предпросмотра перед печатью.
    void duplicate();        //!< Вызов создания дупликата.
    void selectAttach();     //!< Вызов выбора прикрепленного файла.

    //! Функция вставки изображения в заметку.
    /*! Файл вставляемого изображения будет скопирован в папку прикрепленных файлов.
     *  \param fileName путь до файла изображения.
     */
    void insertImage( const QString & fileName );

    //! Функция вставки изображения в заметку.
    /*! Перегруженная функция.
     *  \param pixmap ссылка на класс-изображение.
     */
    void insertImage( const QPixmap & pixmap );

    //! Функция копирует указанный файл в папку прикрепленных файлов.
    /*! \param fileName путь до файла.
     *  \return путь до файла из папки прикрепленных файлов, если удачно, иначе - пустую строку.
     */
    QString attach( const QString & fileName );

    //! Функция обновляет список прикрепленных файлов.
    void updateAttachList();

    //! Функция возвращает количество прикрепленных файлов.
    int numberOfAttachments();


    //! Устанавливает активность таймера автосохранения .
    /*! \sa isActivateTimerAutosave() */
    void setActivateTimerAutosave( bool activate );

    //! Возвращение true, если таймер автосохранения запущен, иначе false.
    /*! \sa setActivateTimerAutosave() */
    bool isActivateTimerAutosave();

    //! Устанавливает интервал таймера автосохранения.
    /*! \param minutes значение интервала таймера в минутах
     *  \sa intervalAutosave()
     */
    void setIntervalAutosave( quint64 minutes );

    //! Функция возвращает интервал срабатывания таймера в минутах.
    /*! \sa setIntervalAutosave() */
    quint64 intervalAutosave();

private slots:
    //! Слот вызывают действия меню прозрачности
    void changeOpacity(QAction *action);

    //! Вызов печати содержимого редактора заметки
    void print( QPrinter * printer );

    //! Обновление состояния
    void updateStates();

    //! Срабатывает, когда происходит изменение содержимого редактора
    void contentsChanged();

    //! Срабатывает, когда происходит двойной клик на заголовок
    void doubleClickingOnTitle();

public:
    d_RichTextNote * d; //!< Данные
    QToolButton * tButtonSetTopBottom;
    QToolButton * tButtonSave;

    QAction * actionSetTopBottom;
    QAction * actionSave;
    QAction * actionVisibleToolBar;

    QDockWidget * dockWidgetFormattingToolbar;
    QToolButton * tButtonVisibleFormattingToolbar;
    QAction * actionVisibleFormattingToolbar;

    QDockWidget * dockWidgetFindAndReplace;
    FindAndReplace * findAndReplace;
    QToolButton * tButtonVisibleFindAndReplace;
    QAction * actionVisibleFindAndReplace;

    QuickFind * quickFind;
    QAction * actionVisibleQuickFind;

    QDockWidget * dockWidgetAttachPanel;
    AttachPanel * attachPanel;
    QStandardItemModel attachModel;
    QToolButton * tButtonVisibleAttachPanel;
    QAction * actionVisibleAttachPanel;

    static QVariantMap defaultMapSettings; //!< Карта с дэфолтными настройками для заметок.

public:
    friend class AttachPanel;

protected:
    void enterEvent( QEvent * );
    void leaveEvent( QEvent * );
};

//! Класс данных rich-text заметки.
/*! \sa RichTextNote, d_AbstractNote */
class d_RichTextNote: public d_AbstractNote
{
public:
    QTimer timerAutosave; //!< Таймер автосохранений
    QString noteFileName; //! Путь до папки заметки
    TextEditor editor;  //! Редактор заметки
};

Q_DECLARE_METATYPE( RichTextNote * )

#endif // NOTE_H
