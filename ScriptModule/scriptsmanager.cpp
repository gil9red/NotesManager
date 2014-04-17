#include "scriptsmanager.h"
#include "ui_scriptsmanager.h"
#include "scriptengine.h"
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerjavascript.h>
#include <QDebug>
#include "utils/func.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QFuture>
#include <QtConcurrentRun>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTimer>
#include <QMenu>

using namespace Script;

const QString & nameTableScripts = "Scripts";
enum { ID = Qt::UserRole, NAME, CONTENT, BOOKMARK };

QSqlQuery sendQuery( QString query )
{
    QSqlQuery sqlQuery;
    bool successful = sqlQuery.exec( query );
    if( !successful )
        WARNING( qPrintable( QString( query + ", error:" + sqlQuery.lastError().text() ) ) );

    return sqlQuery;
}

ScriptsManager::ScriptsManager( QWidget * parent )
    : QMainWindow( parent ),
      ui( new Ui::ScriptsManager ),
      scriptEngineDebugger( new QScriptEngineDebugger ),
      settings(0),
      menuBookmarkScript( new QMenu( tr( "Bookmarks" ) ) )
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

    // Создание таймера, который сохраняет содержимое скриптов. Фича в том, что он сохраняет только после того, как с последнего изменения содержимого
    // прошло определенное количество времени.
    {
        QTimer * timerSavingDelayedContent = new QTimer( this );
        timerSavingDelayedContent->setSingleShot( true );
        timerSavingDelayedContent->setInterval( 33 );
        QObject::connect( timerSavingDelayedContent, SIGNAL(timeout()), SLOT(sl_ContentScriptChanged()) );
        QObject::connect( ui->editScript, SIGNAL(textChanged()), timerSavingDelayedContent, SLOT(start()) );
    }


    // SQL база данных
    {
        QSqlDatabase database = QSqlDatabase::addDatabase( "QSQLITE" );
        database.setDatabaseName( "Scripts.database" );

        if( !database.open() )
            WARNING( qPrintable( database.lastError().text() ) );

        // Если таблицы в базе нет, создаем ее
        if ( !database.tables().contains( nameTableScripts ) )
            sendQuery( QString( "CREATE TABLE %1("
                                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                "name TEXT, "
                                "content TEXT"
                                ");" ).arg( nameTableScripts ) );
    }

    // Закладки
    {
        menuBookmarkScript->setIcon( QIcon( ":/fugue-icons/book-open-bookmark" ) );
        menuBookmarkScript->setToolTip( menuBookmarkScript->title() );
        menuBookmarkScript->setStatusTip( menuBookmarkScript->title() );
        menuBookmarkScript->setWhatsThis( menuBookmarkScript->title() );
    }

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

void ScriptsManager::read()
{
    ui->scripts->clear();
    listOfModified.clear();

    // Загрузка сценариев
    {
        QSqlQuery query = sendQuery( QString( "SELECT * FROM %1" ).arg( nameTableScripts ) );
        QSqlRecord record = query.record();

        while ( query.next() )
        {
            qApp->processEvents();

            const QString & id = query.value( record.indexOf( "id" ) ).toString();
            const QString & name = query.value( record.indexOf( "name" ) ).toString();
            const QString & content = query.value( record.indexOf( "content" ) ).toString();

            QListWidgetItem * item = new QListWidgetItem();
            item->setText( name );
            item->setFlags( item->flags() | Qt::ItemIsEditable );
            item->setData( ID, id );
            item->setData( NAME, name );
            item->setData( CONTENT, content );

            hash_Id_Item.insert( id, item );

            // Действие, вызов которого запустит выполнение скрипта
            {
                QAction * action = new QAction( item->icon(), name, this );
                QObject::connect( action, SIGNAL(triggered()), SLOT(sl_invokeBookmarkAction()) );
                hash_Item_Action.insert( item, action );

                // Закладки
                {
                    bool isBookmark = query.value( record.indexOf( "isBookmark" ) ).toBool();
                    setBookmarkItem( item, isBookmark );
                }
            }

            ui->scripts->addItem( item );
        }
    }

    sl_UpdateStates();
    qApp->restoreOverrideCursor();
}
void ScriptsManager::write()
{
    sl_SaveScripts();
}

void ScriptsManager::readSettings()
{
    if ( !settings )
    {
        WARNING( "null pointer!" );
        return;
    }

    settings->beginGroup( "ScriptsManager" );
    restoreGeometry( settings->value( "Geometry" ).toByteArray() );
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
    settings->setValue( "Geometry", saveGeometry() );
    settings->setValue( "State", saveState() );
    settings->setValue( "Splitter_Main", ui->splitter->saveState() );
    settings->setValue( "UseDebugger", ui->cBoxUseDebugger->isChecked() );
    settings->endGroup();

    settings->sync();
}

