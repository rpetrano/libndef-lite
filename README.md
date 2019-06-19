# ndef-lite

NDEF (NFC Data Exchange Format) Message library written in pure C++

[![forthebadge](https://img.shields.io/badge/MADE%20WITH-C++-ef4041.svg?style=for-the-badge&labelColor=c1282d)](https://forthebadge.com)
[![Gitter](https://img.shields.io/gitter/room/RPiAwesomeness/libndef.svg?logo=gitter&style=for-the-badge)](https://gitter.im/libndef/community)
[![Documentation Status](https://readthedocs.org/projects/libndef/badge/?version=latest&style=for-the-badge)](http://libndef.readthedocs.io/)
[![MIT license](https://img.shields.io/badge/License-MIT-blue.svg?style=for-the-badge)](https://gitlab.com/RPiAwesomeness/libndef/blob/master/LICENSE)

## About

This project was written due to a lack of C++ libraries providing support for the NDEF message standard. The only one that I was able to find was the [NFC Tools Qt-based implementation](https://github.com/nfc-tools/libndef), which is an unnecessary dependency.

This implementation is in pure C++14, aiming to provide feature parity with that library and improve on it while avoiding the Qt dependency.

More information about the NFC/NDEF standards through [the NFC Forum's website](https://nfc-forum.org/our-work/specifications-and-application-documents/specifications/nfc-forum-technical-specifications/).

## Installation

...yeah no clue currently :fire:

## Usage

### Create NDEF Text Record and then serialize to byte array

```c++
#include <string>
#include <vector>

#include <ndef-lite/ndef_record.hpp>
#include <ndef-lite/ndef_message.hpp>

// Create the text record for the message 
std::string message{"I'm sorry Dave, I'm afraid I can't do that."};

// Create NDEF Message object
NDEFMessage msg;

// Record defaults to UTF-8 encoding, but UTF-16 is supported
msg.appendRecord(NDEFRecord::create_text_record(message, "en-US"));

// Serialize record
std::vector<uint8_t> bytes = msg.to_bytes();
```

## Coverage and Tests

This library is currently at 71.6% test coverage according to [LCOV](http://ltp.sourceforge.net/coverage/lcov.php) as of 2019-19-06 14:55:41.

Testing is done via the [Catch2 framework](https://github.com/catchorg/Catch2) and can be executed locally by running `make test`.

Coverage be checked locally by running `make coverage`.

[![forthebadge](https://img.shields.io/badge/USES-BADGES-38c1d0.svg?style=for-the-badge&labelColor=45a4b8)](https://forthebadge.com)