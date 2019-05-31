/*! A pure C++ implementation of the NDEF standard
 * \file ndef.cpp
 * \author Nathanael Olander
 * \version 0.0.1
 * \date 2019-05-31
 * \mainpage NDEF Message Handling
 *
 * \section intro_section Introduction to the NDEF message format
 * An NDEF message is a sequence of bytes stored on an NFC tag. The NFC Forum standard defines the protocol in terms of
 * octets, which we implement with the C++ type `uint8_t` and may synonymously refer to as bytes since most platforms
 * have 8 bit bytes.
 *
 * Much of the documentation here is summarized, with segments of copied verbatim, from the NFC Forum NDEF
 * specification.
 *
 * \section contents_structure Message Contents Structure
 * The following fields are listed in the order they must appear in the NDEF record
 * 
 * - [Message Begin](RecordHeader::mb) - 1 bit (bit 7)
 *     - Required
 *     - Indicates if this is the start of an NDEF message
 * - [Message End](struct.RecordHeader.html#structfield.me) - 1 bit (bit 6)
 *     - Required
 *     - Indicates if this is the last record in the message
 * - [Chunk Flag](struct.RecordHeader.html#structfield.cf) - 1 bit (bit 5)
 *     - Required
 *     - Indicates if this the first record chunk or in the middle
 * - [Short Record Flag](struct.RecordHeader.html#structfield.sr) - 1 bit (bit 4)
 *     - Required
 *     - Indicates if PAYLOAD LENGTH field is short (1 byte, 0-255) or longer
 * - [IL Flag](struct.RecordHeader.html#structfield.il) - 1 bit (bit 3)
 *     - Required
 *     - Indicates whether ID Length Field is present
 * - [Type Name Format](enum.TypeNameFormat.html) - 3 bits (bits 2-0)
 *     - Required
 *     - Can be 0
 * - [Type Length](struct.Record.html#structfield.type_length) - 1 byte
 *     - Will always be 0 for certain values of TNF field
 *     - Specifies length, in octets, of the ID field
 * - [Payload length](struct.Record.html#structfield.payload_length) - 1 byte or 4 bytes
 *     - Required
 *     - Can be 0
 *     - If `SR` flag is set in the record header then this value will be 1 byte, otherwise it will be a 4 byte value
 * - [ID Length](struct.Record.html#structfield.id_length) - 1 byte
 *     - Can be 0 (will result in omission of [ID field](struct.Record.html#structfield.id_field))
 *     - If `IL` flag is set in the record header then this value will be 1 byte in length, otherwise it will be
 *         omitted
 * - [Type](struct.Record.html#structfield.record_type) - length in bytes defined by Type Length field
 *     - Identifier that describes the contents of the payload
 *     - Formed from characters in US ASCII set, characters in range [0-31] and 127 are invalid and SHALL NOT be used
 * - [ID](struct.Record.html#structfield.id_field) - length in bytes defined by ID Length field
 *     - Relative or absolute URI identifier
 *     - Omitted if [ID length](struct.Record.html#structfield.id_length) is 0
 * - [Payload](struct.Record.html#structfield.payload) - length in bytes defined by Payload Length field
 *     - Data in this field is opaque to the library and will be merely passed along
 * 
 * \bug No known bugs
 */

#include <array>

#include "ndef.hpp"

namespace ndef {
  // Allows us to convert from the raw bytes collected from C (converted to u8) into a Record struct
  Record fromBytes(std::array<uint8_t> bytes) {
    
  }
}
