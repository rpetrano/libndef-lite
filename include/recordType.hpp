/*! Type Name Format Field
 * \file typeNameFormat.hpp
 *
 * This code provides helper functions and whatnot for the NFC Forum standard Type Name Format field
 *
 * \author Nathanael Olander (RPiAwesomeness)
 * \bug No known bugs
 */

#ifndef TYPE_NAME_FORMAT_H
#define TYPE_NAME_FORMAT_H

#include <string>

namespace NDEFRecordType {
/// Type Name Format Field types
///
/// Represents 3-bit value describing record type, sets expectation for structure/content of record
enum class Type {
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

  /// Invalid value, used for fromByte() helper. Value 0x07 is reserved by the NFC Forum
  Invalid = 0x07,
};

/// \param value octet (byte) of data to create RecordHeader object from
/// \return type value matching value, ::Type::Invalid if \t value does not match any Type Name Format field
constexpr inline Type fromByte(const uint8_t value)
{
  return (value <= 0x06) ? static_cast<Type>(value) : Type::Invalid;
}

/// \param header Reference to ::Type object
/// \return byte representation of ::Type (only bottom 3 bits will be filled)
constexpr inline uint8_t asByte(const Type& field) { return static_cast<uint8_t>(field); }
} // namespace NDEFRecordType

#endif // TYPE_NAME_FORMAT_H