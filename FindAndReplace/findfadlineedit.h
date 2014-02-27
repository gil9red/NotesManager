/*************************************************************************
**
** Copyright (C) 2013-2014 by Ilya Petrash
** All rights reserved.
** Contact: gil9red@gmail.com, ip1992@inbox.ru
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


#ifndef FINDFADLINEEDIT_H
#define FINDFADLINEEDIT_H

#include "FadLineEdit/fadclearlineedit.h"
#include "findshared.h"

#include <QMenu>

//! Текстовый однострочный редактор виджета поиска и замены.
/*! Имеет возможность выбора флагов поиска */
class FindFadLineEdit: public FadClearLineEdit
{
    Q_OBJECT

public:
    //! Создает однострочный редактор уже с текстом.
    /*! \param text строка редактора
     *  \param parent указатель на виджет-родитель
     */
    explicit FindFadLineEdit( const QString & text, QWidget * parent = 0 );

    /*! Перегруженный конструктор. */
    explicit FindFadLineEdit( QWidget * parent = 0 );

    //! Установка поисковых флагов (Shared::FindFlags).
    void setFindOptions( Shared::FindFlags flags );

    //! Возврат текущих поисковых флагов.
    Shared::FindFlags findOptions();
//    void setCaseSensitive( bool casesensitive );
//    void setWholeWords(bool wholewords );
//    void setRegExp( bool regexp );

//    bool isCaseSensitive();
//    bool isWholeWords();
//    bool isRegExp();

private:
    void init(); //!< Инициализация класса.

protected:
    QToolButton tButtonOptions;    //!< Кнопка выбора флагов.
    QMenu * menuOptions;           //!< Меню выбора флага.
    Shared::FindFlags findFlags;   //!< Флаги.
    QAction * actionCaseSensitive; //!< Действие "Чувствительность к регистру".
    QAction * actionWholeWords;    //!< Действие "Только целые слова".
    QAction * actionRegExp;        //!< Действие "Использование регулярных выражений".

private slots:
    void invokeShowOptions();      //!< Вызов меню выбора флага.
    void updateIcons();            //!< Обновление иконки кнопки выбора флагов.
    void invokeFindFlagsChanged(); //!< Принудительный вызов сигнала изменения флагов.

signals:
    //! Вызывается при изменении установки поисковых флагов
    void findFlagsChanged();
};

#endif // FINDFADLINEEDIT_H
