#include "ndefRecord.hpp"

/// Creates URI record from URI provided
NDEFRecord NDEFRecord::create_uri_record(const std::string& uri) {}

/// Gets string form of URI protocol from URI Record payload
std::string NDEFRecord::get_uri_protocol(const std::vector<uint8_t>& payload) {}

/// Gets string form of actual URI from URI Record payload
std::string NDEFRecord::get_uri(const std::vector<uint8_t>& payload) {}