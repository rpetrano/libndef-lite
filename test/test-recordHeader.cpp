#include "catch.hpp"
#include "recordHeader.hpp"

TEST_CASE( "Simple Record Header", "[simpleRecordHeader]" ) {
    uint8_t newByte = 0xff;
    auto expected = RecordHeader::RecordHeader {
        .mb = true,
        .me = true,
        .cf = true,
        .sr = true,
        .il = true,
        .tnf = TypeNameFormat::fromByte(0x07),
    };
    auto recordHeader = RecordHeader::fromByte(newByte);

    REQUIRE( expected == recordHeader );
}


TEST_CASE( "Short Record header", "[shortRecordHeader]" ) {
    uint8_t newByte = 0xd1;
    auto expected = RecordHeader::RecordHeader {
        .mb = true,
        .me = true,
        .cf = false,
        .sr = true,
        .il = false,
        .tnf = TypeNameFormat::Type::WellKnown,
    };
    auto recordHeader = RecordHeader::fromByte(newByte);

    REQUIRE( expected == recordHeader );
}

TEST_CASE( "Record Header to byte", "[headerToByte]") {
    auto header = RecordHeader::RecordHeader {
        .mb = true,
        .me = true,
        .cf = false,
        .sr = true,
        .il = false,
        .tnf = TypeNameFormat::Type::WellKnown,
    };
    uint8_t byte = RecordHeader::headerAsByte(header);
    REQUIRE( byte == 0b11010001 );
}
