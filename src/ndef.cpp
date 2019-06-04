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

#include <cassert>
#include <deque>
#include <iostream>
#include <string>

#include "exceptions.hpp"
#include "ndef.hpp"
#include "span.hpp"
#include "util.hpp"

namespace ndef {
  using namespace std;
  using namespace util;

  /// Wrapper around fromByte(vector<uint8_t>) that converts the array to a vector
  Record recordFromBytes(uint8_t bytes[], uint64_t size) {
    // Create vector from byte array
    vector<uint8_t> bytesVec(bytes, bytes + size);

    return recordFromBytes(bytesVec);
  }

  /// Wrapper around fromByte(vector<uint8_t>) that converts the span array to a vector
  Record recordFromBytes(span<uint8_t> bytes) {
    // Create vector from byte array
    auto bytesPtr = bytes.data();
    vector<uint8_t> bytesVec(bytesPtr, bytesPtr + bytes.size());

    return recordFromBytes(bytesVec);
  }

  /// Allows us to convert from the raw bytes from the NFC tag into a Record struct
  Record recordFromBytes(vector<uint8_t> bytes) {
    if (bytes.size() < 4) {
      // There are at least 4 required octets (field)
      throw NDEFException("Invalid number of octets, must have at least 4");
    }

    // Create deque to allow pop_front
    deque<uint8_t> vals(
      make_move_iterator(bytes.begin()),
      make_move_iterator(bytes.end())
    );

    // Read first byte into flags and TNF bits - first two bytes are sure to exist due to size check at start
    auto header = RecordHeader::fromByte(popFront(vals));
    uint8_t typeLength = popFront(vals);
    
    uint32_t payloadLength;
    if (header.sr) {
      payloadLength = popFront(vals);
    } else {
      assertHasValues(vals, 4, "payload length");

      // Create uint32 from next four bytes
      uint8_t payloadLenBytes[4] {
        popFront(vals), popFront(vals), popFront(vals), popFront(vals)
      };
      payloadLength = uint32FromBEBytes(payloadLenBytes);
    }

    uint8_t idLength = 0;
    if (header.il) {
      assertHasValue(vals, "ID length");
      idLength = popFront(vals);
    }

    // Confirm there are enough bytes to complete type field then populate type characters from bytes
    assertHasValues(vals, typeLength, "type length");
    vector<uint8_t> typeBytes = drainDeque(vals, typeLength);

    // Create the type field from the bytes, converting them into ASCII characters after validating them
    string typeField;
    for (auto chr : typeBytes) {
      if (chr < 31 || chr == 127) {
        // Invalid character, no ASCII characters [0-31] or 127
        throw NDEFException("Invalid character code " + to_string(chr) + " found in type field");
      }
      
      // Append valid character to type string
      typeField += chr;
    }

    string idField;

    // Only attempt to extract ID field if the length is > 0
    if (idLength > 0) {
        // Check if there are enough bytes to pull out id field
        assertHasValues(vals, idLength, "ID");

        // Checked that the bytes we require are available, now collect them
        auto idBytes = drainDeque(vals, typeLength);

        // Create string from UTF-8 bytes
        for (auto chr : idBytes) {
          idField += chr;
        }
    }

    // Collect remaining data as payload after validating length
    assertHasValues(vals, payloadLength, "payload");
    auto payload = drainDeque(vals, payloadLength);

    // Successfully built Record object from uint8_t array
    return Record { 
      .header = header,
      .typeLength = typeLength,
      .payloadLength = payloadLength,
      .idLength = idLength,
      .recordType = typeField,
      .idField = idField,
      .payload = payload
    };
  }

  /// Creates a byte from the Record object passed
  span<uint8_t> recordToBytes(const Record &record) {
    // Vector to create record byte array from
    vector<uint8_t> bytes;

    // Add Record Header byte
    bytes.push_back(RecordHeader::headerAsByte(record.header));
    
    // Add type length field
    bytes.push_back(record.typeLength);

    // Add payload length, dependant on the Short Record flag
    uint8_t payloadLen[4] = {
      static_cast<uint8_t>(record.payloadLength << 24),
      static_cast<uint8_t>(record.payloadLength << 16),
      static_cast<uint8_t>(record.payloadLength << 8),
      static_cast<uint8_t>(record.payloadLength << 0),
    };
    if (record.header.sr) {
      bytes.push_back(payloadLen[3]);
    } else {
      // Not a short record, append all 4 bytes in big endian order
      bytes.insert(bytes.end(), { payloadLen[0], payloadLen[1], payloadLen[2], payloadLen[3] });
    }

    // Add ID length if applicable
    if (record.header.il && record.idField.size() > 0) {
      bytes.insert(bytes.end(), record.idField.begin(), record.idField.end());
    }

    // Add type field
    assert(record.recordType.length() == record.typeLength);
    for (auto byte : record.recordType) {
        // Validate each character is valid ASCII
        if (byte <= 31 || byte >= 127) {
          throw NDEFException("Invalid type field character with code " + to_string(byte));
        }

        // Valid ASCII, add it to the output vector
        bytes.emplace_back(static_cast<uint8_t>(byte));
    }

    // Add ID field if present
    if (record.idLength > 0) {
      assert(record.idField.length() == record.idLength);
      bytes.insert(bytes.end(), record.idField.begin(), record.idField.end());
    }

    // Add payload bytes
    assert(record.payload.size() == record.payloadLength);
    bytes.insert(bytes.end(), record.payload.begin(), record.payload.end());

    // Return span pointing to location of vector in memory with number of bytes in vector
    return span<uint8_t> { &bytes[0], bytes.size() };
  }
}