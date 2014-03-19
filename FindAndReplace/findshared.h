#ifndef FindShared_H
#define FindShared_H

#include <QFlags>

//! Содержит глобальные общие перечисления, переменные, классы и функции.
namespace Shared
{
    //! Поиск.
    /*! Перечисление флагов поиска. */
    enum FindFlag
    {
        CaseSensitive = 0x0001, //!< Чувствительность к регистру
        WholeWords    = 0x0002, //!< Только целые слова
        RegExp        = 0x0004  //!< Использование регулярных выражений
    };
    Q_DECLARE_FLAGS ( FindFlags, FindFlag )
}

Q_DECLARE_OPERATORS_FOR_FLAGS ( Shared::FindFlags )

#include <QIcon>
#include <QPainter>

//! Содержит функции, связанные с виджетами-поисковиками.
namespace Find
{
    //! Функция, в зависимости от переданных флагов возвращает иконку.
    /*! \param flags поисковые флаги
     *  \return иконка
     */
    QIcon iconForFindFlags( Shared::FindFlags flags );
}

#endif // FindShared_H
