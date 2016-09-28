#include "MainWindow.hpp"
#include "ui_MainWindow.h"

#include <cstdio>
#include <cinttypes>

#include <QFile>
#include <QMessageBox>

static constexpr char g_defWindowTitle[] = "BSM Editor";
static constexpr char g_defWindowTitleFile[] = "BSM Editor - ";
static constexpr char g_defOpenFileTitle[] = "Open BSM file";
static constexpr char g_defSaveFileTitle[] = "Save BSM file";
static constexpr char g_defFileFilter[] = "BSM file (*.bsm);;Any file (*.*)";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_addLanguageDialog(this),
    m_openFileDialog(this, QApplication::translate("MainWindow", g_defOpenFileTitle),
                           QString(),
                           QApplication::translate("MainWindow", g_defFileFilter)),
    m_saveFileDialog(this, QApplication::translate("MainWindow", g_defSaveFileTitle),
                           QString(),
                           QApplication::translate("MainWindow", g_defFileFilter)) {
    ui->setupUi(this);
    ui->tableView->setModel(&m_model);
    ui->tableView->setItemDelegate(new QBSMDelegate());
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    m_openFileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    m_openFileDialog.setFileMode(QFileDialog::ExistingFile);

    m_saveFileDialog.setAcceptMode(QFileDialog::AcceptSave);
    m_saveFileDialog.setFileMode(QFileDialog::AnyFile);

    QObject::connect(ui->aOpen, SIGNAL(triggered()), &m_openFileDialog, SLOT(open()));
    QObject::connect(ui->aSaveAs, SIGNAL(triggered()), &m_saveFileDialog, SLOT(open()));
    QObject::connect(ui->aAddLanguage, SIGNAL(triggered()), &m_addLanguageDialog, SLOT(open()));

    QObject::connect(&m_openFileDialog, SIGNAL(accepted()), this, SLOT(on_openFileDialog_accepted()));
    QObject::connect(&m_saveFileDialog, SIGNAL(accepted()), this, SLOT(on_saveFileDialog_accepted()));
    QObject::connect(&m_addLanguageDialog, SIGNAL(accepted()), this, SLOT(on_addLanguageDialog_accepted()));

    QObject::connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(on_tableView_selectionChanged(QItemSelection,QItemSelection)));
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_aClose_triggered() {
    m_model.clear();
    ui->aReload->setEnabled(false);
    ui->aClose->setEnabled(false);
}

void MainWindow::on_aReload_triggered() {
    on_aClose_triggered();

    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Unbuffered))
        QMessageBox::critical(this, "Can't open file", file.errorString());
    else {
        m_model.resetFromRAW(file.readAll());
        setWindowTitle(QString(g_defWindowTitleFile).append(m_filePath));

        ui->aReload->setEnabled(true);
        ui->aClose->setEnabled(true);

        m_saveFileDialog.setDirectory(m_openFileDialog.directory());
    }
    file.close();
}
void MainWindow::on_aSave_triggered() {
    QFile file(m_filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Unbuffered)) {
        QMessageBox::critical(this, "Can't create file", file.errorString());
        m_saveFileDialog.open();
    }
    else {
        file.write(m_model.saveToRAW());
        setWindowTitle(QString(g_defWindowTitleFile).append(m_filePath));

        m_openFileDialog.setDirectory(m_saveFileDialog.directory());
    }
    file.close();
}

void MainWindow::on_openFileDialog_accepted() {
    QStringList files = m_openFileDialog.selectedFiles();
    Q_ASSUME(files.count() == 1);
    m_filePath = ::std::move(files[0]);
    return on_aReload_triggered();
}
void MainWindow::on_saveFileDialog_accepted() {
    QStringList files = m_saveFileDialog.selectedFiles();
    Q_ASSUME(files.count() == 1);
    m_filePath = ::std::move(files[0]);
    return on_aSave_triggered();
}

void MainWindow::on_tableView_selectionChanged(const QItemSelection &, const QItemSelection &) {
    if (!ui->tableView->selectionModel()->hasSelection())
        m_selected = QModelIndex();
    else {
        const QModelIndexList& selected = ui->tableView->selectionModel()->selectedIndexes();
        if (selected.size() != 1)
            m_selected = QModelIndex();
        else
            m_selected = selected[0];
    }

    const bool enable_remove = m_selected.isValid();
    ui->aRemoveLanguage->setEnabled(enable_remove && m_model.isLanguageColumn(m_selected.column()));
    ui->aRemoveTranslation->setEnabled(enable_remove);
}

void MainWindow::on_addLanguageDialog_accepted() {
    m_model.addLanguage((BSMLANG)m_addLanguageDialog.langID());
    ui->tableView->setCurrentIndex(m_model.index(0, m_model.columnCount() - 1));
}
void MainWindow::on_aRemoveLanguage_triggered() {
    const int column = m_selected.column();
    if (   m_model.hasColumnTranslations(column)
        && QMessageBox::warning(this, QApplication::translate("MainWindow", "This column has translations"),
                                      QApplication::translate("MainWindow", "%1 language has non-empty translations.\nDo you really want remove it?").arg(BSMLANG_toQString(m_model.getColumnLanguage(column))),
                                      QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
        return;

    m_model.removeColumn(column);
}

void MainWindow::on_aAddTranslation_triggered() {
    m_model.insertRow(m_model.rowCount());
    ui->tableView->setCurrentIndex(m_model.index(m_model.rowCount() - 1));
}
void MainWindow::on_aRemoveTranslation_triggered() {
    const int row = m_selected.row();
    if (   m_model.hasRowTranslations(row)
        && QMessageBox::warning(this, QApplication::translate("MainWindow", "This row has translations"),
                                      QApplication::translate("MainWindow", "Selected translation isn't empty.\nDo you really want remove it?"),
                                      QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
        return;

    m_model.removeRow(row);
}