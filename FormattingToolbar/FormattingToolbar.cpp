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


#include "FormattingToolbar.h"
#include "ui_FormattingToolbar.h"
#include "Note/RichTextNote.h"

#include <QPainter>
#include <QTextList>
#include <QDebug>
#include <QInputDialog>
#include <QFileDialog>
#include <QDesktopServices>
#include <QImageReader>
#include <QUrl>
#include <QTextTable>

#include "DialogInsertHyperlink.h"
#include "utils/func.h"


FormattingToolbar::FormattingToolbar( QWidget * parent ) :
    QWidget( parent ),
    ui( new Ui::FormattingToolbar ),
    editor( 0 ),
    note( 0 ),
    alterActivityComponents( false )
{
    ui->setupUi( this );

    // Инициализируем комбо бокс
    foreach ( int size, QFontDatabase::standardSizes() )
        ui->comboBoxFontSize->addItem( QString::number( size ) );

    int pointSize = QApplication::font().pointSize();
    int currentIndex = ui->comboBoxFontSize->findText( QString::number( pointSize ) );
    ui->comboBoxFontSize->setCurrentIndex( currentIndex );

    QButtonGroup * group = new QButtonGroup( this );
    group->setExclusive( true );
    group->addButton( ui->tButtonAlignCenter );
    group->addButton( ui->tButtonAlignJustify );
    group->addButton( ui->tButtonAlignLeft );
    group->addButton( ui->tButtonAlignRight );

    connect( ui->tButtonTextColor, SIGNAL( selectedColor(QColor) ), SLOT( textColor(QColor) ) );
    connect( ui->tButtonColorBackground, SIGNAL( selectedColor(QColor) ), SLOT( backgroundColor(QColor) ) );
    connect( ui->tButtonColorBackground, SIGNAL( clearBackground() ), SLOT( clearBackgroundColor() ) );
    connect( ui->tButtonBulletedList, SIGNAL( selected(int) ), SLOT( list(int) ) );
    connect( ui->tButtonOrderedList, SIGNAL( selected(int) ), SLOT( list(int) ) );
    connect( ui->tButtonUnderline, SIGNAL( selected(int) ), SLOT( underline(int) ) );
    connect( ui->tButtonUnderline, SIGNAL( selected(QColor) ), SLOT( colorUnderline(QColor) ) );
    connect( ui->tButtonInsertTable, SIGNAL( selected(int,int) ), SLOT( insertTable(int,int) ) );

    updateStates();
}

FormattingToolbar::~FormattingToolbar()
{
    delete ui;
}

void FormattingToolbar::installConnect( QTextEdit * editor )
{
    this->editor = editor;

    connect( editor, SIGNAL( currentCharFormatChanged( QTextCharFormat ) ), SLOT( currentCharFormatChanged(QTextCharFormat) ) );
    connect( editor, SIGNAL( cursorPositionChanged() ), SLOT( cursorPositionChanged() ) );
    connect( editor, SIGNAL( selectionChanged() ), SLOT( updateStates() ) );
    connect( editor->document(), SIGNAL(contentsChanged() ), SLOT( updateStates() ) );

    currentCharFormatChanged( editor->currentCharFormat() );

    updateStates();
}

void FormattingToolbar::setNote( RichTextNote * n )
{
    note = n;
}

