#include <algorithm>

#include <QApplication>
#include <QIcon>
#include <QTextCodec>
#include <QDebug>
#include <QSpinBox>
#include <QLineEdit>

#include "qbsmmodel.hpp"

#include "BSMFile.det.hpp"
#include "qbsmfile.hpp"

template <typename T>
QVariant qmake_variant(const T& val) {
    QVariant res;
    res.setValue(val);
    return ::std::move(res);
}

static
QString SDNWStringDecode(const ::std::string & str) {
    QTextCodec::ConverterState state;
    QString result = QTextCodec::codecForName("Windows-1252")
                        ->toUnicode(str.data(), str.length(), &state);

    if (state.invalidChars > 0)
        qWarning() << "SDNWStringDecode: Cannot decode " << state.invalidChars << " chars";

    return std::move(result);
}

static
::std::string SDNWStringEncode(const QString & str) {
    QTextCodec::ConverterState state;
    QByteArray data = QTextCodec::codecForName("Windows-1252")
                        ->fromUnicode(str.data(), str.length(), &state);

    if (state.invalidChars > 0)
        qWarning() << "SDNWStringDecode: Cannot encode " << state.invalidChars << " chars";
    return ::std::string(data.data(), data.length());
}

QBSMDelegate::QBSMDelegate(QObject * parent) : QStyledItemDelegate(parent) {}
QBSMDelegate::~QBSMDelegate() = default;

QWidget * QBSMDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem &, const QModelIndex & index) const {
    QBSMModel::Field field = index.data(Qt::EditRole).value<QBSMModel::Field>();
    switch (field.type) {
        case QBSMModel::Field::FT_CAT: {
            QSpinBox * result = new QSpinBox(parent);
            result->setPrefix("0x");
            result->setDisplayIntegerBase(16);
            result->setMaximum(0xFF);
            result->setValue(field.catIdLang);
            return result;
        } break;
        case QBSMModel::Field::FT_ID: {
            QSpinBox * result = new QSpinBox(parent);
            result->setPrefix("0x");
            result->setDisplayIntegerBase(16);
            result->setMaximum(0xFFFF);
            result->setValue(field.catIdLang);
            return result;
        } break;
        case QBSMModel::Field::FT_STRING:
            return new QLineEdit(field.string, parent);
        default:;
    }

    return nullptr;
}

void QBSMDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const {
    QBSMModel::Field field = index.data(Qt::EditRole).value<QBSMModel::Field>();
    switch (field.type) {
        case QBSMModel::Field::FT_CAT:
            field.catIdLang = ((QSpinBox*)editor)->value();
            field.string = QString("0x%1").arg(field.catIdLang, 2, 16, QLatin1Char('0'));
            break;
        case QBSMModel::Field::FT_ID:
            field.catIdLang = ((QSpinBox*)editor)->value();
            field.string = QString("0x%1").arg(field.catIdLang, 4, 16, QLatin1Char('0'));
            break;
        case QBSMModel::Field::FT_STRING:
            field.string = ((QLineEdit*)editor)->text();
            break;
        default:
            break;
    }
    model->setData(index, qmake_variant(field));
}

QSize QBSMDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const {
    // I havn't idea how to do this in right way
    QBSMModel::Field field = index.data(Qt::EditRole).value<QBSMModel::Field>();
    switch (field.type) {
        case QBSMModel::Field::FT_CAT: {
            QSpinBox * tmp = new QSpinBox();
            tmp->setPrefix("0x");
            tmp->setDisplayIntegerBase(16);
            tmp->setMaximum(0xFFF);
            tmp->setValue(0xFFF);
            QSize result = tmp->sizeHint();
            delete tmp;
            return result;
        } break;
        case QBSMModel::Field::FT_ID: {
            QSpinBox * tmp = new QSpinBox();
            tmp->setPrefix("0x");
            tmp->setDisplayIntegerBase(16);
            tmp->setMaximum(0xFFFFF);
            tmp->setValue(0xFFFFF);
            QSize result = tmp->sizeHint();
            delete tmp;
            return result;
        } break;
        default:;
    };

    return QStyledItemDelegate::sizeHint(option, index);
}

QBSMModel::QBSMModel(QObject * parent) : QAbstractListModel(parent) {
    _regen_columns();
}
QBSMModel::~QBSMModel() = default;

