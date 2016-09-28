#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <cstdint>
#include <vector>
#include <string>

#include <QMainWindow>
#include <QFileDialog>

#include "qbsmmodel.hpp"
#include "AddLanguageDialog.hpp"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

    private slots:
        void on_aClose_triggered();
        void on_aReload_triggered();
        void on_aSave_triggered();

        void on_openFileDialog_accepted();
        void on_saveFileDialog_accepted();

        void on_tableView_selectionChanged(const QItemSelection & selected,
                                           const QItemSelection & deselected);

        void on_addLanguageDialog_accepted();
        void on_aRemoveLanguage_triggered();

        void on_aAddTranslation_triggered();
        void on_aRemoveTranslation_triggered();
    private:
        Ui::MainWindow *ui;
        QBSMModel m_model;
        AddLanguageDialog m_addLanguageDialog;

        QFileDialog m_openFileDialog;
        QFileDialog m_saveFileDialog;

        QModelIndex m_selected; // Current selected cell (valid only when one cell is selected)

        QString m_filePath;
};

#endif // MAINWINDOW_HPP