QList < QToolBar * > FormattingToolbar::toolBars()
{
    QToolBar * toolBar1 = new QToolBar( tr( "Fonts" ) );
    toolBar1->setObjectName( "FormattingToolbar_Fonts" );
    toolBar1->addWidget( ui->fontComboBox );
    toolBar1->addWidget( ui->comboBoxFontSize );
    toolBar1->addSeparator();
    toolBar1->addWidget( ui->tButtonIncreaseSizeFont );
    toolBar1->addWidget( ui->tButtonDecreaseSizeFont );


    QToolBar * toolBar2 = new QToolBar( tr( "Formatting" ) );
    toolBar2->setObjectName( "FormattingToolbar_Formatting" );
    toolBar2->addWidget( ui->tButtonBold );
    toolBar2->addWidget( ui->tButtonItalic );
    toolBar2->addSeparator();
    toolBar2->addWidget( ui->tButtonTextColor );
    toolBar2->addSeparator();
    toolBar2->addWidget( ui->tButtonSubScript );
    toolBar2->addWidget( ui->tButtonSuperScript );


    QToolBar * toolBar3 = new QToolBar( tr( "Text alignment" ) );
    toolBar3->setObjectName( "FormattingToolbar_TextAlignment" );
    toolBar3->addWidget( ui->tButtonAlignLeft );
    toolBar3->addWidget( ui->tButtonAlignCenter );
    toolBar3->addWidget( ui->tButtonAlignRight );
    toolBar3->addWidget( ui->tButtonAlignJustify );


    QToolBar * toolBar4 = new QToolBar( tr( "List" ) );
    toolBar4->setObjectName( "FormattingToolbar_List" );
    toolBar4->addWidget( ui->tButtonBulletedList );
    toolBar4->addWidget( ui->tButtonOrderedList );


    QToolBar * toolBar5 = new QToolBar( tr( "Strikeout" ) );
    toolBar5->setObjectName( "FormattingToolbar_Strikeout" );
    toolBar5->addWidget( ui->tButtonUnderline );
    toolBar5->addWidget( ui->tButtonStrikeout );
    toolBar5->addWidget( ui->tButtonOverline );


    QToolBar * toolBar6 = new QToolBar( tr( "Others" ) );
    toolBar6->setObjectName( "FormattingToolbar_Others" );
    toolBar6->addWidget( ui->tButtonInsertHLine );
    toolBar6->addWidget( ui->tButtonEraser );
    toolBar6->addSeparator();
    toolBar6->addWidget( ui->tButtonLower );
    toolBar6->addWidget( ui->tButtonUpper );

    return QList < QToolBar * > () << toolBar1 << toolBar2 << toolBar3 << toolBar4 << toolBar5 << toolBar6;
}
void FormattingToolbar::setAlterActivityComponents( bool act )
{
    alterActivityComponents = act;
    updateStates();
}
bool FormattingToolbar::isAlterActivityComponents()
{
    return alterActivityComponents;
}

void FormattingToolbar::mergeFormatOnWordOrSelection( const QTextCharFormat & format )
{
    QTextCursor cursor = editor->textCursor();
    if ( !cursor.hasSelection() )
        cursor.select( QTextCursor::WordUnderCursor );

    cursor.mergeCharFormat( format );
    editor->mergeCurrentCharFormat( format );
}
void FormattingToolbar::changeVerticalAlignment(const QTextCharFormat::VerticalAlignment & vAlign)
{
    QTextCharFormat textCharFormat;
    textCharFormat.setVerticalAlignment( vAlign );
    mergeFormatOnWordOrSelection( textCharFormat );
}
void FormattingToolbar::changeCaseSensitive(CaseSensitive caseSensitive )
{
    QTextCursor cursor = editor->textCursor();

    int startPosition = cursor.selectionStart();
    int endPosition = cursor.selectionEnd();
    int cursorStart = 0;
    int cursorEnd = 0;

    //определяем блок по позиции курсора
    QTextBlock block = editor->document()->findBlock( startPosition );

    while ( true )
    {
        //если блок не существует
        if ( !block.isValid() )
            break;

        //если нач позиция первого символа > endPosition
        if ( block.position() > endPosition )
            break;

        QTextBlock::iterator it;

        //перебираем фрагменты - это кусок текста с одинаковым форматированием
        for ( it = block.begin(); !it.atEnd(); ++it )
        {
            QTextFragment currentFragment = it.fragment();
            if ( !currentFragment.isValid() )
                continue;

            //длина фрагменты - количество символов
            int fragmentLength = currentFragment.length();

            //начальная позиция
            int fragmentStart = currentFragment.position();

            //конечная
            int fragmentEnd = fragmentStart + fragmentLength;

            if ( endPosition < fragmentStart || startPosition > fragmentEnd )
                continue;

            QTextCursor temp( editor->document() );

            //определяем позиции
            if ( startPosition < fragmentStart )
                cursorStart = fragmentStart;
            else
                cursorStart = startPosition;

            if ( endPosition < fragmentEnd )
                cursorEnd = endPosition;
            else
                cursorEnd = fragmentEnd;

            //устанавливаем курсор
            temp.setPosition( cursorStart );

            //сдвигаем курсор и выделяем символы
            temp.movePosition( QTextCursor::Right, QTextCursor::KeepAnchor, cursorEnd - cursorStart );

            //получаем выделенные символы
            QString text = temp.selectedText();

            //взависимости от выбора, меняем регистр у текущего фрагмента
            switch ( caseSensitive )
            {
            case Upper:
                text = text.toUpper();
                break;

            case Lower:
                text = text.toLower();
                break;
            }

            temp.beginEditBlock();
            temp.insertText(text);
            temp.endEditBlock();
        }

        //смотрим следующий блок
        block = block.next();
    }

    // восстанавливаем выделение текста
    cursor = editor->textCursor();
    cursor.setPosition( startPosition );
    cursor.movePosition( QTextCursor::Right, QTextCursor::KeepAnchor, endPosition - startPosition );
    editor->setTextCursor( cursor );
}