int QBSMModel::columnCount(const QModelIndex &) const { return m_columns.size(); }
int QBSMModel::rowCount(const QModelIndex &) const { return m_fields.height(); }

Qt::ItemFlags QBSMModel::flags(const QModelIndex & index) const {
    return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
}

QVariant QBSMModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (   orientation == Qt::Horizontal
        && section >= 0 && (unsigned)section < m_columns.size()) {
        const Column & column = m_columns[section];
        switch (role) {
            case Qt::DisplayRole: return column.name;
            case Qt::DecorationRole: {
                if (column.type == Column::CL_LANG) switch (column.lang) {
                    case BSML_ENGLISH:    return QIcon(QString(":/24x24/intl/flag-gb.ico"));
                    case BSML_FRENCH:     return QIcon(QString(":/24x24/intl/flag-fr.ico"));
                    case BSML_GERMAN:     return QIcon(QString(":/24x24/intl/flag-de.ico"));
                    case BSML_DUTCH:      return QIcon(QString(":/24x24/intl/flag-nl.ico"));
                    case BSML_SPANISH:    return QIcon(QString(":/24x24/intl/flag-es.ico"));
                    case BSML_ITALIAN:    return QIcon(QString(":/24x24/intl/flag-it.ico"));
                    case BSML_PORTUGUESE: return QIcon(QString(":/24x24/intl/flag-pt.ico"));
                    default:;
                }
            } break;
        }
    }
    return QAbstractListModel::headerData(section, orientation, role);
}

QVariant QBSMModel::data(const QModelIndex & index, int role) const {
    size_t x, y;
    if (!getPosFromIndex(index, x, y))
        return QVariant();

    switch (role) {
        case Qt::DisplayRole: return m_fields.m_data[y][x].string;
        case Qt::EditRole: return qmake_variant(m_fields.m_data[y][x]);
    }

    return QVariant();
}
bool QBSMModel::setData(const QModelIndex & index, const QVariant & value, int role) {
    if (role != Qt::EditRole)
        return false;

    size_t x, y;
    if (!getPosFromIndex(index, x, y))
        return false;

    if (!value.canConvert<Field>())
        return false;

    m_fields.m_data[y][x] = value.value<Field>();
    emit dataChanged(index, index, QVector<int>(1, role));

    return true;
}

bool QBSMModel::resetFromRAW(const QByteArray& data) {
    QBSMFile qBSMFile;
    if (qBSMFile.loadRAW(data) != QBSMFile::LR_SUCCEEDED)
        return false;

    beginResetModel();
    m_fields.clear();
    m_languages = qBSMFile.availableLanguages();
    _regen_columns();
    m_fields.resize(m_columns.size(), qBSMFile.maxTranslationsAvailable());

    for (const QBSMString & string : qBSMFile.data()) {
        const int cat = string.category();
        const int id = string.identiefier();
        const BSMLANG lang = string.language();

        const unsigned x = [this](BSMLANG langid) {
            for (const Column & column : m_columns)
                if (   column.type == Column::CL_LANG
                    && column.lang == langid)
                    return (size_t)std::distance(&*m_columns.cbegin(), &column);
            Q_ASSUME(false); // Shouldn't be here (assume all data are valid)
        } (lang);

        std::vector<Field> & ins_row = [this, cat, id]() -> std::vector<Field>& {
            for (std::vector<Field> & row : m_fields.m_data)
                if (   (row[0].type == Field::FT_NONE || (row[0].type == Field::FT_CAT && row[0].catIdLang == cat))
                    && (row[1].type == Field::FT_NONE || (row[1].type == Field::FT_ID && row[1].catIdLang == id)))
                    return row;
            Q_ASSUME(false); // Shouldn't be here (assume all data are valid)
        }();

        // Assume category column == 0;
        if (ins_row[0].type != Field::FT_CAT) {
            ins_row[0].type = Field::FT_CAT;
            ins_row[0].catIdLang = cat;
            ins_row[0].string = QString("0x%1").arg(cat, 2, 16, QLatin1Char('0'));
        }

        // Assume id column == 1;
        if (ins_row[1].type != Field::FT_ID) {
            ins_row[1].type = Field::FT_ID;
            ins_row[1].catIdLang = id;
            ins_row[1].string = QString("0x%1").arg(id, 4, 16, QLatin1Char('0'));
        }

        // Override duplicates
        ins_row[x].type = Field::FT_STRING;
        ins_row[x].catIdLang = lang;
        ins_row[x].string = SDNWStringDecode(string.text());
    }

    endResetModel();

    return true;
}

