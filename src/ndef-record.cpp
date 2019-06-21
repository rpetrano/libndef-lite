/*! A pure C++ implementation of the NDEF standard
 * \file ndefRecord.cpp
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
 * - Message Begin - 1 bit (bit 7)
 *     - Required
 *     - Indicates if this is the start of an NDEF message
 * - Message End - 1 bit (bit 6)
 *     - Required
 *     - Indicates if this is the last record in the message
 *     - If both mb and me are set in a header then the record is the only record in the message
 * - Chunk Flag - 1 bit (bit 5)
 *     - Required
 *     - Indicates if this the first record chunk or in the middle
 * - Short Record Flag - 1 bit (bit 4)
 *     - Required
 *     - Indicates if PAYLOAD LENGTH field is short (1 byte, 0-255) or longer
 * - ID Length Flag - 1 bit (bit 3)
 *     - Required
 *     - Indicates whether ID Length Field is present
 * -  Type Name Format - 3 bits (bits 2-0)
 *     - Required
 *     - Can be 0
 * - Type Length - 1 byte
 *     - Will always be 0 for certain values of TNF field
 *     - Specifies length, in octets, of the ID field
 * - Payload length - 1 byte or 4 bytes
 *     - Required
 *     - Can be 0
 *     - If `SR` flag is set in the record header then this value will be 1 byte, otherwise it will be a 4 byte value
 * - ID Length - 1 byte
 *     - Can be 0 (will result in omission of ID field
 *     - If `IL` flag is set in the record header then this value will be 1 byte in length, otherwise it will be
 *         omitted
 * - Type - length in bytes defined by Type Length field
 *     - Identifier that describes the contents of the payload
 *     - Formed from characters in US ASCII set, characters in range [0-31] and 127 are invalid and SHALL NOT be used
 * - ID - length in bytes defined by ID Length field
 *     - Relative or absolute URI identifier
 *     - Omitted if ID length is 0
 * - Payload - length in bytes defined by Payload Length field
 *     - Data in this field is opaque to the library and will be merely passed along
 *
 * \bug No known bugs
 */

#include <cassert>
#include <codecvt>
#include <deque>
#include <iostream>
#include <locale>
#include <string>

#include "ndef/encoding.hpp"
#include "ndef/exceptions.hpp"
#include "ndef/record-header.hpp"
#include "ndef/record.hpp"
#include "ndef/util.hpp"

#define BOM_BE_1ST static_cast<char>('\xef')
#define BOM_LE_2ND static_cast<char>('\xff')

using namespace std;
using namespace util;

/// Default constructor creates empty NDEF record
NDEFRecord::NDEFRecord()
{
  this->record_type = NDEFRecordType{};
  this->id_field = "";
  this->payload_data = vector<uint8_t>{};
}

NDEFRecord::NDEFRecord(const vector<uint8_t>& payload, const NDEFRecordType& type, const string& id, size_t offset,
                       bool chunked)
    : record_type(type), id_field(id), chunked(chunked)
{
  // Set payload from payload bytes, skipping bytes as specified by offset
  this->set_payload(vector<uint8_t>{ payload.begin() + offset, payload.end() });
}

NDEFRecord::NDEFRecord(const vector<uint8_t>& payload, const NDEFRecordType& type, size_t offset, bool chunked)
    : record_type(type), chunked(chunked)
{
  // Set payload from payload bytes, skipping bytes as specified by offset
  this->set_payload(vector<uint8_t>{ payload.begin() + offset, payload.end() });
}

/// Creates header byte from information known to NDEF Record. Other values will be set by NDEFMessage
uint8_t NDEFRecord::header() const
{
  uint8_t flags = 0x00;

  // Set Type Name Format field
  flags |= static_cast<uint8_t>(this->record_type.id());

  // If record < 256 bytes then this is a short record
  flags |= this->is_short() ? static_cast<uint8_t>(RecordFlag::SR) : 0;

  // If ID field has any value, set ID_LENGTH field
  flags |= (this->id_field.size() > 0) ? static_cast<uint8_t>(RecordFlag::IL) : 0;

  // Check if record is chunked
  flags |= this->is_chunked() ? static_cast<uint8_t>(RecordFlag::CF) : 0;

  return flags;
}

/// Wrapper around from_byte(vector<uint8_t>) that converts the array to a vector
NDEFRecord NDEFRecord::from_bytes(uint8_t bytes[], size_t size, size_t offset)
{
  // Create vector from byte array
  vector<uint8_t> bytesVec(bytes, bytes + size);

  return from_bytes(bytesVec, offset);
}

