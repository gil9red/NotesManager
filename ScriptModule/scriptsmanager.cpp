#include "scriptsmanager.h"
#include "ui_scriptsmanager.h"

#include "scriptengine.h"

using namespace Script;

ScriptsManager::ScriptsManager( QWidget * parent )
    : QMainWindow( parent ),
      ui( new Ui::ScriptsManager ),
      scriptEngineDebugger( new QScriptEngineDebugger )
{
    ui->setupUi( this );

    QObject::connect( ui->textEditScript, SIGNAL(textChanged()), SLOT(sl_UpdateStates()) );
    sl_UpdateStates();
}
ScriptsManager::~ScriptsManager()
{
    delete ui;
}

void ScriptsManager::sl_UpdateStates()
{
    ui->tButtonRunScript->setEnabled( !ui->textEditScript->toPlainText().isEmpty() );
}
void ScriptsManager::on_tButtonRunScript_clicked()
{
    const QString & script = ui->textEditScript->toPlainText();
    const QString & result = Script::ScriptEngine::instance()->evaluate( script ).toString();
    ui->lEditLog->setText( result );
}

void ScriptsManager::on_cBoxUseScriptDebugger_clicked( bool checked )
{
    if ( checked )
        scriptEngineDebugger->attachTo( Script::ScriptEngine::instance() );
    else
        scriptEngineDebugger->detach();
}