QByteArray QBSMModel::saveToRAW() const noexcept {
    std::vector<QBSMString> data;

    for (const std::vector<Field>& row : m_fields.m_data) {
        const uint8_t cat = [row](){
            for (const Field & f : row)
                if (f.type == Field::FT_CAT)
                    return f.catIdLang;
            Q_ASSUME(false); // Assume all data are valid
        }();
        const uint16_t id = [row](){
            for (const Field & f : row)
                if (f.type == Field::FT_ID)
                    return f.catIdLang;
            Q_ASSUME(false); // Assume all data are valid
        }();;

        for (const Field & f : row)
        if (f.type == Field::FT_STRING)
            data.emplace_back(cat, id, (BSMLANG)f.catIdLang, SDNWStringEncode(f.string));
    }

    QBSMFile qBSMFile;
    qBSMFile.setData(std::move(data));
    return qBSMFile.saveRAW();
}

void QBSMModel::clear() {
    beginResetModel();
    m_fields.clear();
    m_languages.reset();
    _regen_columns();
    endResetModel();
}

void QBSMModel::addLanguage(BSMLANG lang) {
    if (m_languages.test(lang)) {
        qWarning() << "QBSMModel::addLanguage: Attempt to add existing language";
        return;
    }

    beginInsertColumns(QModelIndex(), m_fields.width(), m_fields.width());
    m_languages.set(lang);
    m_columns.push_back({ BSMLANG_toQString(lang), Column::CL_LANG, lang });

    m_fields.resize(m_fields.width() + 1, m_fields.height());
    for (std::vector<Field> & row : m_fields.m_data) {
        row.back().type = Field::FT_STRING;
        row.back().catIdLang = lang;
    }
    endInsertColumns();
}
bool QBSMModel::removeColumns(int column, int count, const QModelIndex & parent) {
    if (count == 0)
        return true;

    if (parent.isValid())
        return false;

    int _cc;
    if (__builtin_sadd_overflow(column, count, &_cc))
        return false;

    int first = ::std::min(column, _cc);
    int last = ::std::max(column, _cc);

    if (_cc < 0) {
        if (__builtin_sadd_overflow(first, 1, &first))
            return false;
    }
    else {
        if (__builtin_ssub_overflow(last, 1, &last))
            return false;
    }


    if (   first < 0     || (unsigned)first >= m_columns.size()
        || last  < first || (unsigned)last >= m_columns.size())
        return false;


    const auto col_it_beg = m_columns.cbegin() + first;
    const auto col_it_end = m_columns.cbegin() + last + 1;
    for (auto it = col_it_beg; it != col_it_end; ++it)
        if (it->type != Column::CL_LANG)
            return false;
    for (auto it = col_it_beg; it != col_it_end; ++it)
        m_languages.reset(it->lang);

    beginRemoveColumns(parent, first, last);

    m_columns.erase(m_columns.cbegin() + first, m_columns.cbegin() + last + 1);

    for (std::vector<Field> & row : m_fields.m_data)
        row.erase(row.cbegin() + first, row.cbegin() + last + 1);

    if (count < 0)
        m_fields.m_width += count;
    else
        m_fields.m_width -= count;

    endRemoveColumns();

    return true;
}

bool QBSMModel::isLanguageColumn(int column) const noexcept {
    return (0 <= column && (unsigned)column < m_columns.size()
            && m_columns[(unsigned)column].type == Column::CL_LANG);
}

bool QBSMModel::hasColumnTranslations(int column) const {
    if (!isLanguageColumn(column))
        return false;

    for (const std::vector<Field> & row : m_fields.m_data)
        if (!row[column].string.isEmpty())
            return true;

    return false;
}

BSMLANG QBSMModel::getColumnLanguage(int column) const {
    if (!isLanguageColumn(column)) {
        qWarning() << "QBSMModel::getColumnLanguage called for non-language column. Returning 0";
        return (BSMLANG)0;
    }

    return m_columns[column].lang;
}

