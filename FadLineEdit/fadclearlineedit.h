#ifndef FADCLEARLINEEDIT_H
#define FADCLEARLINEEDIT_H

#include "FadLineEdit.h"
#include <QToolButton>

//! Однострочный редактор с возможностью добавления на него кнопок.
/*! По умолчанию, имеет кнопку, которая очищает строку ввода.
 *  Кнопка очищения видима только тогда, когда есть текст.
 *  \sa FadLineEdit
 */
class FadClearLineEdit: public FadLineEdit
{
    Q_OBJECT

public:
    //! Создает однострочный редактор уже с текстом.
    /*! \param text строка редактора
     *  \param parent указатель на виджет-родитель
     */
    explicit FadClearLineEdit( const QString & text, QWidget * parent = 0 );

    /*! Перегруженный конструктор. */
    explicit FadClearLineEdit( QWidget * parent = 0 );

private:
    void init(); //!< Инициализация класса

private slots:
    void updateStates(); //!< Обновление состояния класса

protected:
    QToolButton tButtonClearText; //!< Кнопка, очищающая строку ввода

signals:
    //! Вызывается, при нажатии на кнопку очищения строки ввода
    void cleansingText();
};

#endif // FADCLEARLINEEDIT_H
