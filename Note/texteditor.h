#ifndef TextEditor_H
#define TextEditor_H

#include <QTextEdit>
#include <QKeyEvent>
#include <QTextBrowser>
#include <QApplication>
#include <QKeyEvent>
#include <QDebug>

#include "completer.h"
#include "utils/func.h"

//! Редактор заметки
class TextEditor: public QTextBrowser
{
    Q_OBJECT

public:
    TextEditor( QWidget * parent = 0 );

    //! Активация ссылок в редакторе
    void setActivateLink( bool activate );

private slots:
    void openLink( QUrl url ); //!< Открытие ссылки

signals:
    //!< Сигнал отсылается при открытии ссылки
    void isOpenLink( bool );

protected:
    void focusInEvent( QFocusEvent * event );
    void keyPressEvent( QKeyEvent * event );
    void keyReleaseEvent( QKeyEvent * event );
    void changeEvent( QEvent * event );
    void contextMenuEvent( QContextMenuEvent * event );
};

#endif
