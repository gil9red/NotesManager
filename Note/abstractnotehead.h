#ifndef ABSTRACTNOTEHEAD_H
#define ABSTRACTNOTEHEAD_H

#include <QWidget>
#include <QLabel>
#include <QString>
#include <QColor>
#include <QFont>
#include <QPoint>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QLineEdit>

class d_AbstractHead;

#include "NoteShared.h"

//! Класс реализует голову (шапку) абстрактной заметки.
/*! Этот класс отображает название заметки и также через него
 *  возможно перемещение заметки по экрану
 */
class AbstractNoteHead: public QWidget
{
    Q_OBJECT

public:
    AbstractNoteHead( QWidget * parent = 0 );
    ~AbstractNoteHead();

    //! Установка заголовка.
    /*! \param str текст
     *  \sa title()
     */
    void setTitle( const QString & str );

    //! Возвращение заголовка.
    /*! \sa setTitle() */
    QString title();

    //! Установка шрифта заголовка.
    /*! \param f класс шрифта
     *  \sa font()
     */
    void setFont( const QFont & f );

    //! Возвращение шрифта заголовка.
    /*! \sa setFont() */
    QFont font();

    //! Устанавливает цвет.
    /*! \sa color() */
    void setColor( const QColor & color );

    //! Возвращение цвета.
    /*! \sa setColor() */
    QColor color();

public:
    d_AbstractHead * d; //!< Дата
    QLabel * label;     //!< Класс, который отображает текст заголовка

protected:
    void paintEvent( QPaintEvent * );
    void mouseDoubleClickEvent(QMouseEvent *);
    void mousePressEvent( QMouseEvent * event );
    void mouseReleaseEvent( QMouseEvent *  );
    void mouseMoveEvent( QMouseEvent * event );

signals:
    void doubleClick();
};


//! Класс данных шапки абстрактной заметки
/*! \sa AbstractNoteHead, d_AbstractNote */
class d_AbstractHead
{
public:
    QColor color;       //!< Цвет
    QPoint oldPosition; //!< Старая позиция
    bool isMousePress;  //!< Текущее состояние левой кнопки мыши
};

#endif // ABSTRACTNOTEHEAD_H
