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
#include <QTimer>
#include <QTextFragment>

class RichTextNote;

//! Редактор заметки
class TextEditor: public QTextBrowser
{
    Q_OBJECT

public:
    TextEditor( QWidget * parent = 0 );

    void setNote( RichTextNote * note );
    RichTextNote * getNote();

private:
    QTextFragment findFragmentAtPos(QPoint pos);

public slots:
    void openUrl(QUrl url);

    void sl_FollowLinkAction();
    void sl_RemoveLinkAction();
    void sl_EditLinkAction();

private:
    RichTextNote * note;
    QTimer anchorTooltipTimer;

private slots:
    void sl_AnchorTooltipTimer_Timeout();

signals:
    void sg_LinkClicked( QUrl );

protected:
    void insertFromMimeData(const QMimeData *source);
    bool canInsertFromMimeData(const QMimeData *source) const;

    void focusInEvent( QFocusEvent * event );
    void changeEvent( QEvent * event );
    void contextMenuEvent( QContextMenuEvent * event );
    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
};

#endif
