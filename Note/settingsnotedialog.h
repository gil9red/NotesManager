#ifndef SETTINGSNOTEDIALOG_H
#define SETTINGSNOTEDIALOG_H

#include <QDialog>

namespace Ui {
    class SettingsNoteDialog;
}

#include <QVariantMap>
#include "abstractnote.h"

class SettingsNoteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsNoteDialog(QWidget *parent = 0);
    ~SettingsNoteDialog();

    void setNote( AbstractNote * n );

private:
    Ui::SettingsNoteDialog *ui;
    AbstractNote * note;
    QFont titleFont;

public slots:
    void accept();

private slots:
    void on_selectTitleColor_clicked();
    void on_selectWindowColor_clicked();
    void on_selectTitleFont_clicked();
};

#endif // SETTINGSNOTEDIALOG_H
