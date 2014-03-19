#ifndef UANIMATEDSPLASHSCREEN_H
#define UANIMATEDSPLASHSCREEN_H

#include <QSplashScreen>
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <QMouseEvent>

//! Класс, который создает заставку.
/*! Наследник QSplashScreen, адаптированный под текущий проект. */
class MySplashScreen: public QSplashScreen
{
    Q_OBJECT

public:
    MySplashScreen( const QPixmap & pixmap = QPixmap(), Qt::WindowFlags f = 0 );

private:
    QLabel * labelMessage; //!< Текущая информацию на экранной заставке.
    QPoint oldPosition;    //!< Старая позиция окна.
    bool buttonMousePress; //!< Текущее состояние левой кнопки мыши.

public slots:
    //! Установка сообщения на экранной заставке.
    /*! \param message текст сообщения
     *  \param font шрифт текста
     *  \param color цвет текста
     */
    void setMessage( const QString & message, const QFont & font, const QColor & color = Qt::black );

protected:
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
};

#endif // UANIMATEDSPLASHSCREEN_H
