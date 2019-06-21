#include "catch.hpp"
#include "ndef/encoding.hpp"

using namespace std;

TEST_CASE("std::string to_utf8(std::string)", "[encodingUTF8ToUTF8string]")
{
  // Source and output should be exactly the same
  string source = "test123";

  REQUIRE(encoding::to_utf8(source) == source);
}

TEST_CASE("std::string to_utf8(std::u16string) funky", "[encodingUTF16ToUTF8string-funky]")
{
  // Source and output should be exactly the same
  u16string source = u"𐌄𐌈ﺕ";
  string expected = u8"𐌄𐌈ﺕ";

  REQUIRE(encoding::to_utf8(source) == expected);
}

TEST_CASE("std::string to_utf8(std::u16string) english", "[encodingUTF16ToUTF8string-english]")
{
  // Source and output should be exactly the same
  u16string source = u"testing123";
  string expected = u8"testing123";

  REQUIRE(encoding::to_utf8(source) == expected);
}

TEST_CASE("std::string to_utf16(std::string)", "[encodingUTF8ToUTF16string]")
{
  // Source and output should be exactly the same
  string source = u8"𐌄𐌈ﺕ";
  u16string expected = u"𐌄𐌈ﺕ";

  REQUIRE(encoding::to_utf16(source) == expected);
}

TEST_CASE("std::string to_utf16(std::u16string)", "[encodingUTF16ToUTF16string]")
{
  // Source and output should be exactly the same
  u16string source = u"𐌄𐌈ﺕ";

  REQUIRE(encoding::to_utf16(source) == source);
}

// NOTE: Disabled for now until we can sort out handling BOM appropriately
// TEST_CASE("std::string to_utf16(std::vector<uint8_t> Big Endian)", "[encodingBEBytesToUTF16string]")
// {
//   vector<uint8_t> source{ 0xfe, 0xff, 0xf0, 0x90, 0x8a, 0x99 };
//   u16string expected = u"𐊙";
//   u16string result;

//   try {
//     result = encoding::to_utf16(source);
//   } catch (const std::exception& ex) {
//     WARN(ex.what());
//   }

//   REQUIRE(result == expected);
// }

TEST_CASE("std::string to_utf16(std::vector<uint8_t>) english", "[encodingBytesToUTF16string-english]")
{
  vector<uint8_t> source{ 0x74, 0x65, 0x73, 0x74 };
  u16string expected = u"test";

  REQUIRE(encoding::to_utf16(source) == expected);
}

TEST_CASE("std::string to_utf16le_bytes(std::u16string)", "[encodingUTF16ToUTF16LE]")
{
  u16string source = u"𐀤𐀔";
  vector<uint8_t> expected{ 0x00, 0xd8, 0x24, 0xdc, 0x00, 0xd8, 0x14, 0xdc };
  vector<uint8_t> result = encoding::to_utf16le_bytes(source);

  REQUIRE(result == expected);
}

TEST_CASE("std::string to_utf16be_bytes(std::u16string)", "[encodingUTF16ToUTF16LE]")
{
  u16string source = u"𐀤𐀔";
  vector<uint8_t> expected{ 0xd8, 0x00, 0xdc, 0x24, 0xd8, 0x00, 0xdc, 0x14 };
  vector<uint8_t> result = encoding::to_utf16be_bytes(source);

  REQUIRE(result == expected);
}

TEST_CASE("UTF-16 string has BOM", "[utf16HasBOM]")
{
  u16string source = u"testing123";
  source.insert(source.begin(), { 0xfe, 0xff });

  REQUIRE(encoding::has_BOM(source));
}

TEST_CASE("UTF-16 string no BOM", "[utf16NoBOM]")
{
  u16string source = u"testing123";

  REQUIRE_FALSE(encoding::has_BOM(source));
}

TEST_CASE("UTF-16 bytes no BOM", "[utf16BytesNoBOM]")
{
  u16string src_text = u"testing123";
  vector<uint8_t> source{ src_text.begin(), src_text.end() };

  REQUIRE_FALSE(encoding::has_BOM(source));
}

TEST_CASE("UTF-16 bytes has BOM", "[utf16BytesHasBOM]")
{
  u16string src_text = u"testing123";
  vector<uint8_t> source{ 0xff, 0xfe };
  source.insert(source.end(), src_text.begin(), src_text.end());

  REQUIRE(encoding::has_BOM(source));
}