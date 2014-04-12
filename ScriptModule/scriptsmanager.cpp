#include "scriptsmanager.h"
#include "ui_scriptsmanager.h"
#include "scriptengine.h"
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerjavascript.h>
#include <QDebug>
#include "utils/func.h"

using namespace Script;

ScriptsManager::ScriptsManager( QWidget * parent )
    : QMainWindow( parent ),
      ui( new Ui::ScriptsManager ),
      scriptEngineDebugger( new QScriptEngineDebugger ),
      settings(0)
{
    ui->setupUi( this );

    // Настройка редактора QsciScintilla
    {
        // Создаем и устанавливаем лексер для JavaScript
        ui->editScript->setLexer( new QsciLexerJavaScript( ui->editScript ) );

        // Задаем формат ввода - без этого не будут вводиться русские буквы
        ui->editScript->setUtf8( true );

        // Текущая строка подсвечивается горизонтальной линией
        ui->editScript->setCaretLineVisible( true );

        // Цвет линии
        ui->editScript->setCaretLineBackgroundColor( QColor( "gainsboro" ) );

        // Автоматический отступ
        ui->editScript->setAutoIndent( true );
        ui->editScript->setIndentationGuides( true );

        // Заменять Tab на пробелы
        ui->editScript->setIndentationsUseTabs( false );

        // Ширина отступа - 4 пробела
        ui->editScript->setIndentationWidth(4);

        // Задаем символ конца строки
#if defined Q_WS_X11
        ui->editScript->setEolMode( QsciScintilla::EolUnix );
#elif defined Q_WS_WIN
        ui->editScript->setEolMode( QsciScintilla::EolWindows );
#elif defined Q_WS_MAC
        ui->editScript->setEolMode( QsciScintilla::EolMac );
#endif

        // Задаем цвет вертикальной полосы слева - там где ставятся breakpoints, закладки, номера строк
        ui->editScript->setMarginsBackgroundColor( QColor( "gainsboro" ) );

        // Отображать номера строк
        ui->editScript->setMarginLineNumbers(1, true);

        // Ширина полосы - такая чтобы влезли символы до 1000
        ui->editScript->setMarginWidth( 1, QString( "1000" ) );

        // Автозавершение лексем
        ui->editScript->setAutoCompletionSource( QsciScintilla::AcsAll );
        ui->editScript->setAutoCompletionCaseSensitivity( true );
        ui->editScript->setAutoCompletionReplaceWord( true );
        ui->editScript->setAutoCompletionShowSingle( true );
        ui->editScript->setAutoCompletionThreshold(2);

        // Автоподсветка скобок
        ui->editScript->setBraceMatching( QsciScintilla::SloppyBraceMatch );

        // Цвет подсветки - желный с синим
        ui->editScript->setMatchedBraceBackgroundColor( Qt::yellow );
        ui->editScript->setUnmatchedBraceForegroundColor( Qt::blue );
    }

    QObject::connect( ui->editScript, SIGNAL(textChanged()), SLOT(sl_UpdateStates()) );
    sl_UpdateStates();
}
ScriptsManager::~ScriptsManager()
{
    delete ui;
}

//! В функции указываем с каким классом настроек будем работать.
void ScriptsManager::setSettings( QSettings * s )
{
    settings = s;
}

void ScriptsManager::readSettings()
{
    if ( !settings )
    {
        WARNING( "null pointer!" );
        return;
    }

    settings->beginGroup( "ScriptsManager" );
    restoreState( settings->value( "State" ).toByteArray() );
    ui->splitter->restoreState( settings->value( "Splitter_Main" ).toByteArray() );
    ui->cBoxUseDebugger->setChecked( settings->value( "UseDebugger", false ).toBool() );

    settings->endGroup();
}
void ScriptsManager::writeSettings()
{
    if ( !settings )
    {
        WARNING( "null pointer!" );
        return;
    }

    settings->beginGroup( "ScriptsManager" );
    settings->setValue( "State", saveState() );
    settings->setValue( "Splitter_Main", ui->splitter->saveState() );
    settings->setValue( "UseDebugger", ui->cBoxUseDebugger->isChecked() );
    settings->endGroup();

    settings->sync();
}

void ScriptsManager::sl_UpdateStates()
{
    bool hasSelection = ui->scripts->selectionModel()->hasSelection();
    ui->tButtonRunScript->setEnabled( hasSelection );
    ui->tButtonDeleteScript->setEnabled( hasSelection );
    ui->tButtonRenameScript->setEnabled( hasSelection );

    bool isEmpty = ( ui->scripts->count() == 0 );
    ui->tButtonDeleteAllScripts->setEnabled( !isEmpty );
}

void ScriptsManager::on_tButtonRunScript_clicked()
{
    const QString & script = ui->editScript->text();
    const QString & result = Script::ScriptEngine::instance()->evaluate( script ).toString();
    ui->lEditLog->setText( result );
}

void ScriptsManager::on_cBoxUseDebugger_clicked( bool checked )
{
    if ( checked )
        scriptEngineDebugger->attachTo( Script::ScriptEngine::instance() );
    else
        scriptEngineDebugger->detach();
}
void ScriptsManager::on_tButtonAddScript_clicked()
{
    QListWidgetItem * item = new QListWidgetItem( QIcon( ":/fugue-icons/script-code" ), tr( "New script" ) );
    item->setFlags( item->flags() | Qt::ItemIsEditable );

    ui->scripts->addItem( item );
    ui->scripts->setCurrentItem( item );

    sl_UpdateStates();
}
void ScriptsManager::on_tButtonDeleteScript_clicked()
{
    int row = ui->scripts->currentRow();
    delete ui->scripts->takeItem( row );
    sl_UpdateStates();
}
void ScriptsManager::on_tButtonRenameScript_clicked()
{
    ui->scripts->editItem( ui->scripts->currentItem() );
}
void ScriptsManager::on_tButtonDeleteAllScripts_clicked()
{
    qApp->setOverrideCursor( Qt::WaitCursor );

    while ( ui->scripts->count() > 0 )
        delete ui->scripts->takeItem(0);

    qApp->restoreOverrideCursor();

    sl_UpdateStates();
}

void ScriptsManager::on_scripts_itemSelectionChanged()
{
    sl_UpdateStates();
}
