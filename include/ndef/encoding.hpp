#ifndef UTF_HPP
#define UTF_HPP

#include <codecvt>
#include <string>
#include <vector>

namespace encoding {
const uint8_t BOM_BE_1ST = '\xff';
const uint8_t BOM_BE_2ND = '\xfe';

enum class Endian { BigEndian, LittleEndian };

/// \return Endian variant indicating system endiannes
Endian system_endianness();

/// \param to_swap vector of bytes to be swapped
/// \return boolean indicating success of swap
bool swap_bytes(std::vector<uint8_t>& to_swap);

/// \param src std::string source to convert
/// \return std::string encoded in UTF-8
std::string to_utf8(const std::string& src);

/// \param src std::u16string source to convert
/// \return std::string encoded in UTF-8
std::string to_utf8(const std::u16string& src);

// \param src std::string source to convert
/// \return std::u16string encoded in UTF-16
std::u16string to_utf16(const std::string& src);

/// \param src std::u16string source to convert
/// \return std::u16string encoded in UTF-16
std::u16string to_utf16(const std::u16string& src);

/// \param src std::vector<uint8_t> source bytes to convert
/// \return std::u16string encoded in UTF-16
std::u16string to_utf16(const std::vector<uint8_t>& src);

/// \param src std::u16string source to convert
/// \param endian Endian variant to convert to
/// \return vector of bytes in order specified, or empty vector if the endian swap fails
std::vector<uint8_t> to_utf16_bytes(const std::u16string& src, const Endian& endian = Endian::LittleEndian);

/// \param src std::u16string source to convert
/// \return std::vector<uint8_t> of UTF-16LE bytes
std::vector<uint8_t> to_utf16le_bytes(const std::u16string& src);

/// \param src std::u16string source to convert
/// \return std::vector<uint8_t> of UTF-16BE bytes
std::vector<uint8_t> to_utf16be_bytes(const std::u16string& src);

/// \param src byte vector source to convert
/// \return std::u16string of text
std::u16string from_utf16_bom(const std::vector<uint8_t>& src);

/// \param text string to check. Should be a UTF-16 string
/// \return boolean indicating whether this string has the UTF BOM
bool has_BOM(const std::u16string& text);

/// \param bytes vector of bytes to check. Should be containing UTF-16 string
/// \return boolean indicating whether this string has the UTF BOM
bool has_BOM(const std::vector<uint8_t>& bytes);
} // namespace encoding

#endif // UTF_HPP