#ifndef RECORD_HEADER_H
#define RECORD_HEADER_H

#include "typeNameFormat.hpp"

namespace RecordHeader {
  /// NDEF Record Header
  struct RecordHeader {
    /// Type Name Format - identifies type of content the record contains
    TypeNameFormat::Type tnf;

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
  };

  bool inline operator==(const RecordHeader &lhs, const RecordHeader &rhs) {
    return (
      lhs.cf == rhs.cf &&
      lhs.il == rhs.il &&
      lhs.mb == rhs.mb &&
      lhs.me == rhs.me &&
      lhs.sr == rhs.sr &&
      lhs.tnf == rhs.tnf
    );
  }

  /// \param value octet (byte) of data to create RecordHeader object from
  /// \return ::RecordHeader object
  RecordHeader fromByte(const uint8_t value);

  /// \param header Reference to ::RecordHeader object
  /// \return byte representation of ::RecordHeader
  uint8_t headerAsByte(const RecordHeader &header);
}

#endif // RECORD_HEADER_H