void FormattingToolbar::setVisible( bool visible )
{
    emit visibilityChanged( visible );
    QWidget::setVisible( visible );
}

void FormattingToolbar::updateStates()
{
    // Если не нужно, не меняем активность и не делаем проверки.
    if ( !alterActivityComponents )
        return;

    if ( !editor )
    {
        foreach ( QToolButton * button, findChildren < QToolButton * > () )
            button->setEnabled( false );

        ui->comboBoxFontSize->setEnabled( false );
        ui->fontComboBox->setEnabled( false );

        return;
    }

    bool hasSelection = editor->textCursor().hasSelection();
    bool isEmpty = editor->toPlainText().isEmpty();

    ui->tButtonBulletedList->setEnabled( true );
    ui->tButtonInsertHLine->setEnabled( true );
    ui->tButtonInsertHyperlink->setEnabled( true );
    ui->tButtonInsertPicture->setEnabled( true );
    ui->tButtonInsertTable->setEnabled( true );
    ui->tButtonOrderedList->setEnabled( true );

    ui->tButtonAlignCenter->setEnabled( !isEmpty );
    ui->tButtonAlignLeft->setEnabled( !isEmpty );
    ui->tButtonAlignRight->setEnabled( !isEmpty );
    ui->tButtonAlignJustify->setEnabled( !isEmpty );

    ui->comboBoxFontSize->setEnabled( hasSelection );
    ui->fontComboBox->setEnabled( hasSelection );
    ui->tButtonBold->setEnabled( hasSelection );
    ui->tButtonColorBackground->setEnabled( hasSelection );
    ui->tButtonDecreaseSizeFont->setEnabled( hasSelection );
    ui->tButtonEraser->setEnabled( hasSelection );
    ui->tButtonIncreaseSizeFont->setEnabled( hasSelection );
    ui->tButtonItalic->setEnabled( hasSelection );
    ui->tButtonLower->setEnabled( hasSelection );
    ui->tButtonOverline->setEnabled( hasSelection );
    ui->tButtonStrikeout->setEnabled( hasSelection );
    ui->tButtonSubScript->setEnabled( hasSelection );
    ui->tButtonSuperScript->setEnabled( hasSelection );
    ui->tButtonTextColor->setEnabled( hasSelection );
    ui->tButtonUnderline->setEnabled( hasSelection );
    ui->tButtonUpper->setEnabled( hasSelection );
}
void FormattingToolbar::currentCharFormatChanged( QTextCharFormat format )
{
    fontChanged( format.font() );
    colorChanged( format.foreground().color() );
    colorBackgroundChanged( format.background().color() );
    verticalAlignmentChanged( format.verticalAlignment() );
}
void FormattingToolbar::cursorPositionChanged()
{
    alignmentChanged( editor->alignment() );

    if ( editor->textCursor().currentList() )
    {
        QTextListFormat::Style style = editor->textCursor().currentList()->format().style();
        bool isBulleted = ui->tButtonBulletedList->checkedButton( style );
        bool isOrdered = ui->tButtonOrderedList->checkedButton( style );

        ui->tButtonBulletedList->setChecked( isBulleted );
        ui->tButtonOrderedList->setChecked( isOrdered );
    }
}
void FormattingToolbar::fontChanged( const QFont &font )
{
    ui->fontComboBox->setCurrentIndex( ui->fontComboBox->findText( QFontInfo( font ).family() ) );
    ui->comboBoxFontSize->setCurrentIndex( ui->comboBoxFontSize->findText( QString::number( font.pointSize() ) ) );
    ui->tButtonBold->setChecked( font.bold() );
    ui->tButtonItalic->setChecked( font.italic() );
    ui->tButtonUnderline->setChecked( font.underline() );
    ui->tButtonStrikeout->setChecked( font.strikeOut() );
    ui->tButtonOverline->setChecked( font.overline() );
}
void FormattingToolbar::colorChanged( const QColor & color)
{
    ui->tButtonTextColor->setColor( color );
    ui->tButtonUnderline->setColor( color );
}
void FormattingToolbar::colorBackgroundChanged( const QColor & color )
{
    ui->tButtonColorBackground->setColor( color );
}
void FormattingToolbar::alignmentChanged( Qt::Alignment align )
{
    ui->tButtonAlignLeft->setChecked( false );
    ui->tButtonAlignCenter->setChecked( false );
    ui->tButtonAlignRight->setChecked( false );
    ui->tButtonAlignJustify->setChecked( false );

    if ( align == Qt::AlignLeft )
        ui->tButtonAlignLeft->setChecked( true );

    else if (align == Qt::AlignHCenter )
        ui->tButtonAlignCenter->setChecked( true );

    else if (align == Qt::AlignRight )
        ui->tButtonAlignRight->setChecked( true );

    else if (align == Qt::AlignJustify )
        ui->tButtonAlignJustify->setChecked( true );
}
void FormattingToolbar::verticalAlignmentChanged( QTextCharFormat::VerticalAlignment vAlign )
{
    ui->tButtonSubScript->setChecked( vAlign == QTextCharFormat::AlignSubScript );
    ui->tButtonSuperScript->setChecked( vAlign == QTextCharFormat::AlignSuperScript );
}
void FormattingToolbar::textColor( const QColor & color )
{
    QTextCharFormat textCharFormat;
    textCharFormat.setForeground( color );
    mergeFormatOnWordOrSelection( textCharFormat );
}
void FormattingToolbar::clearBackgroundColor()
{
    QTextCharFormat textCharFormat;
    textCharFormat.setBackground( Qt::NoBrush );
    mergeFormatOnWordOrSelection( textCharFormat );
}
void FormattingToolbar::backgroundColor( const QColor & color )
{
    QTextCharFormat textCharFormat;
    textCharFormat.setBackground( color );
    mergeFormatOnWordOrSelection( textCharFormat );
}

