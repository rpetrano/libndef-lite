/*! Implementation of the record component of the NDEF standard
 * \file ndefRecord.hpp
 */

#ifndef NDEF_H
#define NDEF_H

#include <string>
#include <vector>

#include "recordType.hpp"
#include "util.hpp"

// NDEF Text record is encoded with either UTF8 or UTF16, this is part of a byte set in the first bits of the
// payload
enum class RecordTextCodec { UTF8 = 0x00, UTF16 = 0x80 };

/// NDEF Record struct
class NDEFRecord {
public:
  NDEFRecord();
  NDEFRecord(const NDEFRecordType& type, const std::vector<uint8_t>& payload = std::vector<uint8_t>{},
             bool chunked = false);
  ~NDEFRecord() = default;

  void validate();

  // Conversion helpers

  /// \return vector of uint8 byte values
  std::vector<uint8_t> as_bytes() const;

  /// \param bytes array of bytes (uint8_t) that will be used to attempt to create an NDEFRecord object
  /// \param len number of elements in \p bytes array
  /// \param offset byte offset to start from
  /// \return Record object created from bytes
  static NDEFRecord from_bytes(uint8_t bytes[], size_t len, size_t offset = 0);

  /// \param bytes vector of bytes (uint8_t) that will be used to attempt to create an NDEFRecord object
  /// \param offset byte offset to start from
  /// \return NDEFRecord object created from bytes
  static NDEFRecord from_bytes(std::vector<uint8_t> bytes, size_t offset = 0);

  // Accessors/Mutators
  void set_id(const std::string& newId) { this->idField = newId; }
  std::string id() const { return this->idField; }

  void constexpr set_type(const NDEFRecordType& type) { this->recordType = type; }
  NDEFRecordType type() const { return this->recordType; }

  void constexpr set_chunked(bool flag) { this->chunked = flag; }
  bool constexpr is_chunked() const { return this->chunked; }

  void set_payload(const std::vector<uint8_t>& data);
  void append_payload(const std::vector<uint8_t>& data);
  std::vector<uint8_t> payload() const { return this->payloadData; }

  /// Access number of bytes in the payload
  /// \return size_t number of bytes in the payload
  size_t constexpr payload_length() const { return this->payloadData.size(); }

  // General information
  uint8_t header() const;

  bool constexpr is_short() const { return (this->payload_length() < 256); }
  bool constexpr is_empty() const { return (this->recordType.id() == NDEFRecordType::TypeID::Empty); }
  bool constexpr is_valid() const { return (this->recordType.id() != NDEFRecordType::TypeID::Invalid); }

  // Record creation helpers

  // Text records

  /// \param text std::u16string to be converted
  /// \param locale string's locale. Should be kept <= 5 characters, which is the max limit set by the NDEF standard
  /// \return NDEFRecord object with \p text encoded in UTF-16
  static NDEFRecord create_text_record(const std::u16string& text, const std::string& locale);

  /// \param text string to be converted
  /// \param locale string's locale. Should be kept <= 5 characters, which is the max limit set by the NDEF standard
  /// \param codec RecordTextCodec enum variant representing whether this is a UTF-8 or UTF-16 encoded string
  /// \return NDEFRecord object with \p text encoded in whatever is specified by \p codec
  static NDEFRecord create_text_record(const std::string& text, const std::string& locale,
                                       RecordTextCodec codec = RecordTextCodec::UTF8);

  /// \param payload vector of bytes to have locale extracted from
  /// \return ASCII string representation of locale in payload
  static std::string text_locale(const std::vector<uint8_t>& payload);

  /// \param payload vector of bytes to have locale extracted from
  /// \return string UTF-8 encoded string of record's contents
  static std::string text_from_text_payload(const std::vector<uint8_t>& payload);

  // URI Records

  /// \param uri string of URI to encode
  /// \return NDEFRecord object with URI encoded
  static NDEFRecord create_uri_record(const std::string& uri);

  /// \param payload vector of bytes to have URI protocol extracted from
  /// \return string UTF-8 encoded string of record's URI protocol
  static std::string get_uri_protocol(const std::vector<uint8_t>& payload);

  /// \param payload vector of bytes to have URI extracted from
  /// \return string UTF-8 encoded string of record's URI
  static std::string get_uri(const std::vector<uint8_t>& payload);

private:
  // NDEF Record Fields

  /// Specifies record type. Must follow the structure, encoding, and format implied by the value of the TNF field.
  /// If a NDEF record is received with a supported TNF field but unknown TYPE field value, the type identifier will
  /// be treated as if the TNF field were `0x05` (Unknown)
  ///
  /// For example, if the TNF is set to 0x01 the record type might be `T` to indicate a text message, `U` for a URI
  /// message. If the TNF is set 0x02 then the record type might be one of "text/json", "text/plain", "image/png",
  /// etc.
  NDEFRecordType recordType;

  /// ID field
  ///
  /// Only included if the `IL` flag is set in the record header and the ID_LENGTH field is > 0
  std::string idField;

  /// Payload - A slice of octets, the length of which is retrievable via ::payload_length()
  std::vector<uint8_t> payloadData;

  /// Whether this is a part of a chunked record or not
  bool chunked;

  // Helper functionality

  /// \param textBytes const vector reference of text bytes
  /// \param locale string's locale. Should be kept <= 5 characters, which is the max limit set by the NDEF standard
  /// \param codec RecordTextCodec enum variant representing whether this is a UTF-8 or UTF-16 encoded string
  /// \return vector of bytes with encoding/locale bytes configured
  static std::vector<uint8_t> init_text_record_payload(const std::vector<uint8_t>& textBytes, const std::string& locale,
                                                       RecordTextCodec codec);

  /// \param text const vector reference of text bytes
  /// \param locale string's locale. Should be kept <= 5 characters, which is the max limit set by the NDEF standard
  /// \param codec RecordTextCodec enum variant representing whether this is a UTF-8 or UTF-16 encoded string
  /// \return vector of bytes with encoding/locale bytes configured
  static std::vector<uint8_t> init_text_record_payload(const std::string& text, const std::string& locale,
                                                       RecordTextCodec codec);

  // RELEASE: Below kept for reference. Will be removed for release
  /// NDEF Record header object
  // NDEFRecordHeader::RecordHeader header;

  /// Specifies length of payload type in octets
  //   uint8_t typeLength;

  /// Specifies length of payload in octets
  ///
  /// If `SR` flag is set in the record header then this value will be 1 byte, otherwise it will be a 32-bit value
  //   uint32_t payloadLength;

  /// Specifies length of ID field in octets
  ///
  /// Only included if the `IL` flag is set in the record header, otherwise omitted entirely
  //   uint8_t idLength;
};

#endif // NDEF_H
