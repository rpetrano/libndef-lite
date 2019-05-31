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

  /// \param value octet (byte) of data to create RecordHeader object from
  /// \return ::RecordHeader object
  RecordHeader fromByte(uint8_t value);

  /// \param header Reference to ::RecordHeader object
  /// \return byte representation of ::RecordHeader
  uint8_t headerAsByte(RecordHeader &header);
}



#endif // RECORD_HEADER_H