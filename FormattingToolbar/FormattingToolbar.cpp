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
#include <QWidgetAction>
#include "DialogInsertHyperlink.h"
#include "utils/func.h"
#include "dialoginserthorizontalline.h"

FormattingToolbar::FormattingToolbar( QWidget * parent ) :
    QWidget( parent ),
    ui( new Ui::FormattingToolbar ),
    editor(0),
    note(0),
    alterActivityComponents( false )
{
    ui->setupUi( this );

    // Инициализируем комбо бокс
    foreach ( int size, QFontDatabase::standardSizes() )
        ui->fontSize->addItem( QString::number( size ) );

    int pointSize = QApplication::font().pointSize();
    int currentIndex = ui->fontSize->findText( QString::number( pointSize ) );
    ui->fontSize->setCurrentIndex( currentIndex );

    QButtonGroup * group = new QButtonGroup( this );
    group->setExclusive( true );
    group->addButton( ui->alignCenter );
    group->addButton( ui->alignJustify );
    group->addButton( ui->alignLeft );
    group->addButton( ui->alignRight );

    QObject::connect( ui->textColor, SIGNAL( selectedColor(QColor) ), SLOT( textColor(QColor) ) );
    QObject::connect( ui->colorBackground, SIGNAL( selectedColor(QColor) ), SLOT( backgroundColor(QColor) ) );
    QObject::connect( ui->colorBackground, SIGNAL( clearBackground() ), SLOT( clearBackgroundColor() ) );
    QObject::connect( ui->bulletedList, SIGNAL( selected(int) ), SLOT( list(int) ) );
    QObject::connect( ui->orderedList, SIGNAL( selected(int) ), SLOT( list(int) ) );
    QObject::connect( ui->underline, SIGNAL( selected(int) ), SLOT( underline(int) ) );
    QObject::connect( ui->underline, SIGNAL( selected(QColor) ), SLOT( colorUnderline(QColor) ) );
    QObject::connect( ui->insertTable, SIGNAL( selected(int,int) ), SLOT( insertTable(int,int) ) );

    // Контекстное меню для кнопки вставки горизонтальной линии
    {
        QMenu * menuHLine = new QMenu();
        DialogInsertHorizontalLine * dialogInsertHorizontalLine = new DialogInsertHorizontalLine();
        QObject::connect( dialogInsertHorizontalLine, SIGNAL(sg_insertLine(QString)), SLOT(insertHLine(QString)) );
        QObject::connect( dialogInsertHorizontalLine, SIGNAL(sg_insertLine(QString)), menuHLine, SLOT(hide()) );

        QVBoxLayout * layout = new QVBoxLayout();
        layout->setMargin(0);
        layout->addWidget( dialogInsertHorizontalLine, 0, Qt::AlignCenter );

        menuHLine->setLayout( layout );

        ui->insertHLine->setMenu( menuHLine );
    }

    updateStates();
}

FormattingToolbar::~FormattingToolbar()
{
    delete ui;
}

void FormattingToolbar::installConnect( QTextEdit * editor )
{
    this->editor = editor;

    QObject::connect( editor, SIGNAL( currentCharFormatChanged( QTextCharFormat ) ), SLOT( currentCharFormatChanged(QTextCharFormat) ) );
    QObject::connect( editor, SIGNAL( cursorPositionChanged() ), SLOT( cursorPositionChanged() ) );
    QObject::connect( editor, SIGNAL( selectionChanged() ), SLOT( updateStates() ) );
    QObject::connect( editor->document(), SIGNAL(contentsChanged() ), SLOT( updateStates() ) );

    currentCharFormatChanged( editor->currentCharFormat() );

    // Добавление в редактор функциональности панели форматирования
    {
        QObject::connect( ui->actionBold, SIGNAL(triggered(bool)), SLOT(on_bold_clicked(bool)) );
        QObject::connect( ui->actionItalic, SIGNAL(triggered(bool)), SLOT(on_italic_clicked(bool)) );

        QMenu * menu = this->editor->createStandardContextMenu();
        menu->addSeparator();
        menu->addAction( ui->actionBold );
        menu->addAction( ui->actionItalic );

        this->editor->setContextMenuPolicy( Qt::ActionsContextMenu );
        this->editor->addActions( menu->actions() );
    }

    updateStates();
}

void FormattingToolbar::setNote( RichTextNote * n )
{
    note = n;
}

