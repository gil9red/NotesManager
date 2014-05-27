#ifndef FORMATINGPANEL_H
#define FORMATINGPANEL_H

namespace Ui
{
    class FormattingToolbar;
}

#include <QToolBar>
#include <QTextEdit>
#include <QTextCharFormat>

#include <Note/RichTextNote.h>

//! Виджет, реализующий панель форматирования текста.
class FormattingToolbar: public QWidget
{
    Q_OBJECT

    //! Регистр текста.
    enum CaseSensitive
    {
        Upper, //!< Верхний
        Lower  //!< Нижний
    };

public:
    explicit FormattingToolbar( QWidget * parent = 0 );
    ~FormattingToolbar();

    //! Функция создает связь между панелью форматирования и текстовым редактором.
    void installConnect( QTextEdit * editor );

    //! Функция создает связь между панелью форматирования и заметкой.
    void setNote( RichTextNote * n );

    QToolBar * mainToolBar();

    //! Функция устанавливаем изменение активности компонентов панели.
    /*! При true, если не выделен текст и/или нет текста некоторые компоненты могут оказаться неактивными. */
    void setAlterActivityComponents( bool act );

    //! Функция возвращает true, если происходит изменение активности компонентов панели.
    bool isAlterActivityComponents();

private:
    Ui::FormattingToolbar * ui; //!< Форма UI
    QTextEdit * editor;  //!< Указатель на текстовый редактор, с которым связана панель.
    RichTextNote * note; //!< Указатель на заметку, с которой связана панель.
    bool alterActivityComponents; //!< Изменение активности компонентов панели форматирования.

private:
    //! Изменение текстового формата
    void mergeFormatOnWordOrSelection( const QTextCharFormat & format );

    //! Изменение регистра текста
    void changeVerticalAlignment( const QTextCharFormat::VerticalAlignment & vAlign );

    //! Изменение регистра текста
    void changeCaseSensitive( CaseSensitive caseSensitive );

public slots:    
    void setVisible( bool visible ); //!< Установка видимости панели.

signals:
    //! Изменение видимости панели.
    void visibilityChanged( bool );

private slots:    
    void updateStates(); //!< обновление состояния

    //! Слот вызывается при изменении текстового формата под курсором
    void currentCharFormatChanged( QTextCharFormat format );

    //! Слот вызывается при изменении позиции курсором
    void cursorPositionChanged();

    //! Слот вызывается при изменении шрифта
    void fontChanged( const QFont & font );

    //! Слот вызывается при изменении цвета текста
    void colorChanged( const QColor & color );

    //! Слот вызывается при изменении заднего фона текста
    void colorBackgroundChanged( const QColor & color );

    //! Слот вызывается при изменении выравнивания текста
    void alignmentChanged( Qt::Alignment align );

    //! Слот вызывается при изменении вертикального выравнивания текста
    void verticalAlignmentChanged( QTextCharFormat::VerticalAlignment );

    //! Изменение цвета текста
    void textColor( const QColor & color );

    //! Очищение заднего фона текста
    void clearBackgroundColor();

    //! Изменение цвета заднего фона текста
    void backgroundColor( const QColor & color );

    //! Изменение шрифта текста
    void on_font_activated(const QString &arg1);

    //! Изменение размера шрифта текста
    void on_fontSize_activated(const QString &arg1);

    //! Увеличение размера шрифта текста
    void on_increaseSizeFont_clicked();

    //! Уменьшение размера шрифта текста
    void on_decreaseSizeFont_clicked();

    //! Выделить текст жирным
    void on_bold_clicked(bool checked);

    //! Выделить текст курсивом
    void on_italic_clicked(bool checked);

    //! Текст в нижний индекс
    void on_subScript_clicked(bool checked);

    //! Текст в верхний индекс
    void on_superScript_clicked(bool checked);

    //! Зачеркивание текста
    void on_strikeout_clicked(bool checked);

    //! Надчеркивание текста
    void on_overline_clicked(bool checked);

    //! Текст в нижний регистр
    void on_lower_clicked();

    //! Текст в верхний регистр
    void on_upper_clicked();

    //! Вставка горизонтальной линии
    void insertHLine( const QString &width = "100%" );
    void on_insertHLine_clicked();

    //! Выравнивание по левому краю
    void on_alignLeft_clicked();

    //! Выравнивание по центру
    void on_alignCenter_clicked();

    //! Выравнивание по правому краю
    void on_alignRight_clicked();

    //! Выравнивание по ширине
    void on_alignJustify_clicked();

    //! Очищение текста от форматирования
    void on_eraser_clicked();

    //! Подчеркивание текста
    void underline( int style );

    //! Цвет подчеркивающей линии
    void colorUnderline( const QColor & color);

    //! Создание списка
    void list( int style );

    //! Вставка гиперссылки
    void on_insertHyperlink_clicked();

    //! Вставка изображения
    void on_insertPicture_clicked();

    //! Вставка таблицы
    void insertTable( int rows, int cols );
};

#endif // FORMATINGPANEL_H
