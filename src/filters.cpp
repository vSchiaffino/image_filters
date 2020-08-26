#include <iostream>
#include <string>
#include <stdlib.h>
#include <math.h>       /* sqrt */
#include <vector>
#include "filters.h"
#include <Windows.h>
#include <atomic>
#include <thread>

#define BLACK 0

using namespace std;

// COMPLETAR :)


void blackWhite(ppm& img) {
	for (size_t i = 0; i < img.height; i++)
	{
		for (size_t j = 0; j < img.width; j++)
		{
			pixel p = img.getPixel(i, j);
			pixel* pixelResultante = new pixel();
			unsigned int gr = (p.r + p.g + p.b) / 3;
			pixelResultante->r = gr;
			pixelResultante->g = gr;
			pixelResultante->b = gr;
			img.setPixel(i, j, *pixelResultante);
		}
	}
}

unsigned int f(float c, unsigned int value) {
	return (259 * (c + 255)) / (255 * (259 - c)) * value;
}

void contrast(ppm& img, float contrast) {
	// esta mal esto
	for (size_t i = 0; i < img.height; i++)
	{
		for (size_t j = 0; j < img.width; j++)
		{
			pixel p = img.getPixel(i, j);
			pixel* pixelResultante = new pixel();
			pixelResultante->r = f(contrast, p.r - 128) + 128;
			pixelResultante->g = f(contrast, p.g - 128) + 128;
			pixelResultante->b = f(contrast, p.b - 128) + 128;
			pixelResultante->truncate();
			img.setPixel(i, j, *pixelResultante);
			
		}
	}
};

void brightness(ppm& img, float b, int start, int end) {
	for (size_t i = 0; i < img.height; i++)
	{
		for (size_t j = 0; j < img.width; j++)
		{
			pixel p = img.getPixel(i, j);
			pixel* pixelResultante = new pixel();
			pixelResultante->r = p.r + 255 * b;
			pixelResultante->g = p.g + 255 * b;
			pixelResultante->b = p.b + 255 * b;
			pixelResultante->truncate();

			img.setPixel(i, j, *pixelResultante);
		}
	}
};

void shades(ppm& img, unsigned char shades){
	for (size_t i = 0; i < img.height; i++)
	{
		for (size_t j = 0; j < img.width; j++)
		{
			pixel p = img.getPixel(i, j);
			pixel* pixelResultante = new pixel();
			unsigned int rango = (255 / shades - 1);
			unsigned int gp = ((p.r + p.g + p.b) / 3);
			unsigned int gr = (gp / rango) * rango;
			pixelResultante->r = gr;
			pixelResultante->g = gr;
			pixelResultante->b = gr;


			img.setPixel(i, j, *pixelResultante);
		}
	}
};

void merge(ppm& img1, ppm& img2, float alpha) {
	for (size_t i = 0; i < img1.height; i++)
	{
		for (size_t j = 0; j < img1.width; j++)
		{
			float porc1 = alpha;
			float porc2 = 1 - alpha;
			pixel p1 = img1.getPixel(i, j);
			pixel p2 = img2.getPixel(i, j);
			pixel* pixelResultante = new pixel();

			pixelResultante->r = p1.r * porc1 + p2.r * porc2;
			pixelResultante->g = p1.g * porc1 + p2.g * porc2;
			pixelResultante->b = p1.b * porc1 + p2.b * porc2;


			img1.setPixel(i, j, *pixelResultante);
		}
	}
};

void frame(ppm& img, pixel color, int _x) {
	for (size_t offset = 0; offset < _x; offset++)
	{
		int y = 0 + offset;
		int x = 0 + offset;
		for (size_t _ = 0; _ < img.width - 1 - offset * 2; _++)
		{
			img.setPixel(y, x, color);
			x += 1;
		}
		for (size_t _ = 0; _ < img.height - 1 - offset * 2; _++)
		{
			img.setPixel(y, x, color);
			y += 1;
		}
		for (size_t _ = 0; _ < img.width - 1 - offset * 2; _++)
		{
			img.setPixel(y, x, color);
			x -= 1;
		}
		for (size_t _ = 0; _ < img.height - 1 - offset * 2; _++)
		{
			img.setPixel(y, x, color);
			y -= 1;
		}
	}
};

void zoom(ppm &img, ppm &img_zoomed, int z) {
	for (size_t i = 0; i < img.height; i++)
	{
		for (size_t j = 0; j < img.width; j++)
		{
			pixel p = img.getPixel(i, j);

			unsigned int result_j = (j + (j * (z - 1)));
			unsigned int result_i = (i + (i * (z - 1)));
			// img_zoomed.setPixel(result_i, result_j, p);
			for (size_t offset_i = 0; offset_i < z; offset_i++)
			{
				for (size_t offset_j = 0; offset_j < z; offset_j++)
				{
					img_zoomed.setPixel(result_i + offset_i, result_j + offset_j, p);
				}
			}
		}
	}
};

void boxBlur(ppm &img) {
	ppm new_image = ppm(img.width - 2, img.height - 2);
	for (size_t i = 1; i < img.height - 1; i++)
	{
		for (size_t j = 1; j < img.width - 1; j++)
		{
			pixel p_final = pixel();
			pixel p0 = img.getPixel(i - 1, j - 1);
			pixel p1 = img.getPixel(i - 1, j);
			pixel p2 = img.getPixel(i - 1, j + 1);
			pixel p3 = img.getPixel(i, j - 1);
			pixel p4 = img.getPixel(i, j);
			pixel p5 = img.getPixel(i, j + 1);
			// movy, movx
			pixel p6 = img.getPixel(i + 1, j - 1);
			pixel p7 = img.getPixel(i + 1, j);
			pixel p8 = img.getPixel(i + 1, j + 1);
			p_final.r = (p0.r + p1.r + p2.r + p3.r + p4.r + p5.r + p6.r + p7.r + p8.r) / 9;
			p_final.g = (p0.g + p1.g + p2.g + p3.g + p4.g + p5.g + p6.g + p7.g + p8.g) / 9;
			p_final.b = (p0.b + p1.b + p2.b + p3.b + p4.b + p5.b + p6.b + p7.b + p8.b) / 9;

			p_final.truncate();
			new_image.setPixel(i - 1, j - 1, p_final);
		}
	}
	img = new_image;
};

void edgeDetection(ppm &img, ppm &img_target) {
	for (size_t i = 1; i < img.height - 1; i++)
	{
		for (size_t j = 1; j < img.width - 1; j++)
		{
			pixel p_final = pixel();
			pixel p0 = img.getPixel(i - 1, j - 1);
			pixel p1 = img.getPixel(i - 1, j);
			pixel p2 = img.getPixel(i - 1, j + 1);
			pixel p3 = img.getPixel(i, j - 1);
			pixel p4 = img.getPixel(i, j);
			pixel p5 = img.getPixel(i, j + 1);
			// movy, movx
			pixel p6 = img.getPixel(i + 1, j - 1);
			pixel p7 = img.getPixel(i + 1, j);
			pixel p8 = img.getPixel(i + 1, j + 1);
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
			img_target.setPixel(i - 1, j - 1, p_final);
		}
	}
}

void crop(ppm &img, int k, int t){
	ppm result = ppm(img.width - t, img.height - k);
	for (size_t i = k; i < img.height; i++)
	{
		for (size_t j = t; j < img.width; j++)
		{
			result.setPixel(i - k, j - t, img.getPixel(i, j));
		}
	}
	img = result;
};