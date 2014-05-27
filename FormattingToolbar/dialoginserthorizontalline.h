#ifndef DIALOGINSERTHORIZONTALLINE_H
#define DIALOGINSERTHORIZONTALLINE_H

#include <QDialog>

namespace Ui {
    class DialogInsertHorizontalLine;
}

class DialogInsertHorizontalLine : public QDialog
{
    Q_OBJECT

public:
    explicit DialogInsertHorizontalLine(QWidget *parent = 0);
    ~DialogInsertHorizontalLine();

private:
    Ui::DialogInsertHorizontalLine *ui;

public slots:
    void accept();

private slots:
    void on_type_activated(const QString &arg1);

signals:
    void sg_insertLine(QString);
};

#endif // DIALOGINSERTHORIZONTALLINE_H
