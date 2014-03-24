#ifndef FOO_H
#define FOO_H

#include <QScriptEngine>

namespace ScriptModule
{
    QString evaluate( const QString & script )
    {
        return QScriptEngine().evaluate( script ).toString();
    }
}

#endif // FOO_H
