#include <iostream>
#include <string>
#include <stdlib.h>
#include <math.h>       /* sqrt */
#include <vector>
#include "../h/filters.h"
#include <atomic>
#include<thread>
#include <mutex>

using namespace std;
mutex ppmMutex;

void threadedBlackWhitePt(ppm* img, int fila_start, int fila_end) {

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
	vector<thread> thread_pool;
	for (size_t i = 0; i < nThreads; i++)
	{
		// calculo donde empieza y termina cada thread
		int start = i * c_filas_por_thread;
		int end = (i + 1) * c_filas_por_thread;
		if (i == nThreads - 1) {
			end += offset;
		}
		cout << "[T" << i << "]S: " << start << " E: " << end << endl;
		// Creo el thread
		thread_pool.push_back(thread(threadedBlackWhitePt, &img, start, end));
	}
	for (size_t i = 0; i < nThreads; i++)
	{
		// los espero
        thread_pool[i].join();
	}
}

void threadedShadesPt(ppm* img, int fila_start, int fila_end, unsigned char shades){

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
	vector<thread> thread_pool;
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
		
		// Creo el thread
		thread_pool.push_back(thread(threadedShadesPt, &img, start, end, shades));
	}
	for (size_t i = 0; i < nThreads; i++)
	{
		// los espero
		thread_pool[i].join();
	}
};

void threadedEdgeDetectionPt(ppm img, ppm *img_target, int fila_start, int fila_end){
	for (size_t i = fila_start + 1; i < fila_end - 1; i++)
	{
		for (size_t j = 1; j < img.width - 1; j++)
		{
			pixel p_final = pixel();
			// movy, movx
			// ppmMutex.lock();			
			pixel p0 = img.getPixel(i - 1, j - 1);
			pixel p1 = img.getPixel(i - 1, j);
			pixel p2 = img.getPixel(i - 1, j + 1);
			pixel p3 = img.getPixel(i, j - 1);
			pixel p4 = img.getPixel(i, j);
			pixel p5 = img.getPixel(i, j + 1);
			pixel p6 = img.getPixel(i + 1, j - 1);
			pixel p7 = img.getPixel(i + 1, j);
			pixel p8 = img.getPixel(i + 1, j + 1);
			// ppmMutex.unlock();
			//p_final.r = (p0.r + p1.r + p2.r + p3.r + p4.r + p5.r + p6.r + p7.r + p8.r) / 9;
			//p_final.g = (p0.g + p1.g + p2.g + p3.g + p4.g + p5.g + p6.g + p7.g + p8.g) / 9;
			//p_final.b = (p0.b + p1.b + p2.b + p3.b + p4.b + p5.b + p6.b + p7.b + p8.b) / 9;
			unsigned int gxr = (p0.r - p2.r + 2 * p3.r - 2 * p5.r + p6.r - p8.r);
			unsigned int gyr = (p0.r + 2 * p1.r + p2.r - p6.r - 2 * p7.r - p8.r);
			p_final.r = sqrt(gxr * gxr + gyr * gyr);
			unsigned int gxg = (p0.g - p2.g + 2 * p3.g - 2 * p5.g + p6.g - p8.g);
			unsigned int gyg = (p0.g + 2 * p1.g + p2.g - p6.g - 2 * p7.g - p8.g);
			p_final.g = sqrt(gxg * gxg + gyg * gyg);
			unsigned int gxb = (p0.b - p2.b + 2 * p3.b - 2 * p5.b + p6.b - p8.b);
			unsigned int gyb = (p0.b + 2 * p1.b + p2.b - p6.b - 2 * p7.b - p8.b);
			p_final.b = sqrt(gxb * gxb + gyb * gyb);
			p_final.truncate();
			// ppmMutex.lock();
			img_target->setPixel(i - 1, j - 1, p_final);
			// ppmMutex.unlock();
		}
	}
}

void threadedEdgeDetection(ppm &img, int nThreads) {
    ppm img_target(img.width - 1, img.height - 1);
	
	int c_filas_por_thread = (int)(img.height / nThreads);
	int offset = (img.height) - (c_filas_por_thread * nThreads);
	// hago el pool de threads
	vector<thread> thread_pool;
	for (size_t i = 0; i < nThreads; i++)
	{
		// calculo donde empieza y termina cada thread
		int start = i * c_filas_por_thread;
		int end = (i + 1) * c_filas_por_thread;
		if (i == nThreads - 1) {
			end += offset;
		}
		cout << "[T" << i << "]S: " << start << " E: " << end << endl;
		// Creo el thread
		thread_pool.push_back(thread(threadedEdgeDetectionPt, img, &img_target, start, end));
	}
	for (size_t i = 0; i < nThreads; i++)
	{
		// los espero
        thread_pool[i].join();
	}
	for (size_t i = 0; i < img_target.height; i++)
	{
		for (size_t j = 0; j < img_target.width; j++)
		{
			img.setPixel(i, j, img_target.getPixel(i, j));
		}
	}
	// img = img_target;


    
}
