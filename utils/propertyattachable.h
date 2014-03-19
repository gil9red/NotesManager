#ifndef PropertyAttachable_H
#define PropertyAttachable_H

#include <QObject>
#include <QEvent>

//! Класс дает виджетам-окнам возможность прикрепляться к краю экрана.
/*! Используя метод installTo можно указать какому виджету дать возможность прикрепляться. */
class PropertyAttachable: public QObject
{
    Q_OBJECT

public:
    PropertyAttachable( QObject * parent = 0 );

    //! Функция устанавливает минимальную дистанцию прикрепления.
    /*! Когда окно приблизится к краю экрана на расстояние меньшее или равное d,
     *  произойдет прикрепление.
     *  \param d расстояние в пикселях
     *  \sa distance()
     */
    void setDistance( int d );

    //! Функция возвращает минимальную дистанцию прикрепления.
    /*! \return минимальное расстояние в пикселях
     *  \sa setDistance()
     */
    int distance();

    //! Наделяет указанный виджет возможностью прикрепляться.
    /*! \param widget указатель на виджет
     *  \sa remove()
     */
    void installTo( QWidget * widget );

    //! Убирает у текущего виджета возможностью прикрепления.
    /*! \sa installTo()
     */
    void remove();

private:
    QWidget * window; ///< указатель на текущее окно
    int timerId; ///< id текущего запущенного таймера
    int d_distance; ///< минимальная дистанция прикрепления
    bool activate; ///< включение прикрепления

public slots:
    //! Изменение возможности прикрепления.
    /*! \param act если равно true - можно прикрепляться, иначе нельзя
     *  \sa isActivate()
     */
    void setActivate( bool act );

    //! Возвращает состояние возможности прикрепления.
    /*! \return true - если прикрепление включено, иначе - false
     *  \sa setActivate()
     */
    bool isActivate();

protected:
    void timerEvent( QTimerEvent * );
    bool eventFilter( QObject * object, QEvent * event );
};

#endif
