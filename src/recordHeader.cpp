#include "recordHeader.hpp"
#include "typeNameFormat.hpp"

namespace RecordHeader {
  /// Create a new RecordHeader object from an byte of data
  RecordHeader fromByte(uint8_t value) {
    return RecordHeader {
      mb: (value & 0x80) != 0,
      me: (value & 0x40) != 0,
      cf: (value & 0x20) != 0,
      sr: (value & 0x10) != 0,
      il: (value & 0x08) != 0,
      // Take last 3 bits and retrieve matching TypeName
      tnf: TypeNameFormat::fromByte(value & 0x07)
    };
  }

  /// Creates a byte representation of the RecordHeader object passed
  uint8_t headerAsByte(RecordHeader &header) {
    uint8_t byte = 0x00;

    if (header.mb) {
      byte |= 0x80;
    }
    if (header.me) {
      byte |= 0x40;
    }
    if (header.cf) {
      byte |= 0x20;
    }
    if (header.sr) {
      byte |= 0x10;
    }
    if (header.il) {
      byte |= 0x08;
    }
    byte |= TypeNameFormat::asByte(header.tnf);

    return byte;
  }
}