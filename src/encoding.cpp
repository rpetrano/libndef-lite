#include <codecvt>
#include <locale>

#include "encoding.hpp"

namespace encoding {
/// Converts string to UTF-8 string
/// \note This doesn't do anything, as std::string is already either UTF-8 or ASCII
std::string to_utf8(const std::string& src)
{
  // Return source
  return src;
}

/// Converts string to UTF-8 string from UTF-16 source
std::string to_utf8(const std::u16string& src)
{
  // Conversion from UTF-16 to UTF-8 from std::basic_string<char16_t> string
  std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv;
  return conv.to_bytes(src);
}

/// Converts string to UTF-8 string from UTF-32 source
std::string to_utf8(const std::u32string& src)
{
  // Conversion from UTF-32 to UTF-8 from std::basic_string<char32_t> string
  std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
  return conv.to_bytes(src);
}

/// Converts string to UTF-16 string from UTF-8/ASCII source
std::u16string to_utf16(const std::string& src)
{
  // Conversion from UTF-8/ASCII to UTF-16 from std::basic_string<char> string
  std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv;
  return conv.from_bytes(src);
}

/// Converts string to UTF-16 string from UTF-16
/// \note This doesn't do anything, as std::u16string is most likely already UTF-16
std::u16string to_utf16(const std::u16string& src) { return src; }

/// Converts string to UTF-16 string from UTF-32
std::u16string to_utf16(const std::u32string& src)
{
  // Conversion from UTF-32 to UTF-16 from std::basic_string<char32_t> string
  std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
  return to_utf16(to_utf8(src));
}

/// Converts byte vector to UTF-16 text
std::u16string to_utf16(const std::vector<uint8_t>& src)
{
  std::wstring_convert<std::codecvt_utf8_utf16<char16_t, 0x10ffff, std::codecvt_mode::consume_header>, char16_t> conv;
  return conv.from_bytes(std::string{ src.begin(), src.end() });
}

/// Converts UTF-16 string (std::u16string) to vector of uint8_t in Little Endian order
std::vector<uint8_t> to_utf16le_bytes(const std::u16string& src)
{
  std::wstring_convert<std::codecvt_utf8_utf16<char16_t, 0x10ffff, std::codecvt_mode::little_endian>, char16_t> conv;
  auto bytes = conv.to_bytes(src);
  return std::vector<uint8_t>{ bytes.begin(), bytes.end() };
}

/// Converts UTF-8 bytes to UTF-16 string
std::u16string from_utf16le_bytes(const std::vector<uint8_t>& src)
{
  // Convert bytes to string before converting from little endian bytes to UTF-16 string
  std::wstring_convert<std::codecvt_utf8_utf16<char16_t, 0x10ffff, std::codecvt_mode::little_endian>, char16_t> conv;
  return conv.from_bytes(std::string{ src.begin(), src.end() });
}

/// Checks for UTF Byte Order Mark in UTF-16 string
bool has_BOM(const std::vector<uint8_t>& bytes)
{
  return (bytes.at(0) == '\xff' && bytes.at(1) == '\xfe') || (bytes.at(0) == '\xff' && bytes.at(1) == '\xfe');
}

/// Checks for UTF Byte Order Mark in UTF-16 string
bool has_BOM(const std::string& text)
{
  return (text.at(0) == '\xff' && text.at(1) == '\xfe') || (text.at(0) == '\xff' && text.at(1) == '\xfe');
}
} // namespace encoding
