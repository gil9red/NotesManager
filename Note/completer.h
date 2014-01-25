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

#ifndef COMPLETER_H
#define COMPLETER_H

#include <QCompleter>
#include <QMap>
#include <QTextEdit>
#include <QEvent>

#include "dictionary.h"

//! Класс реализует комплетер.
/*! Этот класс при вводе символом в QTextEdit позволяет показать
 *  варианты окончания слов.
 *  \sa Dictionary
 */
class Completer: public QCompleter
{
    Q_OBJECT

public:
    Completer( QObject * parent = 0 );
    ~Completer();

    //! Возврат статичного указателя на "себя".
    static Completer * instance() { return self; }

    //! Добавление словаря.
    void addDictionary( Dictionary * dict );

    //! Функция возвращает указатель на словарь.
    /*! Класс ищет словарь по его имени.
     *  \param lang текстовая строка, название языка
     *  \return указатель на словарь, иначе 0, если словарь не был найден.
     */
    Dictionary * dictionary( const QString & lang );

    //! Функция устанавливает словарь.
    /*! Устанавливает словарь для комплектора. Поиск словаря в списке происходит по названию.
     *  \param lang текстовая строка, название языка
     *  \return true если получилось установить словарь, иначе false.
     */
    bool setDictionary( const QString & lang );

    //! Установка текстового редактора, к которому подключается комплектор.
    void setEditor( QTextEdit * textEdit );

    //! Возврат указателя текстового редактора, к которому подключен комплектор.
    /*! \return 0, если комплектор не подключен к текстовому редактору */
    QTextEdit * editor();

    //! Возврат списка языков словарей, доступных комплектору.
    QStringList languages();

    //! Возврат количества языков словарей, доступных комплектору  .
    int count();

private:
    QMap < QString, Dictionary * > dictionaries; //!< Доступные словари
    static Completer * self;                     //!< Указатель на "себя"
    bool autocomplete;                           //!< Включение автодополнение текста
    QTextEdit * d_editor;                        //!< Указатель на текстовый редактор

private:
    QString textUnderCursor(); //!< Возвращение текста под курсором

private slots:
    //!< Вставка завершающего текста.
    void insertCompletion( const QString & completion );

public slots:
    //!< Включение автодополнения.
    void setAutocomplete( bool b );

    //!< Функция вернет true, если автодополнение включено, иначе false.
    bool isAutocomplete();

protected:
    bool eventFilter( QObject * object, QEvent * event );
};

#endif // COMPLETER_H
