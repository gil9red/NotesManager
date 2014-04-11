#include "scriptengine.h"

using namespace Script;

ScriptEngine * ScriptEngine::self = 0;

ScriptEngine::ScriptEngine( QObject * parent )
    : QScriptEngine( parent )
{
    if ( !self )
        self = this;
}
ScriptEngine::~ScriptEngine()
{
    self = 0;
}

ScriptEngine * ScriptEngine::instance()
{
    if( !self )
        self = new ScriptEngine();
    return self;
}
