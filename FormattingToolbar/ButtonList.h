#ifndef BUTTONLIST_H
#define BUTTONLIST_H

#include <QToolButton>
#include <QButtonGroup>
#include "utils/func.h"

//! Кнопка, которая объединяет другие кнопки.
class ButtonList: public QToolButton
{
    Q_OBJECT

public:
    ButtonList( QWidget * parent = 0 );

    //! Добавление связи с кнопкой и указание ее id.
    void addButton( QAbstractButton * button, int id );

    //! Установление id по умолчанию.
    /*! \sa defaultId() */
    void setDefaultId( int id );

    //! Возврат стиля (id) по умолчанию.
    /*! \sa setDefaultId() */
    int defaultId();

private:
    QButtonGroup buttonGroup; //!< Группа кнопок.
    int d_defaultId;          //!< id по умолчанию.

public slots:
    //! Функция возвращает указатель на кнопку, которая является контрольной.
    /*! \param style идентификатор кнопки */
    QAbstractButton * checkedButton( int id );

private slots:
    //! Вызов значения по умолчанию.
    /*! \sa selected() */
    void clickDefault();

signals:
    //! Вызывается при клике кнопки из группы.
    /*! Передает id кнопки */
    void selected( int );
};

#endif
