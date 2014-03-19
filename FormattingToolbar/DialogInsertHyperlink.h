#ifndef DIALOGINSERTHYPERLINK_H
#define DIALOGINSERTHYPERLINK_H

#include <QDialog>

namespace Ui
{
    class DialogInsertHyperlink;
}

//! Диалог вставки гиперссылки.
class DialogInsertHyperlink: public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogInsertHyperlink( QWidget * parent = 0 );
    ~DialogInsertHyperlink();

    QString hyperlink(); //!< Функция возвращает строку с гиперссылкой.
    QString text();      //!< Функция возвращает строку текста гиперссылки.
    
private:
    Ui::DialogInsertHyperlink * ui; //!< Форма UI класса.

private slots:
    void updateStates(); //!< Обновление состояния.
};

#endif // DIALOGINSERTHYPERLINK_H
