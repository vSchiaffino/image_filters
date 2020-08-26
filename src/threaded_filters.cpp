#include "pch.h"
#include <iostream>
#include <string>
#include <stdlib.h>
#include <math.h>       /* sqrt */
#include <vector>
#include "filters.h"
#include <Windows.h>
#include <atomic>
#include <thread>

struct ARGS {
	ppm* img;
	int fila_start;
	int fila_end;
	char shades;
};

void threadedBlackWhitePt(LPVOID pArgs_) {
	ARGS *pArgs = (ARGS*)pArgs_;
	int fila_start = pArgs->fila_start;
	int fila_end = pArgs->fila_end;
	ppm* img = pArgs->img;

	for (size_t i = fila_start; i < fila_end; i++)
	{
		for (size_t j = 0; j < img->width; j++)
		{
			pixel p = img->getPixel(i, j);
			pixel* pixelResultante = new pixel();
			unsigned int gr = (p.r + p.g + p.b) / 3;
			pixelResultante->r = gr;
			pixelResultante->g = gr;
			pixelResultante->b = gr;
			img->setPixel(i, j, *pixelResultante);
		}
	}
}

void threadedBlackWhite(ppm& img, int nThreads) {
	// calculo como separar las filas
	int c_filas_por_thread = (int)(img.height / nThreads);
	int offset = img.height - (c_filas_por_thread * nThreads);
	// hago el pool de threads
	vector<HANDLE> thread_pool;
	for (size_t i = 0; i < nThreads; i++)
	{
		// calculo donde empieza y termina cada thread
		int start = i * c_filas_por_thread;
		int end = (i + 1) * c_filas_por_thread;
		if (i == nThreads - 1) {
			end += offset;
		}
		//cout << "[T" << i << "]S: " << start << " E: " << end << endl;
		// lleno los args
		ARGS args = {
			&img,
			start,
			end
		};
		// Creo el thread
		HANDLE hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)threadedBlackWhitePt, &args, 0, 0);
		thread_pool.push_back(hThread);
	}
	for (size_t i = 0; i < nThreads; i++)
	{
		// los espero
		WaitForSingleObject(thread_pool[i], INFINITE);
	}
}

void threadedShadesPt(LPVOID pArgs_) {
	ARGS *pArgs = (ARGS*)pArgs_;
	int fila_start = pArgs->fila_start;
	int fila_end = pArgs->fila_end;
	char shades = pArgs->shades;
	ppm* img = pArgs->img;

	for (size_t i = fila_start; i < fila_end; i++)
	{
		for (size_t j = 0; j < img->width; j++)
		{
			pixel p = img->getPixel(i, j);
			pixel* pixelResultante = new pixel();
			unsigned int rango = (255 / shades - 1);
			unsigned int gp = ((p.r + p.g + p.b) / 3);
			unsigned int gr = (gp / rango) * rango;
			pixelResultante->r = gr;
			pixelResultante->g = gr;
			pixelResultante->b = gr;
			img->setPixel(i, j, *pixelResultante);
		}
	}

};

void threadedShades(ppm& img, unsigned char shades, int nThreads) {
	// calculo como separar las filas
	int c_filas_por_thread = (int)(img.height / nThreads);
	int offset = img.height - (c_filas_por_thread * nThreads);
	// hago el pool de threads
	vector<HANDLE> thread_pool;
	for (size_t i = 0; i < nThreads; i++)
	{
		// calculo donde empieza y termina cada thread
		int start = i * c_filas_por_thread;
		int end = (i + 1) * c_filas_por_thread;
		if (i == nThreads - 1) {
			end += offset;
		}
		cout << "[T" << i << "]S: " << start << " E: " << end << endl;
		// lleno los args
		ARGS args = {
			&img,
			start,
			end,
			shades
		};
		// Creo el thread
		HANDLE hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)threadedShadesPt, &args, 0, 0);
		thread_pool.push_back(hThread);
	}
	for (size_t i = 0; i < nThreads; i++)
	{
		// los espero
		WaitForSingleObject(thread_pool[i], INFINITE);
	}
};