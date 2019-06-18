#ifndef RECORD_HEADER_H
#define RECORD_HEADER_H

#include "recordType.hpp"

/// NDEF Record type binary flags in header
enum class RecordFlag {
  IL = 0x08, // ID_LENGTH is present.
  SR = 0x10, // Short record.
  CF = 0x20, // Chunk flag.
  ME = 0x40, // Message end.
  MB = 0x80  // Message begin.
};

/// NDEF Record Header
struct NDEFRecordHeader
{
  /// Type Name Format - identifies type of content the record contains
  NDEFRecordType::TypeID tnf;

  /// ID Length Flag - Indicates whether ID Length Field is present
  bool il;

  /// Short Record Flag - Indicates if PAYLOAD LENGTH field is 1 byte (0-255) or less
  bool sr;

  /// Chunk Flag - Indicates if this the first record chunk or in the middle
  bool cf;

  /// Message End Flag - Indicates if this is the last record in the message
  bool me;

  /// Message Begin - Indicates if this is the start of an NDEF message
  bool mb;

  // Methods

  /// \param value octet (byte) of data to create NDEFRecordHeader object from
  /// \return ::NDEFRecordHeader object
  static NDEFRecordHeader from_byte(const uint8_t value);

  /// \return byte representation of ::NDEFRecordHeader
  uint8_t asByte();

  bool inline constexpr operator==(const NDEFRecordHeader& rhs) const
  {
    // clang-format off
    return (
      this->cf == rhs.cf && 
      this->il == rhs.il &&
      this->mb == rhs.mb &&
      this->me == rhs.me &&
      this->sr == rhs.sr &&
      this->tnf == rhs.tnf
    );
    // clang-format on
  }
};

#endif // RECORD_HEADER_H