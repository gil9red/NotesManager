#ifndef ATTACHPANEL_H
#define ATTACHPANEL_H

#include <QWidget>
#include <QStandardItemModel>

namespace Ui
{
    class AttachPanel;
}

class RichTextNote;

//! Класс реализует панель прикрепленных файлов заметки.
/*! Панель может управлять прикрепленными файлами заметки. */
class AttachPanel: public QWidget
{
    Q_OBJECT
    
public:
    explicit AttachPanel( QWidget * parent = 0 );
    ~AttachPanel();
    
    //! Отображение и управление прикрепленными файлами заметки n.
    /*! После добавления указателя к заметке, панель будет связана с заметкой. */
    void setViewTo( RichTextNote * n );

public slots:
    //! Обновление списка панели.
    void refresh();

private slots:
    //! Обновление состояния панели.
    void updateStates();

    void on_tButtonAttach_clicked();            //!< Прикрепить файл.
    void on_tButtonRemove_clicked();            //!< Удалить файл.
    void on_tButtonRemoveAll_clicked();         //!< Очистить список.
    void on_tButtonRefreshList_clicked();       //!< Обновить список.
    void on_tButtonCopyToClipboard_clicked();   //!< Копировать файл в буфер обмена.
    void on_tButtonRun_clicked();               //!< Запустить файл.

private:
    Ui::AttachPanel * ui;       //!< Форма UI.
    RichTextNote * note;        //!< Заметка к которой связана панель
    QStandardItemModel * model; //!< Указатель на модель прикрепленных файлов
};

#endif // ATTACHPANEL_H
