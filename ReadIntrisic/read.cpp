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

    Exiv2::ExifData::const_iterator end = exifData.end();
    for (Exiv2::ExifData::const_iterator i = exifData.begin(); i != end; ++i) {
        const char* tn = i->typeName();
        std::cout << std::setw(44) << std::setfill(' ') << std::left
                  << i->key() << " "
                  << "0x" << std::setw(4) << std::setfill('0') << std::right
                  << std::hex << i->tag() << " "
                  << std::setw(9) << std::setfill(' ') << std::left
                  << (tn ? tn : "Unknown") << " "
                  << std::dec << std::setw(3)
                  << std::setfill(' ') << std::right
                  << i->count() << "  "
                  << std::dec << i->value()
                  << "\n";
    }




    const Exiv2::ExifKey focalkey("Exif.Photo.FocalLength");
    Exiv2::ExifData::const_iterator iter = exifData.findKey(focalkey);

    std::cout << "The focal length is " << iter->value() << std::endl;
    std::cout << "The focal length is (double format) " << iter->value().toFloat() << std::endl;

    const Exiv2::ExifKey funitres("Exif.Image.FocalPlaneResolutionUnit");
    iter = exifData.findKey(funitres);
    if(iter == exifData.end()){
    	std::cout << "The key Exif.Image.FocalPlaneResolutionUnit was not found." << std::endl;
    } else {
    	std::cout << "The unit is " << iter->value() << std::endl;
    }

    const Exiv2::ExifKey fxres("Exif.Image.FocalPlaneXResolution");
    iter = exifData.findKey(fxres);
    if(iter == exifData.end()){
    	std::cout << "The key Exif.Image.FocalPlaneXResolution was not found." << std::endl;
    } else {
    	std::cout << "The X resolution is " << iter->value() << std::endl;
    }

    const Exiv2::ExifKey pfunitres("Exif.Photo.FocalPlaneResolutionUnit");
	iter = exifData.findKey(pfunitres);
	if(iter == exifData.end()){
		std::cout << "The key Exif.Photo.FocalPlaneResolutionUnit was not found." << std::endl;
	} else {
		std::cout << "The unit is " << iter->value() << std::endl;
	}

	const Exiv2::ExifKey pfxres("Exif.Photo.FocalPlaneXResolution");
	iter = exifData.findKey(pfxres);
	if(iter == exifData.end()){
		std::cout << "The key Exif.Photo.FocalPlaneXResolution was not found." << std::endl;
	} else {
		std::cout << "The X resolution is " << iter->value() << std::endl;
	}




    return 0;
}

catch (Exiv2::Error& e) {
    std::cout << "Caught Exiv2 exception '" << e.what() << "'\n";
    return -1;
}
