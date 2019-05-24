#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

typedef struct RecordHeader RecordHeader_t;

extern RecordHeader_t*
ndef_recordFromBytes(const uint8_t *bytes, size_t len);

int main(void) {
  uint8_t chars[23] = {
    0xd1, // Record Header
          // - Message Begin (1b), Message End (1b), Last chunk (0b), Short Record (1b)
          // - ID Length not present (0b), NFC Forum Well Known Type TNF (0b001)
    0x01, // Type length - Payload type field is 1 octet long (single "T" char)
          // No ID Length
    0x0d, // Payload length 
          // - 13 octet (character) long payload
          // - SR flag set
    0x54, // Well Known Type - Text (ASCII "T")
    0x85, // Text encoded in UTF-8 (1b), RFU (always 0b), IANA language code "en-US" length = 5 (0b00101)
    // ISO/IANA language code "en-US" encoded in US-ASCII
    0x65, 0x6e, 0x7e, 0x55, 0x53,
    // UTF-8 encoded text payload
    0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64, 0x21
};
  size_t size = sizeof chars / sizeof *chars;
  RecordHeader_t* record = ndef_recordFromBytes(chars, size);
  
  if (record == NULL) {
    printf("NULL time!\n");
  }

  return 0;
}