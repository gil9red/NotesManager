#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

#include <QScriptEngine>

namespace Script
{
    class ScriptEngine : public QScriptEngine
    {
        Q_OBJECT

    public:
        explicit ScriptEngine( QObject * parent = 0 );
        ~ScriptEngine();

        static ScriptEngine * instance();

    private:
        static ScriptEngine * self;
    };
}

#endif // SCRIPTENGINE_H
