#include "ndef/record.hpp"

// All valid URI identifiers
const uint num_identifiers = 36;
const std::string uri_identifiers[num_identifiers]{
  "", // 0x00 - No Protocol prepended
  "http://www.",
  "https://www.",
  "http://",
  "https://",
  "tel:",
  "mailto:",
  "ftp://anonymous:anonymous@",
  "ftp://ftp.",
  "ftps://",
  "sftp://",
  "smb://",
  "nfs://",
  "ftp://",
  "dav://",
  "news:",
  "telnet://",
  "imap:",
  "rtsp://",
  "urn:",
  "pop:",
  "sip:",
  "sips:",
  "tftp:",
  "btspp://",
  "btl2cap://",
  "btgoep://",
  "tcpobex://",
  "irdaobex://",
  "file://",
  "urn:epc:id:",
  "urn:epc:tag:",
  "urn:epc:pat:",
  "urn:epc:raw:",
  "urn:epc:",
  "urn:nfc:",
  // 0x24..0xFF - RFU - must be treated as value 0 if encountered
};

/// Creates URI record from URI provided
NDEFRecord NDEFRecord::create_uri_record(const std::string& uri)
{
  NDEFRecord record;
  record.set_type(NDEFRecordType::uri_record_type());

  // Bytes that will be stored in record
  std::vector<uint8_t> payload;

  // Attempt to match all URIs
  for (size_t i = 0; i < num_identifiers; i++) {
    auto prefix = uri_identifiers[i];

    // Skip any identifiers that are longer than the comparison URI
    if (uri.size() < prefix.size()) {
      continue;
    }

    // Check if the current identifier matches at the beginning of the string
    if (uri.rfind(prefix, 0) == 0) {
      // Add URI identifier code (position in list) and then add URI substring
      payload.emplace_back(i);
      payload.insert(payload.begin(), uri.begin() + prefix.size(), uri.end());
      break;
    }
  }

  record.set_payload(payload);

  return record;
}

/// Gets string form of URI protocol from URI Record payload
std::string NDEFRecord::get_uri_protocol(const std::vector<uint8_t>& payload)
{
  // First byte in the payload represents the URI identifier, allowing us to simply return a string
  return uri_identifiers[payload.at(0)];
}

/// Gets string form of actual URI from URI Record payload
std::string NDEFRecord::get_uri(const std::vector<uint8_t>& payload)
{
  // Bytes are encoded in ASCII/UTF-8, just create a string from them
  return std::string{ payload.begin() + 1, payload.end() };
}