#ifndef PAGE_SETTINGS_H
#define PAGE_SETTINGS_H

#include <QMainWindow>
#include <QSettings>
#include <QTreeWidgetItem>
#include <QAbstractButton>
#include <QVariantMap>
#include <QDebug>

namespace Ui
{
    class Page_Settings;
}

//! Класс, реализующий виджет "Настройки".
class Page_Settings: public QMainWindow
{
    Q_OBJECT
    
public:
    explicit Page_Settings( QWidget * parent = 0 );
    ~Page_Settings();
        
    //! В функции указываем с каким классом настроек будем работать.
    void setSettings( QSettings * s );

    void mapToSettings();
    void settingsToMap();

    static QString getDefaultLanguage();
    static QString getTrDefaultLanguage();
    static QString getLanguage( QSettings * s );

public:
    Ui::Page_Settings * ui; //!< Форма UI.
    QSettings * settings; //!< Класс настройки.
    QVariantMap mapSettings;

public slots:
    void readSettings();  //!< Считывание настроек.
    void writeSettings(); //!< Запись настроек.

private slots:
    void on_treeWidgetMenu_itemClicked( QTreeWidgetItem * item, int column );
    void on_buttonBox_clicked( QAbstractButton * button );
    void on_tButtonFontTitle_clicked();
    void on_lEditTitle_textChanged(const QString & text );

signals:
    void acceptChangeSettings();
    void message( const QString & text, int msec );
};

#endif // PAGE_SETTINGS_H
