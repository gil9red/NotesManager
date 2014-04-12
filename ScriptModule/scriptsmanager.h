#ifndef SCRIPTSMANAGER_H
#define SCRIPTSMANAGER_H

#include <QMainWindow>

namespace Ui
{
    class ScriptsManager;
}

#include <QScriptEngineDebugger>
#include <QCloseEvent>

class ScriptsManager : public QMainWindow
{
    Q_OBJECT

public:
    explicit ScriptsManager( QWidget * parent = 0 );
    ~ScriptsManager();

private slots:
    void sl_UpdateStates();

    void on_tButtonRunScript_clicked();
    void on_cBoxUseScriptDebugger_clicked( bool checked );
    void on_tButtonAddScript_clicked();
    void on_tButtonDeleteScript_clicked();
    void on_tButtonRenameScript_clicked();
    void on_tButtonDeleteAllScripts_clicked();

    void on_scripts_itemSelectionChanged();

private:
    Ui::ScriptsManager * ui;
    QScriptEngineDebugger * scriptEngineDebugger;

protected:
    void showEvent( QShowEvent * ) { emit visibilityChanged( isVisible() ); }
    void hideEvent( QHideEvent * ) { emit visibilityChanged( isVisible() ); }
    void closeEvent( QCloseEvent * event ) { event->ignore(); hide(); }

signals:
    void visibilityChanged( bool );
};

#endif // SCRIPTSMANAGER_H
