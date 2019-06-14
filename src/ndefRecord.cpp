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
 * - \ref RecordHeader::RecordHeader::mb "Message Begin" - 1 bit (bit 7)
 *     - Required
 *     - Indicates if this is the start of an NDEF message
 * - \ref RecordHeader::RecordHeader::me "Message End" - 1 bit (bit 6)
 *     - Required
 *     - Indicates if this is the last record in the message
 *     - If both \t mb and \t me are set in a header then the record is the only record in the message
 * - \ref RecordHeader::RecordHeader::cf "Chunk Flag" - 1 bit (bit 5)
 *     - Required
 *     - Indicates if this the first record chunk or in the middle
 * - \ref RecordHeader::RecordHeader::sr "Short Record Flag" - 1 bit (bit 4)
 *     - Required
 *     - Indicates if PAYLOAD LENGTH field is short (1 byte, 0-255) or longer
 * - \ref RecordHeader::RecordHeader::il "ID Length Flag" - 1 bit (bit 3)
 *     - Required
 *     - Indicates whether ID Length Field is present
 * - \ref TypeNameFormat::Type "Type Name Format" - 3 bits (bits 2-0)
 *     - Required
 *     - Can be 0
 * - \ref ndef::Record::typeLength "Type Length" - 1 byte
 *     - Will always be 0 for certain values of TNF field
 *     - Specifies length, in octets, of the ID field
 * - \ref ndef::Record::payloadLength "Payload length" - 1 byte or 4 bytes
 *     - Required
 *     - Can be 0
 *     - If `SR` flag is set in the record header then this value will be 1 byte, otherwise it will be a 4 byte value
 * - \ref ndef::Record::idLength "ID Length" - 1 byte
 *     - Can be 0 (will result in omission of \ref ndef::Record::idField) "ID field"
 *     - If `IL` flag is set in the record header then this value will be 1 byte in length, otherwise it will be
 *         omitted
 * - \ref ndef::Record::recordType "Type" - length in bytes defined by Type Length field
 *     - Identifier that describes the contents of the payload
 *     - Formed from characters in US ASCII set, characters in range [0-31] and 127 are invalid and SHALL NOT be used
 * - \ref ndef::Record::idField "ID" - length in bytes defined by ID Length field
 *     - Relative or absolute URI identifier
 *     - Omitted if \ref ndef::Record::idLength "ID length" is 0
 * - \ref ndef::Record::payload "Payload" - length in bytes defined by Payload Length field
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

#include "exceptions.hpp"
#include "ndefRecord.hpp"
#include "recordHeader.hpp"
#include "util.hpp"

using namespace std;
using namespace util;

/// Default constructor creates empty NDEF record
NDEFRecord::NDEFRecord()
{
  this->recordType = NDEFRecordType{};
  this->idField = "";
  this->payloadData = vector<uint8_t>{};
}

NDEFRecord::NDEFRecord(const NDEFRecordType& type, const std::vector<uint8_t>& payload, bool chunked)
    : recordType(type), chunked(chunked)
{
  this->setPayload(payload);
}

/// Creates header byte from information known to NDEF Record. Other values will be set by NDEFMessage
uint8_t NDEFRecord::header() const
{
  uint8_t flags = 0x00;

  // Set Type Name Format field
  flags |= static_cast<uint8_t>(this->recordType.id());

  // If record < 256 bytes then this is a short record
  flags |= this->isShort() ? static_cast<uint8_t>(RecordFlag::SR) : 0;

  // If ID field has any value, set ID_LENGTH field
  flags |= (this->idField.size() > 0) ? static_cast<uint8_t>(RecordFlag::IL) : 0;

  // Check if record is chunked
  flags |= this->isChunked() ? static_cast<uint8_t>(RecordFlag::CF) : 0;

  return flags;
}

/// Wrapper around fromByte(vector<uint8_t>) that converts the array to a vector
NDEFRecord NDEFRecord::fromBytes(uint8_t bytes[], size_t size, size_t offset)
{
  // Create vector from byte array
  vector<uint8_t> bytesVec(bytes, bytes + size);

  return fromBytes(bytesVec, offset);
}

