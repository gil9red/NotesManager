#ifndef NoteShared_H
#define NoteShared_H

#include <QFlags>
#include <QColor>
#include <QFile>

//! Содержит глобальные общие перечисления, переменные, классы и функции.
namespace Shared
{
    //! Стороны.
    /*! Перечисление сторон рамки заметки. */
    enum Side
    {
        Left = 0x0001,  //!< Левая
        Right = 0x0002, //!< Правая
        Top = 0x0004,   //!< Верхняя
        Bottom = 0x0008 //!< Нижняя
    };
    Q_DECLARE_FLAGS ( Sides, Side )


    //! Действия при двойном клике на заголовок
    enum ActionDoubleClickingOnTitle
    {
        DoNothing,  //!< Ничего не делать.
        EditTitle,  //!< Редактировать заголовок.
        HideNote,   //!< Скрыть заметку.
        DeleteNote, //!< Удалить заметку.
        SaveAs,     //!< Сохранить заметку как....
        PrintNotes  //!< Печать заметки.
    };
}

Q_DECLARE_OPERATORS_FOR_FLAGS ( Shared::Sides )

//! Содержит значения по умолчанию абстрактной заметки.
namespace nm_Note
{
    const uint topBorder = 31; //!< Высота шапки заметки
    const uint minimalHeight = 75; //!< Минимальная высота заметки
    const uint minimalWidth = 75; //!< Минимальная ширина заметки
    const qreal minimalOpacity = 0.2; //!< Минимальная прозрачность заметки
    const qreal maximalOpacity = 1.0; //!< Максимальная не прозрачность заметки

    //! Окно без кнопки на панели задач и без системной рамки.
    const Qt::WindowFlags flags = Qt::Tool | Qt::FramelessWindowHint;    

    //! Содержит описание параметров рамки заметки (по умолчанию).
    namespace Frame
    {
        const bool visible = true;      //!< Видимость
        const qreal width = 1.0;        //!< Ширина (толщина)
        const QColor color = Qt::black; //!< Цвет
    }

    //! Стиль виджета заметки.
    extern QString style;
}

#endif // NoteShared_H
