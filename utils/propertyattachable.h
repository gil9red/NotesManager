/*************************************************************************
**
** Copyright (C) 2013-2014 by Ilya Petrash
** All rights reserved.
** Contact: gil9red@gmail.com
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the
** Free Software Foundation, Inc.,
** 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
**
**************************************************************************/

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

//class AttachableWindow: public QWidget
//{
//    Q_OBJECT

//public:
//    explicit AttachableWindow( QWidget * parent = 0 )
//        : QWidget( parent )
//    {
//    }

//private:
//    int timerId;

//protected:
//    void timerEvent( QTimerEvent * event )
//    {
//        if( event->timerId() != timerId )
//        {
//            killTimer( event->timerId() );
//            return;
//        }

//        const QRect & availableGeometry = QDesktopWidget().availableGeometry();
//        int d_x = availableGeometry.x();
//        int d_y = availableGeometry.y();
//        int d_width = availableGeometry.width();
//        int d_height = availableGeometry.height();

//        int distance = 10;

//        if ( x() >= d_x && x() <= distance )
//            move( d_x, y() );

//        if ( y() >= d_y && y() <= distance )
//            move( x(), d_y );

//        if ( x() + width() >= d_width - distance && x() + width() <= d_width )
//            move( d_width - width(), y() );

//        if ( y() + height() >= d_height - distance && y() + height() <= d_height )
//            move( x(), d_height - height() );
//    }
//    void mousePressEvent( QMouseEvent * event )
//    {
//        QWidget::mousePressEvent( event );
//        timerId = startTimer( 333 );
//    }
//    void mouseReleaseEvent( QMouseEvent * event )
//    {
//        QWidget::mouseReleaseEvent( event );
//        killTimer( timerId );
//    }
//};

//class MovableWindow: public QWidget
//{
//public:
//    MovableWindow( QWidget * parent = 0 )
//        : QWidget( parent )
//    {
//        setWindowFlags( Qt::Window | Qt::FramelessWindowHint );

//        QHBoxLayout * layout = new QHBoxLayout();
//        layout->setContentsMargins( 0, 0, 0, 0 );
//        layout->addWidget( new QSizeGrip( this ), 0, Qt::AlignLeft | Qt::AlignTop );
//        layout->addWidget( new QSizeGrip( this ), 0, Qt::AlignRight | Qt::AlignBottom );
//        setLayout( layout );

//        setMinimumSize( 50, 50 );
//        resize( 150, 150 );

//        PropertyAttachable * propertyAttachable = new PropertyAttachable( this );
//        propertyAttachable->installTo( this );
//    }

//private:
//    bool isMousePress;
//    QPoint oldPosition;

//protected:
//    void mousePressEvent( QMouseEvent * event )
//    {
//        if ( event->button() == Qt::LeftButton )
//        {
//            oldPosition = event->pos();
//            isMousePress = true;
//        }

//        QWidget::mousePressEvent( event );
//    }
//    void mouseReleaseEvent( QMouseEvent * event )
//    {
//        isMousePress = false;

//        QWidget::mouseReleaseEvent( event );
//    }
//    void mouseMoveEvent( QMouseEvent * event )
//    {
//        if ( isMousePress )
//        {
//            QPoint delta = event->pos() - oldPosition;
//            move( pos() + delta );
//        }

//        QWidget::mouseMoveEvent( event );
//    }
//    void paintEvent( QPaintEvent * )
//    {
//        QPainter painter( this );
//        painter.setPen( QPen( Qt::black, 2.0 ) );
//        painter.drawRect( rect() );
//    }
//};

#endif