/// Allows us to convert from the raw bytes from the NFC tag into a NDEFRecord struct
NDEFRecord NDEFRecord::fromBytes(vector<uint8_t> bytes, size_t offset)
{
  if (bytes.size() < 4) {
    // There are at least 4 required octets (field)
    throw NDEFException("Invalid number of octets, must have at least 4");
  }

  // Generate type field from first byte
  auto recordType = NDEFRecordType::fromBytes(bytes, offset);

  // If the bytes were invalid then return early without further parsing (indicates lack of bytes)
  if (recordType.id() == NDEFRecordType::TypeID::Invalid) {
    // No payload and not chunked, invalid payload
    return NDEFRecord{ recordType };
  }

  // Create deque to allow pop_front
  deque<uint8_t> vals(make_move_iterator(bytes.begin()), make_move_iterator(bytes.end()));

  // Pop off front byte (header already created)
  auto header = NDEFRecordHeader::fromByte(popFront(vals));
  uint8_t typeLength = popFront(vals);

  uint32_t payloadLength;
  if (header.sr) {
    // Payload is a short record, payload is at most 255 bytes long and length is contained in 1 byte
    payloadLength = popFront(vals);
  } else {
    assertHasValues(vals, 4, "payload length");

    // Create uint32 from next four bytes
    uint8_t payloadLenBytes[4]{ popFront(vals), popFront(vals), popFront(vals), popFront(vals) };
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
  return NDEFRecord{ recordType, payload, header.cf };
}

/// Creates a byte from the Record object passed
vector<uint8_t> NDEFRecord::asBytes() const
{
  // Vector to create record byte array from
  vector<uint8_t> bytes;

  NDEFRecordHeader header{ .tnf = this->recordType.id(),
                           .il = (this->id().length() > 0),
                           .sr = this->isShort(),
                           .cf = this->isChunked(),
                           // Assume this record is only record, set message end/message begin. NDEFMessage may change
                           .me = true,
                           .mb = true };

  // Add Record Header byte
  bytes.push_back(header.asByte());

  // Add type length field
  bytes.push_back(recordType.name().length());

  // Add payload length, dependant on the Short Record flag
  if (this->isShort()) {
    assert(payloadData.size() < 256);
    bytes.push_back(static_cast<uint8_t>(payloadData.size()));
  } else {
    uint8_t payloadLen[4] = {
      static_cast<uint8_t>(payloadData.size() >> 24),
      static_cast<uint8_t>(payloadData.size() >> 16),
      static_cast<uint8_t>(payloadData.size() >> 8),
      static_cast<uint8_t>(payloadData.size() >> 0),
    };
    // Not a short record, append all 4 bytes in big endian order
    bytes.insert(bytes.end(), { payloadLen[0], payloadLen[1], payloadLen[2], payloadLen[3] });
  }

  // Add ID length if applicable
  if (header.il && idField.size() > 0) {
    bytes.insert(bytes.end(), idField.begin(), idField.end());
  }

  // Add type field
  for (auto byte : recordType.name()) {
    // Validate each character is valid ASCII
    if (byte <= 31 || byte >= 127) {
      throw NDEFException("Invalid type field character with code " + to_string(byte));
    }

    // Valid ASCII, add it to the output vector
    bytes.emplace_back(static_cast<uint8_t>(byte));
  }

  // Add ID field if present
  if (this->id().length() > 0) {
    bytes.insert(bytes.end(), idField.begin(), idField.end());
  }

  // Add payload bytes
  bytes.insert(bytes.end(), payloadData.begin(), payloadData.end());

  // Return span pointing to location of vector in memory with number of bytes in vector
  return bytes;
}

/// Update the payload stored in this NDEFRecord object, validating the record after doing so
void NDEFRecord::setPayload(const std::vector<uint8_t> data)
{
  payloadData = data;

  // Validate the record type is still valid
  this->validate();
}

/// Append bytes provided to payload stored in this NDEFRecord object, validating the record after doing so
void NDEFRecord::appendPayload(const std::vector<uint8_t> data)
{
  // Reserve required space in advance to improve extension performance
  payloadData.reserve(payloadData.size() + distance(data.begin(), data.end()));

  // Extend payload bytes with contents of data
  payloadData.insert(payloadData.end(), data.begin(), data.end());

  // Validate the record type is still valid
  this->validate();
}

/// Validates that if the payload has changed size then the type is no longer empty
void NDEFRecord::validate()
{
  if (payloadData.size() > 0 && (recordType.id() == NDEFRecordType::TypeID::Empty)) {
    recordType = NDEFRecordType(NDEFRecordType::TypeID::Unknown);
  }
}

NDEFRecord createTextRecord(const std::string& text, const std::string& locale, RecordTextCodec codec) {}
std::string textLocale(const std::vector<uint8_t>& payload)
{
  // Get the length of the locale string from the payload
  const uint localeLength = payload.at(0) & 0x1f;
  return string{ payload.begin() + 1, payload.begin() + 1 + localeLength };
}
std::string textFromTextPayload(const std::vector<uint8_t>& payload)
{
  const uint detailByte = payload.at(0);
  const uint localeLength = detailByte & 0x1f;
  const std::string recordText{ payload.begin() + 1 + localeLength, payload.end() };

  if (detailByte & static_cast<uint8_t>(RecordTextCodec::UTF16) {
    
  }
}
