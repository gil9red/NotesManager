#ifndef FindAndReplace_H
#define FindAndReplace_H

#include <QTextEdit>
#include <QTimer>

namespace Ui
{
    class FindAndReplace;
}

#include "find_highlighter.h"

//! Виджет поиска и замены текста.
class FindAndReplace: public QWidget
{
    Q_OBJECT

public:
    //! Конструктор.
    /*! \param textedit указатель на текстовый редактор,
     *  в котором и будем делать поиск и замену текста, а также
     *  подсветку найденных строк
     *  \parent указатель на виджет-родитель
     */
    explicit FindAndReplace( QTextEdit * textedit, QWidget * parent = 0 );
    ~FindAndReplace();

    void removeBottomVerticalSpacer();
    void setAutoRaiseNextAndPrevious( bool );

    //! Установка задержки подсветки.
    /*! \param mces значение задержки в миллисекундах */
    void setDelayInterval( int msec );

    //! Значение задержки подсветки.
    int delayInterval();

    void setHighlight( bool );
    bool highlight();

protected:
    //! Форма UI виджета.
    Ui::FindAndReplace * ui;

    //! Указатель на текстовый редактор, с котором работает класс.
    QTextEdit * d_editor;

    //! Класс, реализующий подсветку.
    Find_Highlighter * highlighter;

    //! Таймер задержки подсветки.
    QTimer timer;

    bool d_highlight;

private slots:
    //! Вызывается слот при вводе символов в редактор.
    void on_findEdit_textEdited( const QString & text );

    void on_previous_clicked(); //!< Поиск назад (к началу документа).
    void on_next_clicked();     //!< Поиск вперед (к концу документа).

    void on_replace_clicked();    //!< Замена.
    void on_replaceAll_clicked(); //!< Заменить все.

    //! Функция поиска.
    /*! \param next если true, поиск делается вперед, иначе назад */
    void find( bool next = true );

    void rehighlight();  //!< Вызов подсветки.
    void updateStates(); //!< Обновление состояния класса.

signals:
    //! Вызывается при изменении видимости класса.
    void visibilityChanged( bool );

protected:
    void hideEvent( QHideEvent * );
    void showEvent( QShowEvent * );
};

#endif
