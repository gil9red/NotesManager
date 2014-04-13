#ifndef SCRIPTSMANAGER_H
#define SCRIPTSMANAGER_H

#include <QMainWindow>

namespace Ui
{
    class ScriptsManager;
}

#include <QScriptEngineDebugger>
#include <QCloseEvent>
#include <QSettings>
#include <QListWidgetItem>

class ScriptsManager : public QMainWindow
{
    Q_OBJECT

public:
    explicit ScriptsManager( QWidget * parent = 0 );
    ~ScriptsManager();

    //! В функции указываем с каким классом настроек будем работать.
    void setSettings( QSettings * );

    void read();
    void write();

    void readSettings();
    void writeSettings();

private:
    void deleteRow( int row );
    void addToListOfModified( QListWidgetItem * );
    void removeFromListOfModified( QListWidgetItem * );

private slots:
    void sl_UpdateStates();
    void sl_ContentScriptChanged();
    void sl_SaveScripts();

    void on_actionRunScript_triggered();
    void on_actionAddScript_triggered();
    void on_actionDeleteScript_triggered();
    void on_actionRenameScript_triggered();
    void on_actionDeleteAllScripts_triggered();
    void on_actionSaveAllScripts_triggered();
    void on_cBoxUseDebugger_clicked( bool );
    void on_scripts_itemSelectionChanged();
    void on_scripts_itemChanged( QListWidgetItem * );

private:
    Ui::ScriptsManager * ui;
    QScriptEngineDebugger * scriptEngineDebugger;
    QSettings * settings;
    QList < QListWidgetItem * > listOfModified;

protected:
    void showEvent( QShowEvent * ) { emit visibilityChanged( isVisible() ); }
    void hideEvent( QHideEvent * ) { emit visibilityChanged( isVisible() ); }
    void closeEvent( QCloseEvent * event ) { event->ignore(); hide(); }

signals:
    void visibilityChanged( bool );
};

#endif // SCRIPTSMANAGER_H
