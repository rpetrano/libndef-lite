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

This project is built using CMake, so installation is nice and easy. Run the following commands and you'll be all set!

```bash
mkdir build
cd build
cmake ..
make
sudo make install
```

## Usage

Once the library is installed you will import the functionality via `<ndef-lite/[component].hpp>` and compile with the `-lndef-lite` flag!

### Create NDEF Text Record and then serialize to byte array

```c++
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include <ndef-lite/message.hpp>
#include <ndef-lite/record.hpp>

int main() {
    // Create the text record for the message
    std::string message{"I'm sorry Dave, I'm afraid I can't do that."};

    // Create NDEF Message object
    NDEFMessage msg;

    // Record defaults to UTF-8 encoding, but UTF-16 is supported
    msg.append_record(NDEFRecord::create_text_record(message, "en-US"));

    // Serialize record
    std::vector<uint8_t> bytes = msg.as_bytes();

    // Configure cout to show hex characters
    std::cout << std::showbase << std::internal << std::setfill('0') << std::hex
                << std::setw(4);

    // Output raw bytes
    std::cout << "NDEF Record in raw bytes form:\n";
    for (uint &&val : bytes) {
        std::cout << val;
        std::cout << " ";
    }
    std::cout << std::endl;

    std::cout << "Payload message [" << msg.record(0).get_text_locale() << "]:\n";
    std::cout << NDEFRecord::get_text(msg.record(0).payload()) << std::endl;
    return 0;
}
```

## Coverage and Tests

This library is currently at 95.2% test coverage according to [LCOV](http://ltp.sourceforge.net/coverage/lcov.php) as of 2019-06-25 15:30.

Testing is done via the [doctest framework](https://github.com/onqtam/doctest) and can be executed locally by running `make test`.

Coverage be checked locally by running `make coverage`.

[![forthebadge](https://img.shields.io/badge/USES-BADGES-38c1d0.svg?style=for-the-badge&labelColor=45a4b8)](https://forthebadge.com)
