/* 1. Leitura e armazenamento do bitmap
* 2. Cálculo da média de cada componente
* 3. Cálculo da variância de cada componenete
* 4. Offset
* 5. Ganho
* 6. Aplicar 4 e 5 em cada pixel
*/

// A leitura das cores é ao contrário


#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "bitmap.h"
#include <vector>
using namespace std;

BitMapFileHeader img_ref_header;
BitMapFileHeader img_ajuste_header;
BitMapInfoHeader img_ref_info;
BitMapInfoHeader img_ajuste_info;
Pixel pix;

void readBitMap(FILE* img_ref, FILE* img_ajuste)
{
	fread(&img_ref_header, sizeof(BitMapFileHeader), 1, img_ref);
	fread(&img_ref_info, sizeof(BitMapInfoHeader), 1, img_ref);
	fread(&img_ajuste_header, sizeof(BitMapFileHeader), 1, img_ajuste);
	fread(&img_ajuste_info, sizeof(BitMapInfoHeader), 1, img_ajuste);

	cout << img_ref_header.type << " " << img_ref_header.size << " " << img_ref_header.reserved << img_ref_header.reserved2 << " " << img_ref_header.offsetbits << endl;


	cout << img_ref_info.width << " " << img_ref_info.height << endl;


	vector<Pixel> bitmap_ref;
	vector<Pixel> bitmap_ajuste;

	FILE* tes = fopen("tes.txt", "w");
	FILE* tese = fopen("tese.txt", "w");

	int column = img_ref_info.width - 1;

	if((img_ref_info.width%4) != 0) //caso não seja divisivel por 4, a gente faz virar
	{
		column = ceil(img_ref_info.width / 4.0) *4;
	}

	int row = img_ref_info.height - 1;
	int count = 0;
	while (fread(&pix.B, sizeof(char), 1, img_ref))
	{
		fread(&pix.G, sizeof(char), 1, img_ref);
		fread(&pix.R, sizeof(char), 1, img_ref);
		if(column < img_ref_info.width) //caso esteja nas colunas normais, grava direto
		{
			pix.X = column;
			pix.Y = row;
			//cout << (int)(pix.B) << " " << (int)(pix.G) << " " << (int)(pix.R) << endl;
			fprintf(tes, "%d %d %d %d %d \n", pix.B, pix.G, pix.R, pix.X, pix.Y);
		} else // recebe o img_ref_info.width - 1 aqui pra não perder a leitura do bmp
		{
			column = img_ref_info.width - 1;
			pix.X = column;
			pix.Y = row;
			//cout << (int)(pix.B) << " " << (int)(pix.G) << " " << (int)(pix.R) << endl;
			fprintf(tes, "%d %d %d %d %d \n", pix.B, pix.G, pix.R, pix.X, pix.Y);
		}
		column--;

		if (column == -1)
		{
			row--;
			column = img_ref_info.width - 1;
		}

		bitmap_ref.push_back(pix);
		
	}



	column = img_ajuste_info.width - 1;
	row = img_ajuste_info.height - 1;

	while (fread(&pix.B, sizeof(char), 1, img_ajuste) && row >= 0)
	{
		fread(&pix.G, sizeof(char), 1, img_ajuste);
		fread(&pix.R, sizeof(char), 1, img_ajuste);

		pix.X = column;
		pix.Y = row;
		column--;

		if (column == -1)
		{
			row--;
			column = img_ajuste_info.width;
		}

		bitmap_ajuste.push_back(pix);
		fprintf(tese, "%d %d %d %d %d \n'", pix.B, pix.G, pix.R, pix.X, pix.Y);
	}









}

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		cout << "NomePrograma ImagemReferencia ImagemAjuste " << endl;
		exit(1);
	}

	FILE* img_ref = fopen(argv[1], "rb");
	FILE* img_ajuste = fopen(argv[2], "rb");

	readBitMap(img_ref, img_ajuste);

	return 0;
}

