#include <iostream>
#include <fstream>
#include "tgaimage.h"
#include <string>
#include <vector>
#include <sstream>

using namespace std;

const TGAColor whiteTGA = TGAColor(255, 255, 255, 255);
const TGAColor redTGA   = TGAColor(255, 0,   0,   255);


// Loads a PPM file, returning the data as a std::vector of 16-bit unsigned integers.
// PPM must be in "Plain" ASCII format, see https://netpbm.sourceforge.net/doc/ppm.html
// In GIMP, use the ASCII option when saving.
// The values are in blue, green, red order, in rows going down the image from left to right.
// Image dimensions and the maximum intensity value are passed in as pointers and set by
// this function.
std::vector<uint16_t> loadPPM(const std::string& filename, int* width, int* height, int* maxVal)
{
	ifstream input_image(filename);

	// Check the magic number at the start of the PPM is correct (should be P3).
	string magic;
	getline(input_image, magic);
	if (magic != "P3") {
		throw runtime_error("Supplied image is not a valid Plain format PPM file.");
	}

	// Read any comment lines in the PPM.
	string commentLine;
	while (true) {
		// Save our place before reading the next line.
		int startPos = input_image.tellg();
		getline(input_image, commentLine);
		cout << commentLine << std::endl;
		if (commentLine[0] != '#') {
			// This line wasn't a comment. Seek back to the start of the line.
			// Subtract extra two characters to handle \n.
			input_image.seekg(startPos-2, ios::beg);
			break;
		}
	}

	// Read in the width, height and maximum value of the PPM.
	input_image >> *width >> *height >> *maxVal;

	// Allocate data to store the PPM.
	std::vector<uint16_t> data(*width * *height * 3);

	// Load data from the file.
	for (int i = 0; i < *width * *height * 3; ++i) {
		input_image >> data[i];
	}

	return data;
}

// Saves a PPM in "Plain" (ASCII) format https://netpbm.sourceforge.net/doc/ppm.html
// Note: Data must be of size width*height*3, containing BGR pixel values.
//       User should ensure that all data values fall in the [0, maxVal] range.
void savePPM(const std::string& filename, const std::vector<uint16_t>& data, int width, int height, uint16_t maxVal = 65535)
{
	if (data.size() != width * height * 3) {
		throw std::runtime_error("Invalid data size in savePPM. Must be width*height*3 for RGB.");
	}

	ofstream outFile(filename, ios_base::out);

	// PPM Plain magic number.
	outFile << "P3\n";

	// Image dimensions.
	outFile << width << " " << height << " " << maxVal << "\n";

	// Saving raster data.
	for (int i = 0; i < width * height; ++i) {
		outFile
			<< data[i * 3 + 0] << " "
			<< data[i * 3 + 1] << " "
			<< data[i * 3 + 2] << "\n";
	}

	outFile.close();
}

// Clips a floating-point value, forcing it to be between min and max (inclusive).
float clipf(float val, float min, float max) {
	
	if (val < min) val = min;
	if (val > max) val = max;
	return val;
}

int main(int argc, char** argv) {
	int width, height, maxVal;
	auto imageData = loadPPM("../memorial.ppm", &width, &height, &maxVal);
	std::cout << "Loaded a PPM Image!" << std::endl;
	std::cout << "Width " << width << " Height " << height << " MaxVal " << maxVal << endl;

	std::vector<float> exposureValues{ 1.0f, 0.5f, 0.25f, 0.125f };

	for (float& exposure : exposureValues) {
		TGAImage image(width, height, TGAImage::RGB);
		for (int r = 0; r < height; ++r) 
			for (int c = 0; c < width; ++c) {

				// Replace the following with your own code!

				if (r > height / 2) {
					image.set(c, r, whiteTGA);
				}
				else {
					image.set(c, r, redTGA);
				}
			}
		
		stringstream outFilename;
		outFilename << "output" << exposure << ".tga";
		image.write_tga_file(outFilename.str().c_str());
	}

	return 0;
}

