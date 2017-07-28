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

void calcula_media(vector<Pixel> bitmaps, int m_RGB[])
{
	for (Pixel pix : bitmaps)
	{
		m_RGB[0] += static_cast<int>(pix.B);
		m_RGB[1] += static_cast<int>(pix.G);
		m_RGB[2] += static_cast<int>(pix.R);
	}

	m_RGB[0] /= bitmaps.size();
	m_RGB[1] /= bitmaps.size();
	m_RGB[2] /= bitmaps.size();
}

void calcula_variancia(vector<Pixel> bitmaps, int media_img[], int V_RGB[])
{
	for (Pixel pix : bitmaps)
	{
		V_RGB[0] += pow((static_cast<int>(pix.B) - media_img[0]), 2.0);
		V_RGB[1] += pow((static_cast<int>(pix.G) - media_img[1]), 2.0);
		V_RGB[2] += pow((static_cast<int>(pix.R) - media_img[2]), 2.0);
	}

	V_RGB[0] /= bitmaps.size();
	V_RGB[1] /= bitmaps.size();
	V_RGB[2] /= bitmaps.size();
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

	int mr[3] = { 0 };
	int ma[3] = { 0 };
	int Vr[3] = { 0 };
	int Va[3] = { 0 };

	calcula_media(bitmap_ref, mr);
	calcula_media(bitmap_ajuste, ma);
	calcula_variancia(bitmap_ref, mr, Vr);
	calcula_variancia(bitmap_ajuste, ma, Va);

	return 0;
}