QToolBar * FormattingToolbar::mainToolBar()
{
    QToolBar * toolBar = new QToolBar( tr( "Formatting" ) );
    toolBar->setObjectName( "Formatting" );
    toolBar->addWidget( ui->font );
    toolBar->addWidget( ui->fontSize );
    toolBar->addWidget( ui->increaseSizeFont );
    toolBar->addWidget( ui->decreaseSizeFont );

    toolBar->addWidget( ui->bold );
    toolBar->addWidget( ui->italic );
    toolBar->addWidget( ui->textColor );
    toolBar->addWidget( ui->colorBackground );
    toolBar->addWidget( ui->subScript );
    toolBar->addWidget( ui->superScript );

    toolBar->addWidget( ui->alignLeft );
    toolBar->addWidget( ui->alignCenter );
    toolBar->addWidget( ui->alignRight );
    toolBar->addWidget( ui->alignJustify );

    toolBar->addWidget( ui->bulletedList );
    toolBar->addWidget( ui->orderedList );

    toolBar->addWidget( ui->underline );
    toolBar->addWidget( ui->strikeout );
    toolBar->addWidget( ui->overline );

    toolBar->addWidget( ui->insertHLine );
    toolBar->addWidget( ui->eraser );
    toolBar->addWidget( ui->lower );
    toolBar->addWidget( ui->upper );

    toolBar->addWidget( ui->insertHyperlink );
    toolBar->addWidget( ui->insertPicture );
    toolBar->addWidget( ui->insertTable );
    return toolBar;
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
    if ( !editor )
        return;

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
    if ( !editor )
        return;

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

        ui->fontSize->setEnabled( false );
        ui->font->setEnabled( false );
        return;
    }

    bool hasSelection = editor->textCursor().hasSelection();
    bool isEmpty = editor->toPlainText().isEmpty();

    ui->bulletedList->setEnabled( true );
    ui->insertHLine->setEnabled( true );
    ui->insertHyperlink->setEnabled( true );
    ui->insertPicture->setEnabled( true );
    ui->insertTable->setEnabled( true );
    ui->orderedList->setEnabled( true );

    ui->alignCenter->setEnabled( !isEmpty );
    ui->alignLeft->setEnabled( !isEmpty );
    ui->alignRight->setEnabled( !isEmpty );
    ui->alignJustify->setEnabled( !isEmpty );

    ui->fontSize->setEnabled( hasSelection );
    ui->font->setEnabled( hasSelection );    
    ui->bold->setEnabled( hasSelection );
    ui->actionBold->setEnabled( hasSelection );
    ui->italic->setEnabled( hasSelection );
    ui->actionItalic->setEnabled( hasSelection );
    ui->colorBackground->setEnabled( hasSelection );
    ui->decreaseSizeFont->setEnabled( hasSelection );
    ui->eraser->setEnabled( hasSelection );
    ui->increaseSizeFont->setEnabled( hasSelection );    
    ui->lower->setEnabled( hasSelection );
    ui->overline->setEnabled( hasSelection );
    ui->strikeout->setEnabled( hasSelection );
    ui->subScript->setEnabled( hasSelection );
    ui->superScript->setEnabled( hasSelection );
    ui->textColor->setEnabled( hasSelection );
    ui->underline->setEnabled( hasSelection );
    ui->upper->setEnabled( hasSelection );
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
    if ( !editor )
        return;

    const QTextCursor & textCursor = editor->textCursor();

    alignmentChanged( editor->alignment() );
    fontChanged( textCursor.charFormat().font() );

    if ( textCursor.currentList() )
    {
        QTextListFormat::Style style = textCursor.currentList()->format().style();
        bool isBulleted = ui->bulletedList->checkedButton( style );
        bool isOrdered = ui->orderedList->checkedButton( style );

        ui->bulletedList->setChecked( isBulleted );
        ui->orderedList->setChecked( isOrdered );
    }
}
void FormattingToolbar::fontChanged( const QFont &font )
{
    ui->font->setCurrentIndex( ui->font->findText( QFontInfo( font ).family() ) );
    ui->fontSize->setCurrentIndex( ui->fontSize->findText( QString::number( font.pointSize() ) ) );
    ui->bold->setChecked( font.bold() );
    ui->actionBold->setChecked( font.bold() );
    ui->italic->setChecked( font.italic() );
    ui->actionItalic->setChecked( font.italic() );
    ui->underline->setChecked( font.underline() );
    ui->strikeout->setChecked( font.strikeOut() );
    ui->overline->setChecked( font.overline() );
}
void FormattingToolbar::colorChanged( const QColor & color)
{
    ui->textColor->setColor( color );
    ui->underline->setColor( color );
}
void FormattingToolbar::colorBackgroundChanged( const QColor & color )
{
    ui->colorBackground->setColor( color );
}
void FormattingToolbar::alignmentChanged( Qt::Alignment align )
{
    ui->alignLeft->setChecked( false );
    ui->alignCenter->setChecked( false );
    ui->alignRight->setChecked( false );
    ui->alignJustify->setChecked( false );

    if ( align == Qt::AlignLeft )
        ui->alignLeft->setChecked( true );

    else if (align == Qt::AlignHCenter )
        ui->alignCenter->setChecked( true );

    else if (align == Qt::AlignRight )
        ui->alignRight->setChecked( true );

    else if (align == Qt::AlignJustify )
        ui->alignJustify->setChecked( true );
}
void FormattingToolbar::verticalAlignmentChanged( QTextCharFormat::VerticalAlignment vAlign )
{
    ui->subScript->setChecked( vAlign == QTextCharFormat::AlignSubScript );
    ui->superScript->setChecked( vAlign == QTextCharFormat::AlignSuperScript );
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

void FormattingToolbar::on_font_activated(const QString &arg1)
{
    QTextCharFormat textCharFormat;
    textCharFormat.setFontFamily( arg1 );
    mergeFormatOnWordOrSelection( textCharFormat );
}
void FormattingToolbar::on_fontSize_activated(const QString &arg1)
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
void FormattingToolbar::on_increaseSizeFont_clicked()
{
    int indexLast = ui->fontSize->count() - 1;
    int indexCurrent = ui->fontSize->currentIndex();

    if ( indexCurrent == indexLast )
        return;

    indexCurrent++;
    on_fontSize_activated( ui->fontSize->itemText( indexCurrent ) );
}
void FormattingToolbar::on_decreaseSizeFont_clicked()
{
    int indexCurrent = ui->fontSize->currentIndex();

    if ( indexCurrent == 0 )
        return;

    indexCurrent--;
    on_fontSize_activated( ui->fontSize->itemText( indexCurrent ) );
}
void FormattingToolbar::on_bold_clicked(bool checked)
{
    QTextCharFormat textCharFormat;
    textCharFormat.setFontWeight( checked ? QFont::Bold : QFont::Normal );
    mergeFormatOnWordOrSelection( textCharFormat );
}
void FormattingToolbar::on_italic_clicked(bool checked)
{
    QTextCharFormat textCharFormat;
    textCharFormat.setFontItalic( checked );
    mergeFormatOnWordOrSelection( textCharFormat );
}
void FormattingToolbar::on_subScript_clicked(bool checked)
{
    changeVerticalAlignment( checked ? QTextCharFormat::AlignSubScript : QTextCharFormat::AlignNormal );
}
void FormattingToolbar::on_superScript_clicked(bool checked)
{
    changeVerticalAlignment( checked ? QTextCharFormat::AlignSuperScript : QTextCharFormat::AlignNormal );
}
void FormattingToolbar::on_strikeout_clicked(bool checked)
{
    QTextCharFormat textCharFormat;
    textCharFormat.setFontStrikeOut( checked );
    mergeFormatOnWordOrSelection( textCharFormat );
}
void FormattingToolbar::on_overline_clicked(bool checked)
{
    QTextCharFormat textCharFormat;
    textCharFormat.setFontOverline( checked );
    mergeFormatOnWordOrSelection( textCharFormat );
}
void FormattingToolbar::on_lower_clicked()
{
    changeCaseSensitive( Lower );
}
void FormattingToolbar::on_upper_clicked()
{
    changeCaseSensitive( Upper );
}

void FormattingToolbar::insertHLine( const QString & width )
{
    if ( !editor )
        return;

    editor->textCursor().insertHtml( QString("<hr width=\"%1\">").arg( width ) + "<br>" );
}
void FormattingToolbar::on_insertHLine_clicked()
{
    insertHLine( "100%" );
}
void FormattingToolbar::on_alignLeft_clicked()
{
    if ( !editor )
        return;

    editor->setAlignment( Qt::AlignLeft | Qt::AlignAbsolute );
}
void FormattingToolbar::on_alignCenter_clicked()
{
    if ( !editor )
        return;

    editor->setAlignment( Qt::AlignHCenter );
}
void FormattingToolbar::on_alignRight_clicked()
{
    if ( !editor )
        return;

    editor->setAlignment( Qt::AlignRight | Qt::AlignAbsolute );
}
void FormattingToolbar::on_alignJustify_clicked()
{
    if ( !editor )
        return;

    editor->setAlignment( Qt::AlignJustify );
}
void FormattingToolbar::on_eraser_clicked()
{
    if ( !editor )
        return;

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
    if ( !editor )
        return;

    QTextCursor cursor = editor->textCursor();
    QTextListFormat::Style listStyle = (QTextListFormat::Style)style;    

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

void FormattingToolbar::on_insertHyperlink_clicked()
{
    if ( !editor )
        return;

    DialogInsertHyperlink dialog( this );
    dialog.setTextHyperlink( editor->textCursor().selectedText() );
    if ( !dialog.exec() )
        return;

    const QString & hyperlink = dialog.getHyperlink();
    const QString & text = dialog.getTextHyperlink();

    editor->insertHtml( QString( "<a href=\"%1\">%2</a> " ).arg( hyperlink ).arg( text ) );
}
void FormattingToolbar::on_insertPicture_clicked()
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

    note->insertImage( fileName, editor->textCursor() );
}
void FormattingToolbar::insertTable( int rows, int cols )
{
    if ( !editor )
        return;

    QTextTableFormat textTableFormat;
    // Сделаем столбцы одинаковой ширины и их размер будет зависеть от размера
    // текстового поля
    QVector < QTextLength > lenghts;
    for ( int column = 0; column < cols; column++ )
        lenghts << QTextLength( QTextLength::PercentageLength, 100.0 / cols );
    textTableFormat.setColumnWidthConstraints( lenghts );

    editor->textCursor().insertTable( rows, cols, textTableFormat );
}
