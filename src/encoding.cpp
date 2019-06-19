#include <codecvt>
#include <locale>

#include "encoding.hpp"

using namespace std;

namespace encoding {

const uint8_t BOM_BE_1ST = '\xff';
const uint8_t BOM_BE_2ND = '\xfe';

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

/// Converts UTF-16 string (u16string) to vector of uint8_t in Little Endian order
vector<uint8_t> to_utf16le_bytes(const u16string& src)
{
  using namespace std;

  // Convert u16string
  wstring_convert<codecvt_utf8_utf16<char16_t, 0x10ffff, codecvt_mode::little_endian>, char16_t> conv;
  auto bytes = conv.to_bytes(src);
  return vector<uint8_t>{ bytes.begin(), bytes.end() };
}

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
