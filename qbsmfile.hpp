#ifndef QBSMFILE_HPP
#define QBSMFILE_HPP

#include <cstdint>

#include <array>
#include <bitset>
#include <string>
#include <limits>
#include <vector>

#include <QByteArray>

#include <BSMFile.det.hpp>

class QBSMString {
    public:
        QBSMString();
        QBSMString(const QBSMString&);
        QBSMString(QBSMString&&);
        ~QBSMString();

        QBSMString(uint32_t identifier, ::std::string text);
        QBSMString(uint_fast8_t category, uint_fast16_t identifier,
                   BSMLANG language, ::std::string text);

        QBSMString& operator=(const QBSMString&);
        QBSMString& operator=(QBSMString&&);

        const uint32_t & bsmID() const noexcept;
        uint_fast8_t category() const noexcept;
        uint_fast16_t identiefier() const noexcept;
        BSMLANG language() const noexcept;
        const ::std::string & text() const noexcept;

        void setBSMID(uint32_t bsmID) noexcept;
        template<typename... Args>
        void setText(Args... args) noexcept;
    private:
        uint32_t m_id;
        ::std::string m_str;
};

class QBSMFile {
    public:
        enum LOAD_RESULT : int {
            LR_NEG_DATA_SIZE = std::numeric_limits<int>::min(),
            LR_EMPTY_FILE,
            LR_FILE_TOO_SHORT,
            LR_INVALID_HEADER,

            LR_SUCCEEDED = 0,
        };

        LOAD_RESULT loadRAW(const QByteArray & data);
        QByteArray  saveRAW() const;

        const ::std::vector<QBSMString>& data() const noexcept;
        void setData(::std::vector<QBSMString>&& data) noexcept;

        ::std::bitset<256> availableLanguages() const noexcept;
        unsigned maxTranslationsAvailable() const noexcept;
    private:
        ::std::vector<QBSMString> m_data;         // Translation data
        ::std::array<unsigned, 256u> m_languages; // Count of available strings for each language
};

inline const uint32_t & QBSMString::bsmID() const noexcept { return m_id; }
inline uint_fast8_t QBSMString::category() const noexcept {
    return BSMString::category(m_id);
}
inline uint_fast16_t QBSMString::identiefier() const noexcept {
    return BSMString::identifier(m_id);
}
inline BSMLANG QBSMString::language() const noexcept {
    return BSMString::language(m_id);
}
inline const ::std::string & QBSMString::text() const noexcept { return m_str; }

inline void QBSMString::setBSMID(uint32_t bsmID) noexcept { m_id = bsmID; }
template<typename... Args>
inline void QBSMString::setText(Args... args) noexcept { m_str.assign(args...); }

inline const ::std::vector<QBSMString>& QBSMFile::data() const noexcept {
    return m_data;
}

#endif // QBSMFILE_HPP