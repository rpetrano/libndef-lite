#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>
#include <vector>

#include "ndef/record-type.hpp"
#include "ndef/record.hpp"

using NDEFRecordList = std::vector<NDEFRecord>;

class NDEFMessage {
public:
  NDEFMessage() {}
  NDEFMessage(const std::vector<uint8_t>& data, const NDEFRecordType& type = NDEFRecordType{}, uint offset = 0);
  NDEFMessage(const NDEFRecord& record);
  NDEFMessage(const NDEFRecordList& records);
  ~NDEFMessage() = default;

  void append_record(const NDEFRecord& record);
  void insert_record(const NDEFRecord& record, uint index = 0);
  void remove_record(uint index = 0);
  void set_record(const NDEFRecord& record, uint index = 0);

  NDEFRecord record(uint index = 0) const { return this->message_records.at(index); }
  NDEFRecordList records() const { return this->message_records; }

  size_t record_count() const { return this->message_records.size(); }
  bool is_valid() const;

  std::vector<uint8_t> as_bytes() const;

  static NDEFMessage from_bytes(const std::vector<uint8_t>& data, uint offset = 0);

private:
  NDEFRecordList message_records;
};

#endif // MESSAGE_HPP