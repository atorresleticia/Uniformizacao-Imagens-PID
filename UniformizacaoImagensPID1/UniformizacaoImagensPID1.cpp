#include "stdafx.h"
#include "bitmap.h"
#include "correlacao.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

BitMapFileHeader img_ref_header;
BitMapFileHeader img_ajuste_header;
BitMapInfoHeader img_ref_info;
BitMapInfoHeader img_ajuste_info;
Pixel pix;

vector<Pixel> bitmap_ref;
vector<Pixel> bitmap_ajuste;
vector<Pixel> subimagem_ref;
vector<Pixel> subimagem_ajuste;
vector<correlacao> correlacoes;

void ler_bitmap(FILE* img_ref, FILE* img_ajuste)
{
	fread(&img_ref_header, sizeof(BitMapFileHeader), 1, img_ref);
	fread(&img_ajuste_header, sizeof(BitMapFileHeader), 1, img_ajuste);
	fread(&img_ref_info, sizeof(BitMapInfoHeader), 1, img_ref);
	fread(&img_ajuste_info, sizeof(BitMapInfoHeader), 1, img_ajuste);

	FILE* ref_bitmap = fopen("bitmap_ref.txt", "w");
	FILE* ajuste_bitmap = fopen("bitmap_ajuste.txt", "w");

	int size_padding = (4 - img_ref_info.width * 3 % 4) % 4;
	char* padding = new char[size_padding + 1];

	for (int j = 0; j < img_ref_info.height; j++)
	{
		for (int i = 0; i < img_ref_info.width * 3; i += 3)
		{
			fread(&pix.B, sizeof(char), 1, img_ref);
			fread(&pix.G, sizeof(char), 1, img_ref);
			fread(&pix.R, sizeof(char), 1, img_ref);
			bitmap_ref.push_back(pix);
			fprintf(ref_bitmap, "%d %d %d\n", pix.B, pix.G, pix.R);
		}
		fread(padding, sizeof(char), size_padding, img_ref);
	}
	delete[] padding;

	size_padding = (4 - img_ajuste_info.width * 3 % 4) % 4;
	padding = new char[size_padding + 1];

	for (int j = 0; j < img_ajuste_info.height; j++)
	{
		for (int i = 0; i < img_ajuste_info.width * 3; i += 3)
		{
			fread(&pix.B, sizeof(char), 1, img_ajuste);
			fread(&pix.G, sizeof(char), 1, img_ajuste);
			fread(&pix.R, sizeof(char), 1, img_ajuste);
			bitmap_ajuste.push_back(pix);
			fprintf(ajuste_bitmap, "%d %d %d\n", pix.B, pix.G, pix.R);
		}
		fread(padding, sizeof(char), size_padding, img_ajuste);
	}
	delete[] padding;
	fclose(ref_bitmap);
	fclose(ajuste_bitmap);
}

void calcula_media_variancia(vector<Pixel> bitmap, double m_RGB[], double V_RGB[])
{
	m_RGB[0] = m_RGB[1] = m_RGB[2] = 0;
	V_RGB[0] = V_RGB[1] = V_RGB[2] = 0;

	for (Pixel pix : bitmap)
	{
		m_RGB[0] += static_cast<int>(pix.B);
		m_RGB[1] += static_cast<int>(pix.G);
		m_RGB[2] += static_cast<int>(pix.R);
		V_RGB[0] += pow((static_cast<int>(pix.B)), 2.0);
		V_RGB[1] += pow((static_cast<int>(pix.G)), 2.0);
		V_RGB[2] += pow((static_cast<int>(pix.R)), 2.0);
	}

	V_RGB[0] = (V_RGB[0] - pow(m_RGB[0], 2.0) / bitmap.size()) / bitmap.size();
	V_RGB[1] = (V_RGB[1] - pow(m_RGB[1], 2.0) / bitmap.size()) / bitmap.size();
	V_RGB[2] = (V_RGB[2] - pow(m_RGB[2], 2.0) / bitmap.size()) / bitmap.size();

	m_RGB[0] /= bitmap.size();
	m_RGB[1] /= bitmap.size();
	m_RGB[2] /= bitmap.size();
}

void calcula_ganho_offset(double mr[], double ma[], double Vr[], double Va[], double ganho[], double offset[])
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
	FILE* imagem_uniformizada = fopen("bitmap_imagem_uniformizada.txt", "w");


	for (Pixel pix : bitmap_ajuste)
	{
		pix.B = ganho[0] * static_cast<int>(pix.B) + offset[0];
		pix.G = ganho[1] * static_cast<int>(pix.G) + offset[1];
		pix.R = ganho[2] * static_cast<int>(pix.R) + offset[2];
		fprintf(imagem_uniformizada, "%d %d %d\n", pix.B, pix.G, pix.R);
	}

	fclose(imagem_uniformizada);
}

