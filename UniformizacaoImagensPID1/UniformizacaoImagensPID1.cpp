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

	if(img_ajuste_info.width != img_ref_info.width || img_ajuste_info.height != img_ref_info.height)
	{
		cout << "Imagens não são do mesmo tamanho, processo de uniformização não pode ser executado." << endl;
		exit(1);
	}

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

void calcula_variancia(vector<Pixel> bitmaps, int m_RGB[], int V_RGB[])
{
	for (Pixel pix : bitmaps)
	{
		V_RGB[0] += pow((static_cast<int>(pix.B) - m_RGB[0]), 2.0);
		V_RGB[1] += pow((static_cast<int>(pix.G) - m_RGB[1]), 2.0);
		V_RGB[2] += pow((static_cast<int>(pix.R) - m_RGB[2]), 2.0);
	}

	V_RGB[0] /= bitmaps.size();
	V_RGB[1] /= bitmaps.size();
	V_RGB[2] /= bitmaps.size();
}

void calcula_ganho_offset(int mr[], int ma[], int Vr[], int Va[], double ganho[], double offset[])
{

	ganho[0] = sqrt(Vr[0] / Va[0]);
	ganho[1] = sqrt(Vr[1] / Va[1]);
	ganho[2] = sqrt(Vr[2] / Va[2]);

	offset[0] = mr[0] - ganho[0] * ma[0];
	offset[1] = mr[1] - ganho[1] * ma[1];
	offset[2] = mr[2] - ganho[2] * ma[2];

}

void correcao(double ganho[], double offset[])
{
	for (Pixel pix : bitmap_ajuste)
	{
		pix.B = ganho[0] * pix.B + offset[0];
		pix.G = ganho[1] * pix.G + offset[1];
		pix.R = ganho[2] * pix.R + offset[2];
	}
}

void calcula_correlacao(int mr[], int ma[], double p[], int Vr[], int Va[])
{
	double C[3] = { 0 };
	for (int i = 0; i < bitmap_ref.size(); i++)
	{
		C[0] += (bitmap_ref.at(i).B - mr[0]) * (bitmap_ajuste.at(i).B - ma[0]);
		C[1] += (bitmap_ref.at(i).G - mr[1]) * (bitmap_ajuste.at(i).G - ma[1]);
		C[2] += (bitmap_ref.at(i).R - mr[2]) * (bitmap_ajuste.at(i).R - ma[2]);
	}

	p[0] = C[0] / sqrt(Vr[0] * Va[0]);
	p[1] = C[1] / sqrt(Vr[1] * Va[1]);
	p[2] = C[2] / sqrt(Vr[2] * Va[2]);
}
void comparacao_sub_imagem()
{
	
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
	
	int mr[3] = { 0 };
	int ma[3] = { 0 };
	int Vr[3] = { 0 };
	int Va[3] = { 0 };
	double ganho[3] = { 0 };
	double offset[3] = { 0 };
	double p[3] = { 0 };

	ler_bitmap(img_ref, img_ajuste);
	calcula_media(bitmap_ref, mr);
	calcula_media(bitmap_ajuste, ma);
	calcula_variancia(bitmap_ref, mr, Vr);
	calcula_variancia(bitmap_ajuste, ma, Va);
	calcula_ganho_offset(mr, ma, Vr, Va, ganho, offset);
	calcula_correlacao(mr, ma, p, Vr, Va);

	return 0;
}