void ScriptsManager::deleteRow( int row )
{
    QListWidgetItem * item = ui->scripts->takeItem( row );
    removeFromListOfModified( item );
    const QString & id = item->data( ID ).toString();

    delete hash_Item_Action.value( item ); // удаление action, связанного с данным элементом
    hash_Item_Action.remove( item );
    hash_Id_Item.remove( id );
    hash_Id_Bookmark.remove( id );

    delete item;
    sendQuery( QString( "DELETE FROM %1 WHERE id = '%2';" ).arg( nameTableScripts ).arg( id ) );

    sl_UpdateStates();
}
void ScriptsManager::addToListOfModified( QListWidgetItem * item )
{
    if ( !item )
    {
        WARNING( "Null pointer!" );
        return;
    }

    if ( listOfModified.contains( item ) )
        return;

    listOfModified.append( item );

    QFont font = item->font();
    font.setBold( true );
    item->setFont( font );

    sl_UpdateStates();
}
void ScriptsManager::removeFromListOfModified( QListWidgetItem * item )
{
    if ( !item )
    {
        WARNING( "Null pointer!" );
        return;
    }

    if ( !listOfModified.contains( item ) )
        return;

    QFont font = item->font();
    font.setBold( false );
    item->setFont( font );

    listOfModified.removeOne( item );

    sl_UpdateStates();
}
void ScriptsManager::setBookmarkItem( QListWidgetItem * item, bool value )
{
    item->setData( BOOKMARK, value );

    QAction * actionBookmark = hash_Item_Action.value( item );
    if ( value )
        menuBookmarkScript->addAction( actionBookmark );
    else
        menuBookmarkScript->removeAction( actionBookmark );

    item->setIcon( QIcon( value ? ":/fugue-icons/script-code--bookmark" : ":/fugue-icons/script-code" ) );
    actionBookmark->setIcon( item->icon() );

    const QString & id = item->data( ID ).toString();
    hash_Id_Bookmark.insert( id, value );
}
void ScriptsManager::runScript( const QString & script )
{
    const QString & result = Script::ScriptEngine::instance()->evaluate( script ).toString();
    ui->lEditLog->setText( result );
}

void ScriptsManager::sl_UpdateStates()
{
    bool hasSelection = ui->scripts->selectionModel()->hasSelection();
    ui->actionRunScript->setEnabled( hasSelection );
    ui->actionDeleteScript->setEnabled( hasSelection );
    ui->actionRenameScript->setEnabled( hasSelection );
    ui->actionBookmarkScript->setEnabled( hasSelection );
    if ( hasSelection )
        ui->actionBookmarkScript->setChecked( ui->scripts->currentItem()->data( BOOKMARK ).toBool() );
    else
        ui->actionBookmarkScript->setChecked( false );

    ui->editScript->setEnabled( hasSelection );
    ui->lEditLog->setEnabled( hasSelection );
    ui->cBoxUseDebugger->setEnabled( hasSelection );

    ui->actionDeleteAllScripts->setEnabled( ui->scripts->count() > 0 );

    ui->actionSaveAllScripts->setEnabled( !listOfModified.isEmpty() );
}
void ScriptsManager::sl_ContentScriptChanged()
{
    QListWidgetItem * item = ui->scripts->currentItem();
    const QString & content = ui->editScript->text();
    item->setData( CONTENT, content );

    addToListOfModified( item );

    sl_UpdateStates();
}
void ScriptsManager::sl_SaveScripts()
{
    qApp->setOverrideCursor( Qt::WaitCursor );

    while( listOfModified.count() > 0 )
    {
        QListWidgetItem * item = listOfModified.first();
        qApp->processEvents();

        const QString & id = item->data( ID ).toString();
        QString name = item->data( NAME ).toString();
        QString content = item->data( CONTENT ).toString();
        bool isBookmark = item->data( BOOKMARK ).toBool();

        {
            QSqlQuery query;
            query.prepare( QString( "UPDATE %1 SET name=:name WHERE id=%2;" ).arg( nameTableScripts ).arg( id ) );
            query.bindValue( ":name", name );
            query.exec();
        }
        {
            QSqlQuery query;
            query.prepare( QString( "UPDATE %1 SET content=:content WHERE id=%2;" ).arg( nameTableScripts ).arg( id ) );
            query.bindValue( ":content", content );
            query.exec();
        }

//        sendQuery( QString( "UPDATE %1 SET content='%2' WHERE id=%3;" ).arg( nameTableScripts ).arg( content.replace( "'", "\'" ) ).arg( id ) );
//        sendQuery( QString( "UPDATE %1 SET name='%2' WHERE id=%3;" ).arg( nameTableScripts ).arg( name.replace( "'", "\\'" ) ).arg( id ) );
        sendQuery( QString( "UPDATE %1 SET isBookmark='%2' WHERE id=%3;" ).arg( nameTableScripts ).arg( isBookmark ).arg( id ) );

        removeFromListOfModified( item );
    }

    qApp->restoreOverrideCursor();
}