void calcula_correlacao(double mr[], double ma[], double Vr[], double Va[], double p[])
{
	double C[3] = {0};

	for (int i = 0; i < subimagem_ref.size(); i++)
	{
		C[0] += (static_cast<int>(subimagem_ref.at(i).B) - mr[0]) * (static_cast<int>(subimagem_ajuste.at(i).B) - ma[0]);
		C[1] += (static_cast<int>(subimagem_ref.at(i).G) - mr[1]) * (static_cast<int>(subimagem_ajuste.at(i).G) - ma[1]);
		C[2] += (static_cast<int>(subimagem_ref.at(i).R) - mr[2]) * (static_cast<int>(subimagem_ajuste.at(i).R) - ma[2]);
	}

	if (Vr[0] == 0 && Va[0] == 0)
	{
		p[0] = 1;
	}
	else if (Vr[0] == 0 ^ Va[0] == 0)
	{
		p[0] = 0;
	}
	else
	{
		p[0] = (C[0] / subimagem_ref.size()) / sqrt(Vr[0] * Va[0]);
	}

	if (Vr[1] == 0 && Va[1] == 0)
	{
		p[1] = 1;
	}
	else if (Vr[1] == 0 ^ Va[1] == 0)
	{
		p[1] = 0;
	}
	else
	{
		p[1] = (C[1] / subimagem_ref.size()) / sqrt(Vr[1] * Va[1]);
	}

	if (Vr[2] == 0 && Va[2] == 0)
	{
		p[2] = 1;
	}
	else if (Vr[2] == 0 ^ Va[2] == 0)
	{
		p[2] = 0;
	}
	else
	{
		p[2] = (C[2] / subimagem_ref.size()) / sqrt(Vr[2] * Va[2]);		
	}

}

void ler_subimagem(int x_inicio, int y_inicio, int x_final, int y_final)
{
	int width = img_ajuste_info.width;

	for (int j = y_inicio; j <= y_final; j++)
	{
		for (int i = x_inicio; i <= x_final; i++)
		{
			subimagem_ajuste.push_back(bitmap_ajuste.at(j * width + i));
		}
	}
}

void ler_subimagem_ref(int x_inicio, int y_inicio, int x_final, int y_final)
{
	int width = img_ref_info.width;

	for (int j = y_inicio; j <= y_final; j++)
	{
		for (int i = x_inicio; i <= x_final; i++)
		{
			subimagem_ref.push_back(bitmap_ref.at(j * width + i));
		}
	}
}

void comparacao_sub_imagem(int x_inicio, int y_inicio, int x_final, int y_final, double mr[], double ma[], double Vr[], double Va[])
{
	double p[3] = {0};
	int width = x_final - x_inicio;
	int height = y_final - y_inicio;
	correlacao C;
	FILE* correlacao = fopen("correlacoes.txt", "w");

	calcula_media_variancia(subimagem_ref, mr, Vr);

	fprintf(correlacao, "SUBIMAGEM ->\t (%d, %d) a (%d, %d)\n\n", x_inicio, y_inicio, x_final, y_final);
	fprintf(correlacao, "Casamentos da subimagem na segunda imagem:\n");
	fprintf(correlacao, "Posicao de correlacao \t Indice de Correlacao { B, G, R }\n");

	for (int j = 0; j < img_ajuste_info.height; j++)
	{
		for (int i = 0; i < img_ajuste_info.width; i++)
		{
			if (i >= img_ajuste_info.width - width || j >= img_ajuste_info.height - height)
			{
				continue;
			}

			ler_subimagem(i, j, i + width, j + height);
			calcula_media_variancia(subimagem_ajuste, ma, Va);
			calcula_correlacao(mr, ma, Vr, Va, p);
			subimagem_ajuste.clear();

			if (p[0] != 0 && p[1] != 0 && p[2] != 0)
			{
				C.correlacao_B = p[0];
				C.correlacao_G = p[1];
				C.correlacao_R = p[2];
				C.x_inicial = i;
				C.x_final = i + width;
				C.y_inicial = j;
				C.y_final = j + height;
				correlacoes.push_back(C);

				fprintf(correlacao, "\t(%d, %d) a (%d, %d)\t\t\t", correlacoes.back().x_inicial, 
					correlacoes.back().y_inicial, correlacoes.back().x_final, correlacoes.back().y_final);
				fprintf(correlacao, "{ %.3f, %.3f, %.3f } \n", correlacoes.back().correlacao_B,
					correlacoes.back().correlacao_G, correlacoes.back().correlacao_R);
			}
		}
	}
	fclose(correlacao);
}

int main(int argc, char* argv[])
{
	if (argc != 7)
	{
		cout << endl << "Argumentos = <nome_programa> <imagem_referencia> <imagem_ajuste> <x_inicial> <y_inicial> <x_final> <y_final> " << endl;
		exit(1);
	}

	FILE* img_ref = fopen(argv[1], "rb");
	FILE* img_ajuste = fopen(argv[2], "rb");
	int x_inicio = atoi(argv[3]);
	int y_inicio = atoi(argv[4]);
	int x_final = atoi(argv[5]);
	int y_final = atoi(argv[6]);

	if (!img_ref)
	{
		cout << "Erro ao abrir a imagem refer�ncia" << endl;
		exit(1);
	}
	if (!img_ajuste)
	{
		cout << "Erro ao abrir a imagem ajuste" << endl;
		exit(1);
	}

	double mr[3] = {0};
	double ma[3] = {0};
	double Vr[3] = {0};
	double Va[3] = {0};
	double ganho[3] = {0};
	double offset[3] = {0};

	ler_bitmap(img_ref, img_ajuste);
	calcula_media_variancia(bitmap_ref, mr, Vr);
	calcula_media_variancia(bitmap_ajuste, ma, Va);
	calcula_ganho_offset(mr, ma, Vr, Va, ganho, offset);
	correcao(ganho, offset);
	ler_subimagem_ref(x_inicio, y_inicio, x_final, y_final);
	comparacao_sub_imagem(x_inicio, y_inicio, x_final, y_final, mr, ma, Vr, Va);

	fclose(img_ref);
	fclose(img_ajuste);
	return 0;
}