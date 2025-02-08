#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <windows.h>

using namespace std;

#pragma pack(1)
struct bmpFH {
	uint16_t bfType;
	uint32_t bfSize;
	uint16_t bfReversed1;
	uint16_t bfReversed2;
	uint32_t bfOffBits;
};

#pragma pack(1)
struct bmpIH {
	uint32_t biSize;
	uint32_t biWidth;
	uint32_t biHeight;
	uint16_t biPlanes;
	uint16_t biBitCount;
	uint32_t biCompression;
	uint32_t biSizeImage;
	uint32_t biXPelsPerMeter;
	uint32_t biYPelsPerMeter;
	uint32_t biClrUsed;
	uint32_t biClrImportant;
};

#pragma pack(1)
struct RGBQuad {
	uint32_t Red = 255;
	uint32_t Green = 255;
	uint32_t Blue = 255;
	uint32_t Alpha = 255;
};

int main() {
	string fname;
	cout << "Source image name: ";
	getline(cin, fname);
	fstream bmp(fname + ".bmp", ios::in | ios::binary);
	bmpFH fileHeader;
	bmpIH infoHeader;

	string negativename;
	cout << "Negative image name: ";
	getline(cin, negativename);
	fstream negbmp(negativename + ".bmp", ios::out | ios::binary);

	if (!bmp.is_open()) {
		cout << "Can't open file" << endl;
		return 0;
	}

	bmp.read((char*)&fileHeader, sizeof(fileHeader));
	bmp.read((char*)&infoHeader, sizeof(infoHeader));

	cout << "Width: " << infoHeader.biWidth << endl;
	cout << "Height: " << infoHeader.biHeight << endl;
	cout << "Bit count: " << infoHeader.biBitCount << endl;
	cout << "Color channels: " << infoHeader.biBitCount / 8 << endl;
	cout << "Bit offset: " << hex << fileHeader.bfOffBits << endl << dec;

	vector<vector<RGBQuad>> pixels(infoHeader.biHeight);
	for (int i = 0; i != infoHeader.biHeight; ++i) {
		vector<RGBQuad> row(infoHeader.biWidth);
		pixels[i] = row;
	}

	bmp.seekg(fileHeader.bfOffBits, ios::beg);

	for (int y = infoHeader.biHeight - 1; y != -1; --y) {
		for (int x = 0; x != infoHeader.biWidth; ++x) {
			switch (infoHeader.biBitCount / 8)
			{
			case 1:
				bmp.read((char*)&pixels[y][x], 1);
				pixels[y][x].Green = pixels[y][x].Red;
				pixels[y][x].Blue = pixels[y][x].Red;
				pixels[y][x].Alpha = pixels[y][x].Red;
				break;
			case 3:
				bmp.read((char*)&pixels[y][x].Blue, 1);
				bmp.read((char*)&pixels[y][x].Green, 1);
				bmp.read((char*)&pixels[y][x].Red, 1);
				break;
			case 4:
				bmp.read((char*)&pixels[y][x].Blue, 1);
				bmp.read((char*)&pixels[y][x].Green, 1);
				bmp.read((char*)&pixels[y][x].Red, 1);
				bmp.read((char*)&pixels[y][x].Alpha, 1);
				break;
			default:
				break;
			}
		}
		bmp.seekg(ceil(infoHeader.biWidth / 4.0f) * 4 - infoHeader.biWidth, ios::cur);
	}

	bmp.seekg(ios::beg);
	negbmp << bmp.rdbuf();
	bmp.close();

	negbmp.seekp(fileHeader.bfOffBits, ios::beg);

	for (int y = infoHeader.biHeight - 1; y != -1; --y) {
		for (int x = 0; x != infoHeader.biWidth; ++x) {
			pixels[y][x] = {
				255 - pixels[y][x].Red,
				255 - pixels[y][x].Green,
				255 - pixels[y][x].Blue,
				255 - pixels[y][x].Alpha,
			};
			switch (infoHeader.biBitCount / 8)
			{
			case 1:
				negbmp.write((char*)&pixels[y][x].Alpha, 1);
				break;
			case 3:
				negbmp.write((char*)&pixels[y][x].Blue, 1);
				negbmp.write((char*)&pixels[y][x].Green, 1);
				negbmp.write((char*)&pixels[y][x].Red, 1);
				break;
			case 4:
				negbmp.write((char*)&pixels[y][x].Blue, 1);
				negbmp.write((char*)&pixels[y][x].Green, 1);
				negbmp.write((char*)&pixels[y][x].Red, 1);
				negbmp.write((char*)&pixels[y][x].Alpha, 1);
				break;
			default:
				break;
			}
		}
		negbmp.seekp(ceil(infoHeader.biWidth / 4.0f) * 4 - infoHeader.biWidth, ios::cur);
	}
	negbmp.close();
}