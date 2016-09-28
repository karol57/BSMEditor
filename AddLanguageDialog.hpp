#ifndef ADDLANGUAGEDIALOG_HPP
#define ADDLANGUAGEDIALOG_HPP

#include <memory>
#include <QDialog>

namespace Ui { class AddLanguageDialog; }

class AddLanguageDialog : public QDialog {
    Q_OBJECT
    Q_PROPERTY(int langID READ langID WRITE setLangID)

    public:
        explicit AddLanguageDialog(QWidget * parent = nullptr,
                                   Qt::WindowFlags f = Qt::WindowFlags());
        ~AddLanguageDialog();

        int langID() const;
    public slots:
        void setLangID(int langID);
    signals:
        void langIDChanged(int langID);
    private slots:
        void on_cbLang_currentIndexChanged(int index);
    private:
        ::std::unique_ptr<Ui::AddLanguageDialog> m_ui;
};

#endif // ADDLANGUAGEDIALOG_HPP
