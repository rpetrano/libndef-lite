/*! Type Name Format Field
 * \file typeNameFormat.hpp
 *
 * 
 * This code provides helper functions and whatnot for the NFC Forum standard Type Name Format field
 * 
 * \author Nathanael Olander (RPiAwesomeness)
 * \bug No known bugs
 */

#ifndef TYPE_NAME_FORMAT_HPP
#define TYPE_NAME_FORMAT_HPP

#include <string>

namespace TypeNameFormat {
    /// Type Name Format Field types 
    /// 
    /// Represents 3-bit value describing record type, sets expectation for structure/content of record
    enum Type {
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

        // Not actually implemented, but the value 0x07 is reserved by the NFC Forum
        // Reserved = 0x07,
    };

    /// \param value octet (byte) of data to create RecordHeader object from
    /// \return ::Type value
    Type fromByte(uint8_t value);

    /// \param header Reference to ::Type object
    /// \return byte representation of ::Type (only bottom 3 bits will be filled)
    uint8_t asByte(Type &field);
}

#endif // TYPE_NAME_FORMAT_HPP