/// Allows us to convert from the raw bytes from the NFC tag into a NDEFRecord struct
NDEFRecord NDEFRecord::from_bytes(vector<uint8_t> bytes, size_t offset, size_t& bytes_used)
{
  // Keep track of number of bytes used
  bytes_used = 0;

  if (bytes.size() < 4) {
    // There are at least 4 required octets (field)
    throw NDEFException("Invalid number of octets, must have at least 4");
  }

  // Generate type field from first byte
  auto record_type = NDEFRecordType::from_bytes(bytes, offset);

  // If the bytes were invalid then return early without further parsing (indicates lack of bytes)
  if (record_type.id() == NDEFRecordType::TypeID::Invalid) {
    // No payload and not chunked, invalid payload
    return NDEFRecord{ vector<uint8_t>{}, record_type };
  }

  // Create deque to allow pop_front
  deque<uint8_t> vals(make_move_iterator(bytes.begin()), make_move_iterator(bytes.end()));

  // Pop off front byte (header already created)
  auto header = NDEFRecordHeader::from_byte(pop_front(vals));
  uint8_t type_length = pop_front(vals);

  // Header/type length bytes
  bytes_used += 2;

  uint32_t payload_length;
  if (header.sr) {
    // Payload is a short record, payload is at most 255 bytes long and length is contained in 1 byte
    payload_length = pop_front(vals);

    // One byte used for short record length
    bytes_used += 1;
  } else {
    assertHasValues(vals, 4, "payload length");

    // Create uint32 from next four bytes
    uint8_t payloadLenBytes[4]{ pop_front(vals), pop_front(vals), pop_front(vals), pop_front(vals) };
    payload_length = uint32FromBEBytes(payloadLenBytes);

    // Four bytes used for record length
    bytes_used += 4;
  }

  uint8_t id_length = 0;
  if (header.il) {
    assertHasValue(vals, "ID length");
    id_length = pop_front(vals);

    // One byte used for ID field
    bytes_used += 1;
  }

  // Confirm there are enough bytes to complete type field then populate type characters from bytes
  assertHasValues(vals, type_length, "type length");
  vector<uint8_t> type_bytes = drain_deque(vals, type_length);

  // Create the type field from the bytes, converting them into ASCII characters after validating them
  string type_field;
  for (auto&& chr : type_bytes) {
    if (chr <= 31 || chr == 127) {
      // Invalid character, no ASCII characters [0-31] or 127
      throw NDEFException("Invalid character code " + to_string(chr) + " found in type field");
    }

    // Append valid character to type string
    type_field += chr;
  }

  // type_field length # of bytes used to create type field string
  bytes_used += type_field.length();

  string id_field;

  // Only attempt to extract ID field if the length is > 0
  if (id_length > 0) {
    // Check if there are enough bytes to pull out id field
    assertHasValues(vals, id_length, "ID");

    // Checked that the bytes we require are available, now collect them
    auto id_bytes = drain_deque(vals, id_length);

    // Create string from UTF-8 bytes
    for (auto&& chr : id_bytes) {
      id_field += chr;
    }
  }

  // id_field length # of bytes used to create ID field string
  bytes_used += id_field.length();

  // Collect remaining data as payload after validating length
  assertHasValues(vals, payload_length, "payload");
  auto payload = drain_deque(vals, payload_length);

  // payload # of bytes used as the payload
  bytes_used += payload.size();

  // Successfully built Record object from uint8_t array
  return NDEFRecord{ payload, record_type, id_field, header.cf };
}

/// Creates the bytes representation of the Record object passed
vector<uint8_t> NDEFRecord::as_bytes(uint8_t flags) const
{
  // Vector to create record byte array from
  vector<uint8_t> bytes;

  NDEFRecordHeader header{ .tnf = this->record_type.id(),
                           .il = (this->id().length() > 0),
                           .sr = this->is_short(),
                           .cf = this->is_chunked(),
                           // Assume this record is only record, set message end/message begin. NDEFMessage may change
                           .me = true,
                           .mb = true };

  // Add Record Header byte
  bytes.push_back(header.asByte() | flags);

  // Add type length field
  bytes.push_back(record_type.name().length());

  // Add payload length, dependant on the Short Record flag
  if (this->is_short()) {
    assert(payload_data.size() < 256);
    bytes.push_back(static_cast<uint8_t>(payload_data.size()));
  } else {
    uint8_t payloadLen[4] = {
      static_cast<uint8_t>(payload_data.size() >> 24),
      static_cast<uint8_t>(payload_data.size() >> 16),
      static_cast<uint8_t>(payload_data.size() >> 8),
      static_cast<uint8_t>(payload_data.size() >> 0),
    };
    // Not a short record, append all 4 bytes in big endian order
    bytes.insert(bytes.end(), { payloadLen[0], payloadLen[1], payloadLen[2], payloadLen[3] });
  }

  // Add ID length if applicable
  if (header.il && id_field.size() > 0) {
    bytes.push_back(id_field.size());
  }

  // Add type field
  for (auto&& byte : record_type.name()) {
    // Validate each character is valid ASCII
    if (byte <= 31 || byte >= 127) {
      throw NDEFException("Invalid type field character with code " + to_string(byte));
    }

    // Valid ASCII, add it to the output vector
    bytes.emplace_back(static_cast<uint8_t>(byte));
  }

  // Add ID field if present
  if (this->id().length() > 0) {
    bytes.insert(bytes.end(), id_field.begin(), id_field.end());
  }

  // Add payload bytes
  bytes.insert(bytes.end(), payload_data.begin(), payload_data.end());

  // Return span pointing to location of vector in memory with number of bytes in vector
  return bytes;
}

/// Update the payload stored in this NDEFRecord object, validating the record after doing so
void NDEFRecord::set_payload(const vector<uint8_t>& data)
{
  payload_data = data;

  // Validate the record type is still valid
  this->validate();
}

/// Validates that if the payload has changed size then the type is no longer empty
void NDEFRecord::validate()
{
  // Not sure how this would happen, but reflecting their
  if (payload_data.size() > 0 && (record_type.id() == NDEFRecordType::TypeID::Empty)) {
    record_type = NDEFRecordType(NDEFRecordType::TypeID::Unknown);
  }
}
