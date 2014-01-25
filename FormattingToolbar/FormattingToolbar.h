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

    //! Функция создает и возвращает список панелей инструментов.
    /*! На панелях инструментов размещены все виджеты панели форматирования. */
    QList < QToolBar * > toolBars();

private:
    Ui::FormattingToolbar * ui; //!< Форма UI
    QTextEdit * editor;  //!< Указатель на текстовый редактор, с которым связана панель.
    RichTextNote * note; //!< Указатель на заметку, с коротой связана панель.

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
    void on_fontComboBox_activated(const QString &arg1);

    //! Изменение размера шрифта текста
    void on_comboBoxFontSize_activated(const QString &arg1);

    //! Увеличение размера шрифта текста
    void on_tButtonIncreaseSizeFont_clicked();

    //! Уменьшение размера шрифта текста
    void on_tButtonDecreaseSizeFont_clicked();

    //! Выделить текст жирным
    void on_tButtonBold_clicked(bool checked);

    //! Выделить текст курсивом
    void on_tButtonItalic_clicked(bool checked);

    //! Текст в нижний индекс
    void on_tButtonSubScript_clicked(bool checked);

    //! Текст в верхний индекс
    void on_tButtonSuperScript_clicked(bool checked);

    //! Зачеркивание текста
    void on_tButtonStrikeout_clicked(bool checked);

    //! Надчеркивание текста
    void on_tButtonOverline_clicked(bool checked);

    //! Текст в нижний регистр
    void on_tButtonLower_clicked();

    //! Текст в верхний регистр
    void on_tButtonUpper_clicked();

    //! Вставка горизонтальной линии
    void on_tButtonInsertHLine_clicked();

    //! Выравнивание по левому краю
    void on_tButtonAlignLeft_clicked();

    //! Выравнивание по центру
    void on_tButtonAlignCenter_clicked();

    //! Выравнивание по правому краю
    void on_tButtonAlignRight_clicked();

    //! Выравнивание по ширине
    void on_tButtonAlignJustify_clicked();

    //! Очищение текста от форматирования
    void on_tButtonEraser_clicked();

    //! Подчеркивание текста
    void underline( int style );

    //! Цвет подчеркивающей линии
    void colorUnderline( const QColor & color);

    //! Создание списка
    void list( int style );

    //! Вставка гиперссылки
    void on_tButtonInsertHyperlink_clicked();

    //! Вставка изображения
    void on_tButtonInsertPicture_clicked();

    //! Вставка таблицы
    void insertTable( int rows, int cols );
};

#endif // FORMATINGPANEL_H
