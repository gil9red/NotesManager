#ifndef Find_Highlighter_H
#define Find_Highlighter_H

#include <QSyntaxHighlighter>
#include "findshared.h"

//! Подсвечивает обнаруженные совпадения, при заданной строке поиска и флагов.
class Find_Highlighter: public QSyntaxHighlighter
{
    Q_OBJECT

public:
    //! Принимает в качестве обязательного параметра указатель на документ.
    Find_Highlighter( QTextDocument * document );

    //! Указание текста, который будем искать и флагов.
    void setHighlightText(QString text, Shared::FindFlags flags );

    //! Цвет подсветки.
    void setHighlightColor( const QColor & color );

private:
    QString expression;          //!< Поисковое выражение
    QTextCharFormat format;      //!< Правила подсветки
    Shared::FindFlags findFlags; //!< Поисковые флаги

public slots:    
    void clearHighlight(); //! Очистить от подсветки.

protected:
    //! Вызов подсветки для текущего блока.
    void highlightBlock( const QString & text );
};

#endif
