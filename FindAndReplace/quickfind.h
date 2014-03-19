#ifndef QuickFind_H
#define QuickFind_H

#include "findandreplace.h"

//! Виджет быстрого поиска текста.
class QuickFind: public FindAndReplace
{
    Q_OBJECT

public:
    //! Конструктор.
    /*! \param textedit указатель на текстовый редактор,
     *  в котором и будем делать поиск и замену текста, а также
     *  подсветку найденных строк
     *  \parent указатель на виджет-родитель
     */
    QuickFind( QTextEdit * textedit = 0, QWidget * parent = 0 );
};

#endif
