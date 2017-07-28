/* 1. Leitura e armazenamento do bitmap OK
* 2. Cálculo da média de cada componente OK
* 3. Cálculo da variância de cada componente OK
* 4. Offset
* 5. Ganho
* 6. Aplicar 4 e 5 em cada pixel
* 7. Calcular correlação
*/

// A leitura das cores é ao contrário


#include "stdafx.h"
#include "bitmap.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

BitMapFileHeader img_ref_header;
BitMapFileHeader img_ajuste_header;
BitMapInfoHeader img_ref_info;
BitMapInfoHeader img_ajuste_info;
Pixel pix;
vector<Pixel> bitmap_ref;
vector<Pixel> bitmap_ajuste;

void ler_bitmap(FILE* img_ref, FILE* img_ajuste)
{
	fread(&img_ref_header, sizeof(BitMapFileHeader), 1, img_ref);
	fread(&img_ajuste_header, sizeof(BitMapFileHeader), 1, img_ajuste);
	fread(&img_ref_info, sizeof(BitMapInfoHeader), 1, img_ref);
	fread(&img_ajuste_info, sizeof(BitMapInfoHeader), 1, img_ajuste);

	FILE* tes = fopen("tes.txt", "w");
	FILE* tese = fopen("tese.txt", "w");

	while (fread(&pix, sizeof(Pixel), 1, img_ref))
	{
		bitmap_ref.push_back(pix);
		fprintf(tes, "%d %d %d\n", pix.B, pix.G, pix.R);
	}

	while (fread(&pix, sizeof(Pixel), 1, img_ajuste))
	{
		bitmap_ajuste.push_back(pix);
		fprintf(tese, "%d %d %d\n", pix.B, pix.G, pix.R);
	}
}

int* calcula_media(vector<Pixel> bitmaps)
{
	int mediaRGB[3] = { 0 };

	for (Pixel pix : bitmaps)
	{
		mediaRGB[0] += pix.B;
		mediaRGB[1] += pix.G;
		mediaRGB[2] += pix.R;
	}

	mediaRGB[0] /= bitmaps.size();
	mediaRGB[1] /= bitmaps.size();
	mediaRGB[2] /= bitmaps.size();

	return mediaRGB;
}

int* calcula_variancia(vector<Pixel> bitmaps, int* media_img)
{
	int varianciaRGB[3] = { 0 };

	for (Pixel pix : bitmaps)
	{
		varianciaRGB[0] += pow((pix.B - media_img[0]), 2);
		varianciaRGB[1] += pow((pix.G - media_img[1]), 2);
		varianciaRGB[2] += pow((pix.R - media_img[2]), 2);
	}

	varianciaRGB[0] /= bitmaps.size();
	varianciaRGB[1] /= bitmaps.size();
	varianciaRGB[2] /= bitmaps.size();

	return varianciaRGB;
}

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		cout << endl << "Argumentos = <nome_programa> <imagem_referencia> <imagem_ajuste> " << endl;
		exit(1);
	}

	FILE* img_ref = fopen(argv[1], "rb");
	FILE* img_ajuste = fopen(argv[2], "rb");

	if (!img_ref)
	{
		cout << "Erro ao abrir a imagem referência" << endl;
		exit(1);
	}
	if (!img_ajuste)
	{
		cout << "Erro ao abrir a imagem ajuste" << endl;
		exit(1);		
	}

	ler_bitmap(img_ref, img_ajuste);

	int* mr = calcula_media(bitmap_ref);
	int* ma = calcula_media(bitmap_ajuste);
	int* Vr = calcula_variancia(bitmap_ref, mr);
	int* Va = calcula_variancia(bitmap_ajuste, ma);

	return 0;
}

