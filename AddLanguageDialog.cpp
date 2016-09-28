#include <QDebug>
#include "BSMFile.det.hpp"

#include "AddLanguageDialog.hpp"
#include "ui_AddLanguageDialog.h"

AddLanguageDialog::AddLanguageDialog(QWidget * parent, Qt::WindowFlags f) :
    QDialog(parent, f),
    m_ui{new Ui::AddLanguageDialog} {
    m_ui->setupUi(this);

    m_ui->cbLang->setItemIcon(0, QIcon(QString::fromLatin1(":/24x24/intl/flag-gb.ico")));
    m_ui->cbLang->setItemIcon(1, QIcon(QString::fromLatin1(":/24x24/intl/flag-fr.ico")));
    m_ui->cbLang->setItemIcon(2, QIcon(QString::fromLatin1(":/24x24/intl/flag-de.ico")));
    m_ui->cbLang->setItemIcon(3, QIcon(QString::fromLatin1(":/24x24/intl/flag-nl.ico")));
    m_ui->cbLang->setItemIcon(4, QIcon(QString::fromLatin1(":/24x24/intl/flag-es.ico")));
    m_ui->cbLang->setItemIcon(5, QIcon(QString::fromLatin1(":/24x24/intl/flag-it.ico")));
    m_ui->cbLang->setItemIcon(6, QIcon(QString::fromLatin1(":/24x24/intl/flag-pt.ico")));
}
AddLanguageDialog::~AddLanguageDialog() = default;

int AddLanguageDialog::langID() const { return m_ui->sbLangID->value(); }
void AddLanguageDialog::setLangID(int langID) {
    if (langID != m_ui->sbLangID->value()) {
        const bool old_block = m_ui->cbLang->blockSignals(true);
        switch (langID) {
            case BSML_ENGLISH:    m_ui->cbLang->setCurrentIndex(0); break;
            case BSML_FRENCH:     m_ui->cbLang->setCurrentIndex(1); break;
            case BSML_GERMAN:     m_ui->cbLang->setCurrentIndex(2); break;
            case BSML_DUTCH:      m_ui->cbLang->setCurrentIndex(3); break;
            case BSML_SPANISH:    m_ui->cbLang->setCurrentIndex(4); break;
            case BSML_ITALIAN:    m_ui->cbLang->setCurrentIndex(5); break;
            case BSML_PORTUGUESE: m_ui->cbLang->setCurrentIndex(6); break;
            default:              m_ui->cbLang->setCurrentIndex(7);
        }
        m_ui->cbLang->blockSignals(old_block);

        m_ui->sbLangID->setEnabled(langID == 7);
        m_ui->sbLangID->setValue(langID);
        emit langIDChanged(m_ui->sbLangID->value());
    }
}

void AddLanguageDialog::on_cbLang_currentIndexChanged(int index) {
    switch (index) {
        case 0: m_ui->sbLangID->setValue(BSML_ENGLISH); break;
        case 1: m_ui->sbLangID->setValue(BSML_FRENCH); break;
        case 2: m_ui->sbLangID->setValue(BSML_GERMAN); break;
        case 3: m_ui->sbLangID->setValue(BSML_DUTCH); break;
        case 4: m_ui->sbLangID->setValue(BSML_SPANISH); break;
        case 5: m_ui->sbLangID->setValue(BSML_ITALIAN); break;
        case 6: m_ui->sbLangID->setValue(BSML_PORTUGUESE); break;
        case 7: break;
        default:;
            qCritical() << "AddLanguageDialog::on_cbLang_currentIndexChanged "
                           "bad index: " << index;
    }

    m_ui->sbLangID->setEnabled(index == 7);
    emit langIDChanged(m_ui->sbLangID->value());
}