#pragma pack(push,1)

struct BitMapFileHeader
{
	short int type;
	int size;
	short int reserved;
	short int reserved2;
	int offsetbits;
};

struct BitMapInfoHeader
{
	int size;
	int width;
	int height;
	short int planes;
	short int bitcount;
	int compression;
	int sizeimage;
	int xpelspermeter;
	int ypelspermeter;
	int clrused;
	int clrimportant;
};

struct Pixel
{
	int X;
	int Y;
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char reserved;
};

#pragma (pop)

