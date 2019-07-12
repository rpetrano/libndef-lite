/*! Type Name Format Field
 * \file recordType.hpp
 *
 * This code provides helper functions and whatnot for the NFC Forum standard Type Name Format field
 *
 * \author RPiAwesomeness
 * \bug No known bugs
 */

#ifndef TYPE_NAME_FORMAT_H
#define TYPE_NAME_FORMAT_H

#include <deque>
#include <string>
#include <vector>

class NDEFRecordType {
public:
  /// Type Name Format Field types
  ///
  /// Represents 3-bit value describing record type, sets expectation for structure/content of record
  enum class TypeID {
    /// Record does not contain any information
    Empty = 0x00,

    /// Well Known Data Type - Data is defined by Record Type Definition (RTD) specification from NFC Forum
    WellKnown = 0x01,

    /// MIME Media - Data is one of the data types normally found in internet communications defined in RFC 2046
    MIMEMedia = 0x02,

    /// Absolute Uniform Resource Identifier - Pointer to a resource following the RFC 3986 syntax
    AbsoluteURI = 0x03,

    /// User-defined data that relies on the format specified by the RTD specification
    External = 0x04,

    /// Unknown data type - **Type length must be set to 0**
    Unknown = 0x05,

    /// Indicates this payload is a part of a chunked record, with the type unchanged from the first chunk
    Unchanged = 0x06,

    /// Invalid value, used for from_byte() helper. Value 0x07 is reserved by the NFC Forum
    Invalid = 0x07,
  };

  NDEFRecordType(TypeID id = TypeID::Empty, const std::string& name = "");

  constexpr inline bool operator==(const NDEFRecordType& rhs) const
  {
    return (this->type_id == rhs.type_id && this->type_name == rhs.type_name);
  }

  constexpr inline bool operator!=(const NDEFRecordType& rhs) const { return !(*this == rhs); }

  /// \param bytes vector of octets (bytes) of data to create RecordHeader object from
  /// \param offset offset within values vector to start from
  /// \return type value matching value, ::TypeID::Invalid if value does not match any TypeID Name Format field
  static NDEFRecordType from_bytes(std::vector<uint8_t> bytes, size_t offset = 0);

  /// \param bytes array of octets (bytes) of data to create RecordHeader object from
  /// \param len number of values in data array
  /// \param offset offset within values vector to start from
  /// \return type value matching value, #TypeID::Invalid if value does not match any TypeID Name Format field
  static NDEFRecordType from_bytes(uint8_t bytes[], size_t len, size_t offset = 0);

  // Accessors/mutators

  /// Gets record type variant
  /// \return TypeID variant of tnf member
  constexpr inline TypeID id() const { return this->type_id; }

  /// Gets record type name
  /// \return string record type
  inline std::string name() const { return this->type_name; }

  // Static type generators
  static NDEFRecordType text_record_type();
  static NDEFRecordType uri_record_type();
  static NDEFRecordType invalid_record_type();

private:
  /// ASCII Character/string representing record type
  std::string type_name;

  /// Type Name Format field value
  TypeID type_id;
};

#endif // TYPE_NAME_FORMAT_H