void FormattingToolbar::on_fontComboBox_activated(const QString &arg1)
{
    QTextCharFormat textCharFormat;
    textCharFormat.setFontFamily( arg1 );
    mergeFormatOnWordOrSelection( textCharFormat );
}
void FormattingToolbar::on_comboBoxFontSize_activated(const QString &arg1)
{
    bool b;
    qreal pointSize = arg1.toFloat( &b );

    if ( b )
    {
        QTextCharFormat textCharFormat;
        textCharFormat.setFontPointSize( pointSize );
        mergeFormatOnWordOrSelection( textCharFormat );
    }
}
void FormattingToolbar::on_tButtonIncreaseSizeFont_clicked()
{
    int indexLast = ui->comboBoxFontSize->count() - 1;
    int indexCurrent = ui->comboBoxFontSize->currentIndex();

    if ( indexCurrent == indexLast )
        return;

    indexCurrent++;
    on_comboBoxFontSize_activated( ui->comboBoxFontSize->itemText( indexCurrent ) );
}
void FormattingToolbar::on_tButtonDecreaseSizeFont_clicked()
{
    int indexCurrent = ui->comboBoxFontSize->currentIndex();

    if ( indexCurrent == 0 )
        return;

    indexCurrent--;
    on_comboBoxFontSize_activated( ui->comboBoxFontSize->itemText( indexCurrent ) );
}
void FormattingToolbar::on_tButtonBold_clicked(bool checked)
{
    QTextCharFormat textCharFormat;
    textCharFormat.setFontWeight( checked ? QFont::Bold : QFont::Normal );
    mergeFormatOnWordOrSelection( textCharFormat );
}
void FormattingToolbar::on_tButtonItalic_clicked(bool checked)
{
    QTextCharFormat textCharFormat;
    textCharFormat.setFontItalic( checked );
    mergeFormatOnWordOrSelection( textCharFormat );
}
void FormattingToolbar::on_tButtonSubScript_clicked(bool checked)
{
    changeVerticalAlignment( checked ? QTextCharFormat::AlignSubScript : QTextCharFormat::AlignNormal );
}
void FormattingToolbar::on_tButtonSuperScript_clicked(bool checked)
{
    changeVerticalAlignment( checked ? QTextCharFormat::AlignSuperScript : QTextCharFormat::AlignNormal );
}
void FormattingToolbar::on_tButtonStrikeout_clicked(bool checked)
{
    QTextCharFormat textCharFormat;
    textCharFormat.setFontStrikeOut( checked );
    mergeFormatOnWordOrSelection( textCharFormat );
}
void FormattingToolbar::on_tButtonOverline_clicked(bool checked)
{
    QTextCharFormat textCharFormat;
    textCharFormat.setFontOverline( checked );
    mergeFormatOnWordOrSelection( textCharFormat );
}
void FormattingToolbar::on_tButtonLower_clicked()
{
    changeCaseSensitive( Lower );
}
void FormattingToolbar::on_tButtonUpper_clicked()
{
    changeCaseSensitive( Upper );
}
void FormattingToolbar::on_tButtonInsertHLine_clicked()
{
    editor->textCursor().insertHtml( "<hr><br>" );
}
void FormattingToolbar::on_tButtonAlignLeft_clicked()
{
    editor->setAlignment( Qt::AlignLeft | Qt::AlignAbsolute );
}
void FormattingToolbar::on_tButtonAlignCenter_clicked()
{
    editor->setAlignment( Qt::AlignHCenter );
}
void FormattingToolbar::on_tButtonAlignRight_clicked()
{
    editor->setAlignment( Qt::AlignRight | Qt::AlignAbsolute );
}
void FormattingToolbar::on_tButtonAlignJustify_clicked()
{
    editor->setAlignment( Qt::AlignJustify );
}
void FormattingToolbar::on_tButtonEraser_clicked()
{
    editor->setCurrentCharFormat( QTextCharFormat() );
}

