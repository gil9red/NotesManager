#ifndef FadLineEdit_H
#define FadLineEdit_H

#include <QLineEdit>
#include <QList>
#include <QAbstractButton>
#include <QHBoxLayout>

//! Однострочный редактор с возможностью добавления на него кнопок.
class FadLineEdit: public QLineEdit
{
    Q_OBJECT

public:
    //! Положение кнопки.
    enum Side
    {
        None, //!< Никакое
        Left, //!< Слева
        Right //!< Справа
    };

public:    
    //! Создает однострочный редактор уже с текстом.
    /*! \param text строка редактора
     *  \param parent указатель на виджет-родитель
     */
    explicit FadLineEdit( const QString & text, QWidget * parent = 0 );

    /*! Перегруженный конструктор. */
    explicit FadLineEdit( QWidget * parent = 0 );

    //! Добавление кнопки на текстовый редактор.
    /*! \param button указатель на кнопку
     *  \param side положение кнопки (Left или Right)
     */
    void addButton( QAbstractButton * button, Side side = Right );

    //! Удаление кнопки из текстового редактора.
    /*! Функция вернет не 0 указатель, если кнопку удалось обнаружить и убрать из редактора.
     *  \param button указатель на кнопку
     *  \param side положение кнопки (Left или Right)
     */
    QAbstractButton * removeButton( QAbstractButton * button, Side side = Right );

    //! Функция вернет список кнопок, находящиеся слева.
    QList < QAbstractButton * > * leftSideButtons();

    //! Функция вернет список кнопок, находящиеся справа.
    QList < QAbstractButton * > * rightSideButtons();

    //! Функция вернет положение кнопки (Side) на текстовом редакторе.
    /*! \param button указатель на кнопку */
    Side sideButton( QAbstractButton * button );

private:
    void init();               //!< Инициализация класса
    void refreshTextMargins(); //!< Обновление внутренней геометрии класса

protected:
    QList < QAbstractButton * > d_leftSideButtons;  //!< Cписок кнопок, находящиеся слева.
    QList < QAbstractButton * > d_rightSideButtons; //!< Cписок кнопок, находящиеся справа.

    QHBoxLayout leftSideLayout;  //! Компоновщик кнопок, находящиеся слева.
    QHBoxLayout rightSideLayout; //! Компоновщик кнопок, находящиеся справа.
};

#endif
