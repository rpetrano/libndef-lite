#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte) \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

typedef struct RecordSpan {
  uint8_t *buffer;
  size_t size;
} RecordSpan_t;

typedef struct NDEFRecord NDEFRecord_t;

extern NDEFRecord_t*
ndef_recordFromBytes(const uint8_t *bytes, size_t len);

extern RecordSpan_t*
ndef_recordToBytes(NDEFRecord_t *span);

extern void
ndef_freeSpan(RecordSpan_t *span);

extern void
ndef_freeRecord(NDEFRecord_t *record);

NDEFRecord_t *testFromRawBytes();
void testToRawBytes(NDEFRecord_t *record);

// Known valid NDEF text record bytes
const uint8_t valid_sr_text[23] = {
  0xd1, // Record Header
        // - Message Begin (1b), Message End (1b), Last chunk (0b), Short Record (1b)
        // - ID Length not present (0b), NFC Forum Well Known Type TNF (0b001)
  0x01, // Type length - Payload type field is 1 octet long (single "T" char)
        // No ID Length
  0x13, // Payload length
        // - 19 octet (character) long payload
        // - SR flag set
  0x54, // Well Known Type - Text (ASCII "T")
  0x85, // Text encoded in UTF-8 (1b), RFU (always 0b), IANA language code "en-US" length = 5 (0b00101)
  // ISO/IANA language code "en-US" encoded in US-ASCII
  0x65, 0x6e, 0x7e, 0x55, 0x53,
  // UTF-8 encoded text payload
  0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64, 0x21
};

NDEFRecord_t *testFromRawBytes(RecordSpan_t *bytes) {
  NDEFRecord_t* record = ndef_recordFromBytes(bytes->buffer, bytes->size);

  assert(record != NULL);

  return record;
}

void testToRawBytes(NDEFRecord_t *record) {
  RecordSpan_t *span = ndef_recordToBytes(record);
  
  // Validate span is handled correctly
  assert(span->size == 23);

  // Validate contents of byte array
  for (size_t i = 0; i < span->size; i++) {
    printf("Buffer: " BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(span->buffer[i]));
    printf("Bytes: " BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(valid_sr_text[i]));
    assert(span->buffer[i] == valid_sr_text[i]);
  }

  // ndef_freeSpan(span);

  return;
}

int main(void) {
  RecordSpan_t span = { valid_sr_text, 23 };

  // Create new record
  NDEFRecord_t *record = testFromRawBytes(&span);

  // Attempt to get bytes from record
  testToRawBytes(record);

  // Only call if ndef_recordToBytes
  ndef_freeRecord(record);

  return 0;
}
