#include "catch.hpp"
#include "recordHeader.hpp"

TEST_CASE( "Simple Record Header", "[simpleRecordHeader]" ) {
    uint8_t newByte = 0xff;
    auto expected = NDEFRecordHeader::RecordHeader {
        .mb = true,
        .me = true,
        .cf = true,
        .sr = true,
        .il = true,
        .tnf = NDEFRecordType::fromByte(0x07),
    };
    auto recordHeader = NDEFRecordHeader::fromByte(newByte);

    REQUIRE( expected == recordHeader );
}


TEST_CASE( "Short Record header", "[shortRecordHeader]" ) {
    uint8_t newByte = 0xd1;
    auto expected = NDEFRecordHeader::RecordHeader {
        .mb = true,
        .me = true,
        .cf = false,
        .sr = true,
        .il = false,
        .tnf = NDEFRecordType::Type::WellKnown,
    };
    auto recordHeader = NDEFRecordHeader::fromByte(newByte);

    REQUIRE( expected == recordHeader );
}

TEST_CASE( "Record Header to byte", "[headerAsByte]") {
    auto header = NDEFRecordHeader::RecordHeader {
        .mb = true,
        .me = true,
        .cf = false,
        .sr = true,
        .il = false,
        .tnf = NDEFRecordType::Type::WellKnown,
    };
    uint8_t byte = NDEFRecordHeader::asByte(header);
    
    REQUIRE( byte == 0b11010001 );
}
