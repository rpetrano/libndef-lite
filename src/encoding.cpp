#include <codecvt>
#include <locale>

#include "ndef/encoding.hpp"

using namespace std;

namespace encoding {

// In theory, this union and system_endianness/all calls to it should be optimized away at compile-time.
// I haven't checked to confirm this
constexpr union {
  uint32_t i;
  uint8_t c[4];
} endian_int = { 0x01020304 };

/// Detects and returns the endianness of the system executing on
/// \note should be optimized away at compile time, since this result will always be the same
constexpr Endian system_endianness() { return (endian_int.c[0] == 0x01) ? Endian::BigEndian : Endian::LittleEndian; }

/// Converts string to UTF-8 string
/// \note This doesn't do anything, as string is already either UTF-8 or ASCII
string to_utf8(const string& src)
{
  // Return source
  return src;
}

/// Converts string to UTF-8 string from UTF-16 source
string to_utf8(const u16string& src)
{
  // Conversion from UTF-16 to UTF-8 from basic_string<char16_t> string
  wstring_convert<codecvt_utf8_utf16<char16_t>, char16_t> conv;
  return conv.to_bytes(src);
}

/// Converts string to UTF-16 string from UTF-8/ASCII source
u16string to_utf16(const string& src)
{
  // Conversion from UTF-8/ASCII to UTF-16 from basic_string<char> string
  wstring_convert<codecvt_utf8_utf16<char16_t>, char16_t> conv;
  return conv.from_bytes(src);
}

/// Converts string to UTF-16 string from UTF-16
/// \note This doesn't do anything, as u16string is most likely already UTF-16
u16string to_utf16(const u16string& src) { return src; }

/// Converts byte vector to UTF-16 text
u16string to_utf16(const vector<uint8_t>& src)
{
  wstring_convert<codecvt_utf8_utf16<char16_t, 0x10ffff, codecvt_mode::consume_header>, char16_t> conv;
  return conv.from_bytes(string{ src.begin(), src.end() });
}

/// Converts std::u16string to array of bytes in specified endianness
vector<uint8_t> to_utf16_bytes(const u16string& src, const Endian& endian)
{
  // Create byte vector from u16string contents, no adjustment
  vector<uint8_t> bytes;
  const bool endian_match = (system_endianness() == endian);

  for (auto&& byte : src) {
    /* If the system endianness doesn't match the desired output endianness then the byte order must be reversed.
     *
     * Eg. If on a little endian system and big endian is wanted the bytes must be switched:
     * the first 8 bits (byte) put on the array will be the high 8 bytes in the char16_t shifted 8 places, then the
     * second 8 bits (byte) will be the low 8
     *
     * Little endian char16_t: 0xabcd => Big endian uint8_t[2]: { 0xcd, 0xab }
     */
    const uint shift_first = (endian_match) ? 8 : 0;
    const uint shift_second = (endian_match) ? 0 : 8;

    bytes.push_back(static_cast<uint8_t>(byte >> shift_first));
    bytes.push_back(static_cast<uint8_t>(byte >> shift_second));
  }

  return bytes;
}

/// Converts UTF-16 string (u16string) to vector of uint8_t in Little Endian order
/// \note Wrapper around to_utf16_bytes, specifying Little Endian endianness
vector<uint8_t> to_utf16le_bytes(const u16string& src) { return to_utf16_bytes(src, Endian::LittleEndian); }

/// Converts UTF-16 string (u16string) to vector of uint8_t in Big Endian order
/// \note Wrapper around to_utf16_bytes, specifying Big Endian endianness
vector<uint8_t> to_utf16be_bytes(const u16string& src) { return to_utf16_bytes(src, Endian::BigEndian); }

/// Checks for UTF Byte Order Mark in UTF-16 string
bool has_BOM(const vector<uint8_t>& bytes)
{
  return (static_cast<uint8_t>(bytes.at(0)) == BOM_BE_1ST && static_cast<uint8_t>(bytes.at(1)) == BOM_BE_2ND) ||
         (static_cast<uint8_t>(bytes.at(0)) == BOM_BE_2ND && static_cast<uint8_t>(bytes.at(1)) == BOM_BE_1ST);
}

/// Checks for UTF Byte Order Mark in UTF-16 string
bool has_BOM(const u16string& text)
{
  return (static_cast<uint8_t>(text.at(0)) == BOM_BE_1ST && static_cast<uint8_t>(text.at(1)) == BOM_BE_2ND) ||
         (static_cast<uint8_t>(text.at(0)) == BOM_BE_2ND && static_cast<uint8_t>(text.at(1)) == BOM_BE_1ST);
}
} // namespace encoding