bool QBSMModel::insertRows(int row, int _count, const QModelIndex & parent) {
    if (_count == 0)
        return true;

    if (parent.isValid())
        return false;

    int _rc;
    if (__builtin_sadd_overflow(row, _count, &_rc))
        return false;


    int first = ::std::min(row, _rc);
    int last = ::std::max(row, _rc);

    if (_rc < 0) {
        if (__builtin_sadd_overflow(first, 1, &first))
            return false;
    }
    else {
        if (__builtin_ssub_overflow(last, 1, &last))
            return false;
    }

    if (first < 0 || (unsigned)first > m_fields.height())
        return false;

    beginInsertRows(parent, first, last);


    const unsigned count = (unsigned)abs(_count);
    m_fields.m_data.insert(m_fields.m_data.cbegin() + first, count, {});

    const auto row_it_beg = m_fields.m_data.begin() + first;
    const auto row_it_end = m_fields.m_data.begin() + last + 1;
    for (auto it = row_it_beg; it != row_it_end; ++it) {
        const auto col_it_beg = m_columns.cbegin();
        const auto col_it_end = m_columns.cend();

        it->resize(m_columns.size());
        const auto field_it_beg = it->begin();
        const auto field_it_end = it->end();

        auto col_it = col_it_beg;
        auto field_it = field_it_beg;

        for (;   col_it != col_it_end
              && field_it != field_it_end;
             ++col_it, ++field_it) switch (col_it->type) {
            case Column::CL_CAT:
                field_it->type = Field::FT_CAT;
                field_it->catIdLang = 0;
                field_it->string = "0x00";
                break;
            case Column::CL_ID:
                field_it->type = Field::FT_ID;
                field_it->catIdLang = 0;
                field_it->string = "0x0000";
                break;
            case Column::CL_LANG:
                field_it->type = Field::FT_STRING;
                field_it->catIdLang = col_it->lang;
                break;
            default: Q_ASSUME(false); break;
        }
    }

    m_fields.m_height += count;
    endInsertRows();

    return true;
}
bool QBSMModel::removeRows(int row, int count, const QModelIndex & parent) {
    if (count == 0)
        return true;

    if (parent.isValid())
        return false;

    int _rc;
    if (__builtin_sadd_overflow(row, count, &_rc))
        return false;

    int first = ::std::min(row, _rc);
    int last = ::std::max(row, _rc);

    if (_rc < 0) {
        if (__builtin_sadd_overflow(first, 1, &first))
            return false;
    }
    else {
        if (__builtin_ssub_overflow(last, 1, &last))
            return false;
    }


    if (   first < 0     || (unsigned)first >= m_fields.height()
        || last  < first || (unsigned)last >= m_fields.height())
        return false;

    beginRemoveRows(parent, first, last);
    m_fields.m_data.erase(m_fields.m_data.cbegin() + first,
                          m_fields.m_data.cbegin() + last + 1);

    if (count < 0)
        m_fields.m_height += count;
    else
        m_fields.m_height -= count;
    endRemoveRows();


    return true;
}

bool QBSMModel::hasRowTranslations(int row) const {
    if (row < 0 || (unsigned)row >= m_fields.height())
        return false;

    for (const Field& f : m_fields.m_data[row])
        if (f.type == Field::FT_STRING && !f.string.isEmpty())
            return true;

    return false;
}

void QBSMModel::_regen_columns() {
    m_columns.resize(2u + m_languages.count());

    m_columns[0].name = QApplication::translate("QBSMModel", "Cat");
    m_columns[0].type = Column::CL_CAT;
    m_columns[1].name = QApplication::translate("QBSMModel", "Id");
    m_columns[1].type = Column::CL_ID;

    auto lang_from_column_id = [this](size_t idx) {
        const size_t lang_index  = idx - 2u;
        size_t lang_id = 0;
        for (size_t i = 0; i <= lang_index; ++i)
            while (!m_languages.test(lang_id++) && lang_id < 256);
        return (BSMLANG)--lang_id;
    };

    for (size_t id = 2; id < m_columns.size(); ++id) {
        const auto lang_id = lang_from_column_id(id);
        m_columns[id].name = BSMLANG_toQString(lang_id);
        m_columns[id].type = Column::CL_LANG;
        m_columns[id].lang = lang_id;
    }
}

bool QBSMModel::getPosFromIndex(const QModelIndex & index, size_t & x, size_t & y) const {
    if (!index.isValid())
        return false;

    const int _x = index.column();
    if (_x < 0 || (unsigned)_x > m_fields.width())
        return false;

    const int _y = index.row();
    if (_y < 0 || (unsigned)_y > m_fields.height())
        return false;

    x = _x;
    y = _y;

    return true;
}