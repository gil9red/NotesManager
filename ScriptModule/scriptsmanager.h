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

class ScriptsManager : public QMainWindow
{
    Q_OBJECT

public:
    explicit ScriptsManager( QWidget * parent = 0 );
    ~ScriptsManager();

    //! В функции указываем с каким классом настроек будем работать.
    void setSettings( QSettings * );

    void readSettings();
    void writeSettings();

private slots:
    void sl_UpdateStates();

    void on_tButtonRunScript_clicked();
    void on_cBoxUseDebugger_clicked( bool );
    void on_tButtonAddScript_clicked();
    void on_tButtonDeleteScript_clicked();
    void on_tButtonRenameScript_clicked();
    void on_tButtonDeleteAllScripts_clicked();

    void on_scripts_itemSelectionChanged();

private:
    Ui::ScriptsManager * ui;
    QScriptEngineDebugger * scriptEngineDebugger;
    QSettings * settings;

protected:
    void showEvent( QShowEvent * ) { emit visibilityChanged( isVisible() ); }
    void hideEvent( QHideEvent * ) { emit visibilityChanged( isVisible() ); }
    void closeEvent( QCloseEvent * event ) { event->ignore(); hide(); }

signals:
    void visibilityChanged( bool );
};

#endif // SCRIPTSMANAGER_H