void ScriptsManager::on_actionRunScript_triggered()
{
    const QString & script = ui->editScript->text();
    if ( script.isEmpty() )
        return;

    runScript( script );
}
void ScriptsManager::on_actionAddScript_triggered()
{
    QListWidgetItem * item = new QListWidgetItem( QIcon( ":/fugue-icons/script-code" ), tr( "New script" ) );
    item->setFlags( item->flags() | Qt::ItemIsEditable );    
    item->setData( NAME, item->text() );
    item->setData( CONTENT, "" );

    ui->scripts->addItem( item );
    ui->scripts->setCurrentItem( item );

    const QString & name = item->data( NAME ).toString();
    const QString & content = item->data( CONTENT ).toString();
    bool isBookmark = false;
    QSqlQuery query = sendQuery( QString( "INSERT INTO %1(name, content, isBookmark) VALUES('%2', '%3', '%4');").arg( nameTableScripts ).arg( name ).arg( content ).arg( isBookmark ) );
    const QString & id = query.lastInsertId().toString();
    item->setData( ID, id );

    // Действие, вызов которого запустит выполнение скрипта
    {
        QAction * action = new QAction( item->icon(), name, this );
        QObject::connect( action, SIGNAL(triggered()), SLOT(sl_invokeBookmarkAction()) );
        hash_Item_Action.insert( item, action );

        // Закладки
        {
            setBookmarkItem( item, isBookmark );
        }
    }
    hash_Id_Item.insert( id, item );

    removeFromListOfModified( item );

    sl_UpdateStates();
}
void ScriptsManager::on_actionDeleteScript_triggered()
{
    const QString & title = tr( "Confirm deletion" );
    const QString & message = tr( "Delete this script?" );
    bool hasCancel = QMessageBox::question( this, title, message, QMessageBox::Yes | QMessageBox::No ) != QMessageBox::Yes;
    if ( hasCancel )
        return;

    qApp->setOverrideCursor( Qt::WaitCursor );
    int row = ui->scripts->currentRow();
    deleteRow( row );
    qApp->restoreOverrideCursor();
}
void ScriptsManager::on_actionRenameScript_triggered()
{
    QListWidgetItem * item = ui->scripts->currentItem();
    if ( !item )
    {
        WARNING( "Null pointer!" );
        return;
    }

    ui->scripts->editItem( item );
}
void ScriptsManager::on_actionDeleteAllScripts_triggered()
{
    const QString & title = tr( "Confirm deletion" );
    const QString & message = tr( "Delete all scripts?" );
    bool hasCancel = QMessageBox::question( this, title, message, QMessageBox::Yes | QMessageBox::No ) != QMessageBox::Yes;
    if ( hasCancel )
        return;

    while ( ui->scripts->count() > 0 )
    {
        qApp->processEvents();
        deleteRow(0);
    }
    sl_UpdateStates();
}
void ScriptsManager::on_actionSaveAllScripts_triggered()
{
    sl_SaveScripts();
}
void ScriptsManager::on_actionBookmarkScript_triggered( bool checked )
{
    QListWidgetItem * item = ui->scripts->currentItem();
    if ( !item )
    {
        WARNING( "Null pointer!" );
        return;
    }

    setBookmarkItem( item, checked );
}

void ScriptsManager::on_cBoxUseDebugger_clicked( bool checked )
{
    if ( checked )
        scriptEngineDebugger->attachTo( Script::ScriptEngine::instance() );
    else
        scriptEngineDebugger->detach();
}
void ScriptsManager::on_scripts_itemSelectionChanged()
{
    qApp->setOverrideCursor( Qt::WaitCursor );

    ui->editScript->blockSignals( true );
    QListWidgetItem * item = ui->scripts->currentItem();
    if ( item )
    {
        ui->editScript->setText( item->data( CONTENT ).toString() );
        sl_UpdateStates();
    } else
        ui->editScript->clear();
    ui->editScript->blockSignals( false );

    qApp->restoreOverrideCursor();
}
void ScriptsManager::on_scripts_itemChanged( QListWidgetItem * item )
{
    if ( !item )
    {
        WARNING( "Null pointer!" );
        return;
    }

    const QString & name = item->text();
    item->setData( NAME, name );

    QAction * action = hash_Item_Action.value( item );
    if ( action )
    {
        action->setText( name );
        action->setIcon( item->icon() );
    }

    addToListOfModified( item );
}

void ScriptsManager::sl_invokeBookmarkAction()
{
    QAction * action = qobject_cast < QAction * > ( sender() );
    if ( !action )
    {
        WARNING( "Null pointer!" );
        return;
    }

    QListWidgetItem * item = hash_Item_Action.key( action, 0 );
    if ( !item )
    {
        WARNING( "Null pointer!" );
        return;
    }

    runScript( item->data( CONTENT ).toString() );
}
