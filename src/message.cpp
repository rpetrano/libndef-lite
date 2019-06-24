#include "ndef-lite/message.hpp"
#include "ndef-lite/record-header.hpp"
#include "ndef-lite/record-type.hpp"

using namespace std;

/// Creates NDEF Message object with single initial record
NDEFMessage::NDEFMessage(const vector<uint8_t>& data, const NDEFRecordType& type, uint offset)
{
  this->message_records.push_back(NDEFRecord{ data, type, offset });
}

/// Creates NDEF Message object from existing NDEF Record
NDEFMessage::NDEFMessage(const NDEFRecord& record) { this->message_records.push_back(record); }

/// Creates NDEF Message object from multiple existing NDEF Records
NDEFMessage::NDEFMessage(const NDEFRecordList& records) { this->message_records = records; }

/// Append an existing NDEF Record object to the message
void NDEFMessage::append_record(const NDEFRecord& record) { this->message_records.push_back(record); }

/// Insert an existing NDEF Record object at specified index in the message
void NDEFMessage::insert_record(const NDEFRecord& record, uint index)
{
  // Have to provide bounds checking
  if (index > this->message_records.size()) {
    throw std::out_of_range{ "Unable to insert record. Index " + to_string(index) + " outside of range of message" };
  }

  this->message_records.emplace(this->message_records.begin() + index, record);
}

/// Remove NDEF Record object from message at specified index
void NDEFMessage::remove_record(uint index)
{
  // Have to provide bounds checking
  if (index >= this->message_records.size()) {
    throw std::out_of_range{ "Unable to remove record. Index " + to_string(index) + " outside of range of message" };
  }
  this->message_records.erase(this->message_records.begin() + index);
}

/// Replace record in message at specified index
void NDEFMessage::set_record(const NDEFRecord& record, uint index)
{
  // Have to provide bounds checking
  if (index >= this->message_records.size()) {
    throw std::out_of_range{ "Unable to set record. Index " + to_string(index) + " outside of range of message" };
  }
  this->message_records.at(index) = record;
}

/// Validates whether this NDEF Message object can be marshalled into a valid NDEF Message byte sequence
bool NDEFMessage::is_valid() const
{
  // Messages must have at least 1 record
  if (this->message_records.empty()) {
    return false;
  }

  // Validate all individual records
  for (auto&& record : this->message_records) {
    if (!record.is_valid()) {
      return false;
    }
  }

  return true;
}

std::vector<uint8_t> NDEFMessage::as_bytes() const
{
  vector<uint8_t> byte_sequence;

  // Can only generate a byte sequence if the message is valid
  if (!this->is_valid()) {
    return byte_sequence;
  }

  // Generate header for each record
  uint8_t header = 0x00;
  size_t num_records = this->message_records.size();
  for (size_t i = 0; i < num_records; i++) {
    // Immutable reference to current record to allow us to retrieve data without changing anything
    auto&& record = this->message_records.at(i);

    // Retrieve individual record's header, then set ME/MB bit flags appropriately
    header = record.header();
    // First record, set Message Begin flag
    header |= (i == 0) ? static_cast<uint8_t>(RecordFlag::MB) : 0;
    // Last record, set Message End flag
    header |= (i == (num_records - 1)) ? static_cast<uint8_t>(RecordFlag::ME) : 0;

    // Create byte sequence, setting MB/ME header flags for record, then add the bytes to the ouput bytes
    auto record_bytes = record.as_bytes(header);
    byte_sequence.insert(byte_sequence.end(), record_bytes.begin(), record_bytes.end());
  }

  return byte_sequence;
}

NDEFMessage NDEFMessage::from_bytes(const std::vector<uint8_t>& data, uint offset)
{
  NDEFMessage msg;

  // NOTE: Potentially should be using a deque due to removal from front of vector
  // Get all bytes after offset position from input
  auto buffer = vector<uint8_t>{ data.begin() + offset, data.end() };

  // Read in all bytes into records
  while (!buffer.empty()) {
    // Number of bytes used by record during creation
    size_t bytes_used = 0;

    // Create record from current byte array (offset already handled)
    auto record = NDEFRecord::from_bytes(buffer, 0, bytes_used);

    // If the record was invalid, then quit now, ignoring all current/remaining bytes
    if (record.type().id() == NDEFRecordType::TypeID::Invalid) {
      break;
    }

    // Record is valid, add it to the message
    msg.append_record(record);

    // Remove bytes read by record creation
    buffer.erase(buffer.begin(), buffer.begin() + bytes_used);
  }

  return msg;
}