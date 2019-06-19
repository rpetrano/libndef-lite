#include "ndef/message.hpp"
#include "ndef/record-type.hpp"

using namespace std;

/// Creates NDEF Message object with single initial record
NDEFMessage::NDEFMessage(const vector<uint8_t>& data, const NDEFRecordType& type, uint offset)
{
  this->message_records.push_back(NDEFRecord{ type, data, offset });
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