void FormattingToolbar::underline( int style )
{
    QTextCharFormat textCharFormat;
    textCharFormat.setUnderlineStyle( (QTextCharFormat::UnderlineStyle)style );
    mergeFormatOnWordOrSelection( textCharFormat );
}
void FormattingToolbar::colorUnderline( const QColor & color)
{
    QTextCharFormat textCharFormat;
    textCharFormat.setUnderlineColor( color );
    mergeFormatOnWordOrSelection( textCharFormat );
}
void FormattingToolbar::list( int style )
{
    QTextListFormat::Style listStyle = (QTextListFormat::Style)style;
    QTextCursor cursor = editor->textCursor();

    cursor.beginEditBlock();

    QTextBlockFormat blockFormat = cursor.blockFormat();
    QTextListFormat listFormat;

    if ( cursor.currentList() )
    {
        listFormat = cursor.currentList()->format();

        if ( listFormat.style() == listStyle )
        {
            listFormat.setStyle( (QTextListFormat::Style)0 );
            cursor.createList( listFormat );
            cursor.endEditBlock();
            return;
        }
    } else
    {
        listFormat.setIndent( blockFormat.indent() + 1 );
        blockFormat.setIndent( 0 );
        cursor.setBlockFormat( blockFormat );
    }

    listFormat.setStyle( listStyle );
    cursor.createList( listFormat );
    cursor.endEditBlock();
}

void FormattingToolbar::on_tButtonInsertHyperlink_clicked()
{
    DialogInsertHyperlink dialog( this );
    if ( !dialog.exec() )
        return;

    const QString & hyperlink = dialog.hyperlink();
    const QString & text = dialog.text();

    editor->insertHtml( QString( "<a href=\"%1\">%2</a> " ).arg( hyperlink ).arg( text ) );
}
void FormattingToolbar::on_tButtonInsertPicture_clicked()
{
    QString filters = tr( "Images" );
    filters += " (";
    foreach ( const QByteArray & format, QImageReader::supportedImageFormats() )
        filters += QString( " *.%1" ).arg( QString( format.toLower() ) );
    filters += " )";

    const QString & dir = QDesktopServices::storageLocation( QDesktopServices::PicturesLocation );
    QString fileName = QFileDialog::getOpenFileName( this, tr( "Select picture" ), dir, filters );

    if ( fileName.isEmpty() )
        return;

    if ( !note )
        return;

    note->insertImage( fileName );
}
void FormattingToolbar::insertTable( int rows, int cols )
{
    QTextTableFormat textTableFormat;
    // Сделаем столбцы одинаковой ширины и их размер будет зависеть от размера
    // текстового поля
    QVector < QTextLength > lenghts;
    for ( int column = 0; column < cols; column++ )
        lenghts << QTextLength( QTextLength::PercentageLength, 100.0 / cols );
    textTableFormat.setColumnWidthConstraints( lenghts );

    editor->textCursor().insertTable( rows, cols, textTableFormat );
}
