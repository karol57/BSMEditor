#include <cstring>
#include <utility>

#include "BSMFile.det.hpp"

const char* BSMLANG_toString(BSMLANG lang) noexcept {
    switch (lang) {
        case BSML_ENGLISH:    return "English";
        case BSML_FRENCH:     return "French";
        case BSML_GERMAN:     return "German";
        case BSML_DUTCH:      return "Dutch";
        case BSML_SPANISH:    return "Spanish";
        case BSML_ITALIAN:    return "Italian";
        case BSML_PORTUGUESE: return "Portuguese";
    }
    return nullptr;
}

const char g_bsmMagic[12] = { 'G','R','E','E','T','I','N','G','V','1','.','0' };
static_assert(sizeof(BSMHeader::magic) == sizeof g_bsmMagic, "Internal error");

bool BSMHeader::isValid() const noexcept {
    return memcmp(magic, g_bsmMagic, sizeof(g_bsmMagic)) != 0;
}

BSMHeader BSMHeader::create(::std::uint32_t strings) noexcept {
    BSMHeader result;
    memcpy(result.magic, g_bsmMagic, sizeof(g_bsmMagic));
    result.strings = strings;
    return ::std::move(result);
}

#include <QApplication>
QString BSMLANG_toQString(BSMLANG lang) {
    const char * lang_name = BSMLANG_toString(lang);
    return lang_name ? QApplication::translate("BSMLANG", lang_name) :
                       QString("0x%1").arg((int)lang, 2, 16, QLatin1Char('0'));
}