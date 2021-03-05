#include "ndef-lite/encoding.hpp"
#include "ndef-lite/record.hpp"

using namespace std;

/// Generates the text record's initial bytes based on the contents passed
vector<uint8_t> NDEFRecord::init_text_record_payload(const vector<uint8_t>& textBytes, const string& locale,
                                                     RecordTextCodec codec)
{
  NDEFRecord record;
  vector<uint8_t> payload;

  // Locale string must be at most 5 characters
  const uint locale_length = min(locale.length(), static_cast<size_t>(5));

  // Reserve space required to speed up insertions later (status byte + locale string + text)
  payload.reserve(1 + locale_length + textBytes.size());

  // Set type as Text Well Known Type
  record.set_type(NDEFRecordType::text_record_type());

  // Combine codec encoding (bit 7) and locale length (bits 5..0) for first byte
  uint8_t statusByte = 0x00 | (static_cast<uint8_t>(codec) & 0x80) | (locale_length & 0x3f);
  payload.push_back(statusByte);

  // Add <=5 chars from locale string to payload
  payload.insert(payload.end(), locale.begin(), locale.begin() + locale_length);

  return payload;
}

/// Generates the text record's initial bytes based on the contents of the text string passed
/// \note wrapper around init_text_record_payload(const vector<uint8_t>&, const string&, RecordTextCodec)
vector<uint8_t> NDEFRecord::init_text_record_payload(const string& text, const string& locale, RecordTextCodec codec)
{
  return init_text_record_payload(vector<uint8_t>{ text.begin(), text.end() }, locale, codec);
}

/// Creates NDEF Text record from the UTF-8 string provided
NDEFRecord NDEFRecord::create_text_record(const string& text, const string& locale, RecordTextCodec codec)
{
  NDEFRecord record;
  vector<uint8_t> payload = init_text_record_payload(text, locale, codec);

  // Set type as Text Well Known Type
  record.set_type(NDEFRecordType::text_record_type());

  // Add text to payload
  payload.insert(payload.end(), text.begin(), text.end());
  record.set_payload(payload);

  return record;
}

/// Creates NDEF Text record from the UTF-16 string provided
/// \note wrapper around create_text_record(const string&, const string&, RecordTextCodec)
NDEFRecord NDEFRecord::create_text_record(const u16string& text, const string& locale)
{
  NDEFRecord record;

  if (encoding::has_BOM(text)) {
    // BOM already present, don't
    record = create_text_record(string{ text.begin(), text.end() }, locale, RecordTextCodec::UTF16);
  } else {
    // Encoding not specified, convert to UTF-8-sized bytes in Big Endian order
    record = create_text_record(encoding::to_utf8(text), locale, RecordTextCodec::UTF16);
  }

  return record;
}

/// Extracts the text locale in string from the payload vector passed
string NDEFRecord::get_text_locale(const vector<uint8_t>& payload)
{
  // Get the length of the locale string from the payload - max 5 characters
  const uint locale_length = min((payload.at(0) & 0x1f), 5);
  return string{ payload.begin() + 1, payload.begin() + 1 + locale_length };
}

/// Extracts stored text from the payload vector passed
string NDEFRecord::get_text(const vector<uint8_t>& payload)
{
  const uint status_byte = payload.at(0);
  const uint locale_length = status_byte & 0x1f;
  vector<uint8_t> record_bytes{ payload.begin() + 1 + locale_length, payload.end() };

  // Convert to UTF-8 string, handling UTF-16 if need be
  if (status_byte & static_cast<uint8_t>(RecordTextCodec::UTF16)) {
    return encoding::to_utf8(encoding::to_utf16(record_bytes));
  } else {
    return std::string{ record_bytes.begin(), record_bytes.end() };
  }
}

/// Extracts the text locale in string from the record object
string NDEFRecord::get_text_locale() const
{
  // Get the length of the locale string from the payload - max 5 characters
  const uint locale_length = min((this->payload_data.at(0) & 0x1f), 5);
  return string{ this->payload_data.begin() + 1, this->payload_data.begin() + 1 + locale_length };
}

/// Extracts stored text from record
string NDEFRecord::get_text() const
{
  const uint status_byte = this->payload_data.at(0);
  const uint locale_length = status_byte & 0x1f;
  vector<uint8_t> record_bytes{ this->payload_data.begin() + locale_length, this->payload_data.end() };

  // Convert to UTF-8 string, handling UTF-16 if need be
  if (status_byte & static_cast<uint8_t>(RecordTextCodec::UTF16)) {
    return encoding::to_utf8(encoding::to_utf16(record_bytes));
  } else {
    return std::string{ record_bytes.begin(), record_bytes.end() };
  }
}
