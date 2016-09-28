/*!
 * @file BSMFile.det.hpp
 *
 * @author Karol Rudnik
 * @date 27-04-2016
 *
 * TODO: Update documentation
 */

#ifndef _BSM_INSIDE_HPP_
#define _BSM_INSIDE_HPP_

#include <cstdint>

enum BSMLANG : ::std::uint8_t {
    BSML_ENGLISH     = 0x01u, // Angielski
    BSML_FRENCH      = 0x02u, // Francuski
    BSML_GERMAN      = 0x04u, // Niemiecki
    BSML_DUTCH       = 0x08u, // Holenderski
    BSML_SPANISH     = 0x10u, // Hiszpański
    BSML_ITALIAN     = 0x20u, // Włoski
    BSML_PORTUGUESE  = 0x40u  // Portugalski
};
extern const char* BSMLANG_toString(BSMLANG lang) noexcept;

#pragma pack(push, 1)
struct BSMHeader {
    char magic[12];          //!< "GREETINGV1.0"
    ::std::uint32_t strings; //!< Data count

    bool isValid() const noexcept; //!< Check if magic number is valid
    static BSMHeader create(::std::uint32_t strings) noexcept; //!< Create struct with valid magic number
};

struct BSMString {
    ::std::uint32_t bsm_id;  //!< Internal string identifier
    ::std::uint32_t len;     //!< String length
    char str[0];             //!< String, probably encoded in Windows-1252

    ::std::uint8_t  category() const noexcept;
    ::std::uint16_t identifier() const noexcept;
    BSMLANG         language() const noexcept;

    static ::std::uint8_t  category(::std::uint32_t bsm_id) noexcept;
    static ::std::uint16_t identifier(::std::uint32_t bsm_id) noexcept;
    static BSMLANG         language(::std::uint32_t bsm_id) noexcept;

    static ::std::uint32_t bsmID(::std::uint8_t category,
                                 ::std::uint16_t identifier,
                                 BSMLANG lang) noexcept;
};
#pragma pack(pop)

inline ::std::uint8_t  BSMString::category()   const noexcept {
    return category(bsm_id);
}
inline ::std::uint16_t BSMString::identifier() const noexcept {
    return identifier(bsm_id);
}
inline BSMLANG BSMString::language()   const noexcept {
    return language(bsm_id);
}

inline ::std::uint8_t BSMString::category(::std::uint32_t bsm_id) noexcept {
    return (bsm_id >> 24u) & 0xFF;
}
inline ::std::uint16_t BSMString::identifier(::std::uint32_t bsm_id) noexcept {
    return (bsm_id >> 8u) & 0xFFFF;
}
inline BSMLANG BSMString::language(::std::uint32_t bsm_id) noexcept {
    return (BSMLANG)(bsm_id & 0xFF);
}

inline ::std::uint32_t BSMString::bsmID(::std::uint8_t cat,
                                        ::std::uint16_t id,
                                        BSMLANG lang) noexcept {
    return   ((::std::uint32_t)cat << 24u)
           | ((::std::uint32_t)id << 8u)
           | ((::std::uint32_t)lang << 0u);
}

#include <QString>
extern QString BSMLANG_toQString(BSMLANG lang);

#endif // !_BSM_INSIDE_HPP_