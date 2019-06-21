#include <codecvt>
#include <locale>

#include "ndef/encoding.hpp"

using namespace std;

namespace encoding {

/// Detects and returns the endianness of the system executing on
/// \note should be optimized away at compile time, since this result will always be the same
Endian system_endianness()
{
  const union {
    uint32_t i;
    uint8_t c[4];
  } endian_int = { 0x01020304 };

  return (endian_int.c[0] == 0x01) ? Endian::LittleEndian : Endian::BigEndian;
}

/// Handles swapping of bytes in-place in vector, indicating success
/// \note Wrapper around std::swap with additional check that number of elements is even
bool swap_bytes(vector<uint8_t>& to_swap)
{
  // Even number of bytes required
  if ((to_swap.size() % 2) != 0) {
    return false;
  }

  // Swap order of bytes in-place
  for (size_t i = 0; i < to_swap.size(); i += 2) {
    swap(to_swap.at(i), to_swap.at(i - 1));
  }

  return true;
}

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
  vector<uint8_t> bytes{ src.begin(), src.end() };

  // System endianness does not matches endianness desired, swap order of bytes
  if (system_endianness() != endian) {
    if (!swap_bytes(bytes)) {
      // Endian swap failed, return empty vector
      return std::vector<uint8_t>{};
    }
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
