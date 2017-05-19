/*
 * read.cpp
 *
 *  Created on: May 19, 2017
 *      Author: Diego Coelho, PhD Candidate, UofC
 */

#include "exiv2/exiv2.hpp"

#include <iostream>
#include <iomanip>
#include <cassert>
#include <typeinfo>
#include <utility>

int main(int argc, char* const argv[])
try {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " file\n";
        return 1;
    }
    Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(argv[1]);
    assert(image.get() != 0);
    image->readMetadata();
    Exiv2::ExifData &exifData = image->exifData();
    if (exifData.empty()) {
        std::string error(argv[1]);
        error += ": No Exif data found in the file";
        throw Exiv2::Error(1, error);
    }

    const Exiv2::ExifKey focalkey("Exif.Photo.FocalLength");
    Exiv2::ExifData::const_iterator iter = exifData.findKey(focalkey);

    std::cout << "The focal length is " << iter->value() << std::endl;
    std::cout << "The focal length is (double format) " << iter->value().toFloat() << std::endl;

    return 0;
}

catch (Exiv2::Error& e) {
    std::cout << "Caught Exiv2 exception '" << e.what() << "'\n";
    return -1;
}
