#include "qbsmfile.hpp"

#include <algorithm>
#include "File.hpp"
#include <QDebug>

QBSMString::QBSMString() = default;
QBSMString::QBSMString(const QBSMString&) = default;
QBSMString::QBSMString(QBSMString&&) = default;
QBSMString::~QBSMString() = default;
QBSMString::QBSMString(uint32_t id, ::std::string str) : m_id(id), m_str(::std::move(str)) {}
QBSMString::QBSMString(uint_fast8_t cat, uint_fast16_t id, BSMLANG lang, ::std::string str) : m_id(BSMString::bsmID(cat, id, lang)), m_str(::std::move(str)) {}

QBSMString& QBSMString::operator=(const QBSMString&) = default;
QBSMString& QBSMString::operator=(QBSMString&&) = default;

QBSMFile::LOAD_RESULT QBSMFile::loadRAW(const QByteArray & qba) {
    if (qba.size() < 0)
        return LR_NEG_DATA_SIZE;
    if (qba.size() == 0)
        return LR_EMPTY_FILE;
    ReadRAMFile file(qba.data(), (unsigned)qba.size());

    if (!file.CanRead(sizeof(BSMHeader)))
        return LR_FILE_TOO_SHORT;

    const BSMHeader& header = file.ForceRead<BSMHeader>();
    if (header.isValid())
        return LR_INVALID_HEADER;

    ::std::array<unsigned, 256u> languages;
    languages.fill(0u);

    ::std::vector<QBSMString> data;
    data.resize(header.strings);

    for (QBSMString & str : data) {
        if (!file.CanRead(sizeof(BSMString)))
            return LR_FILE_TOO_SHORT;

        const BSMString& str_header = file.ForceRead<BSMString>();

        if (!file.MoveCursor(sizeof(char) * str_header.len))
            return LR_FILE_TOO_SHORT;

        // Check if translation with same ID exists
        if (std::find_if(data.begin(), data.end(),
                         [str_header](const QBSMString & item) {
                             return item.bsmID() == str_header.bsm_id;
                         }) != data.cend())
            qWarning() << "QBSMFile::loadRAW Multiple strings with same id in file.";

        static constexpr auto bsmStrLen = [](const BSMString& str_header) -> size_t {
            size_t str_len = str_header.len - 1u;
            if (str_header.str[str_len] != '\0') {
                qWarning() << "QBSMFile::loadRAW Not null-terminated string in file";
                ++str_len;
            }
            return str_len;
        };

        ++languages[str_header.language()];
        str.setBSMID(str_header.bsm_id);
        str.setText(str_header.str, bsmStrLen(str_header));
    }

    m_data = ::std::move(data);
    m_languages = ::std::move(languages);

    return LR_SUCCEEDED;
}

template<typename T> static inline
QByteArray & QByteArray_Append(QByteArray & qba, const T& data) {
    return qba.append((char*)&data, sizeof(T));
}

QByteArray QBSMFile::saveRAW() const {
    QByteArray result;
    QByteArray_Append(result, BSMHeader::create(m_data.size()));

    for (const QBSMString & pd : m_data) {
        QByteArray_Append(result, BSMString{pd.bsmID(), (uint32_t)(pd.text().size() + 1), {}});
        result.append(pd.text().data(), sizeof(char) * pd.text().size());
        result.append('\0');
    }

    return result;
}

void QBSMFile::setData(::std::vector<QBSMString>&& data) noexcept {
    m_data = std::move(data);
    m_languages.fill(0);
    for (QBSMString & str : m_data)
        ++m_languages[str.language()];
}

::std::bitset<256> QBSMFile::availableLanguages() const noexcept {
    ::std::bitset<256> result;
    for (unsigned i = 0; i < 256; ++i)
        if (m_languages[i] > 0)
            result.set(i);
    return result;
}

unsigned QBSMFile::maxTranslationsAvailable() const noexcept {
    return *std::max_element(m_languages.cbegin(), m_languages.cend());
}