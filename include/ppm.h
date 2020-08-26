#ifndef PPM_H
#define PPM_H
#include<fstream>

using namespace std;


struct pixel{

    short int  r;
    short int  g;
    short int  b;

    // Constructor 
    pixel(short int r2=0, short int g2=0, short int b2 = 0) : r(r2), g(g2), b(b2) {}

	pixel& add(int k){ r += k; g += k; b += k; return *this; }

    pixel& sub(int k){ r -= k; g -= k; b -= k; return *this; }

    pixel& mult(float k){ r *= k; g *= k; b *= k; return *this; }

    pixel& addp(pixel p){ r += p.r; g += p.g; b += p.b; return *this; }

    short int cumsum(){ return r + g + b; } 

	pixel& truncate(){ 
	   r = (r > 255) ? 255 : ((r < 0) ? 0: (unsigned char)r); 
	   g = (g > 255) ? 255 : ((g < 0) ? 0: (unsigned char)g); 
	   b = (b > 255) ? 255 : ((b < 0) ? 0: (unsigned char)b);
	   return *this;
	}
};


class ppm {

private:

	void init(int _width, int _height) {
		this->height = _height;
		this->width = _width;
		this->max_col_val = _width * _height * 3;
		for (size_t i = 0; i < this->height; i++)
		{
			for (size_t j = 0; j < this->width; j++)
			{
				bitmap[i][j] = pixel();
			}
		}
	};

	ifstream OpenPPMFile(const string fname) {
		ifstream file;
		file.open(fname, ios::in, ios::binary);
		if (!file) {
			cout << "No se pudo abrir el archivo." << endl;
			exit(EXIT_FAILURE);
		}
		string version;
		file >> version;
		if (!version.compare("P6") == 0) {
			cout << "Formato de imagen invalida";
			exit(EXIT_FAILURE);
		}
		
	};

    unsigned int nr_lines;
    unsigned int nr_columns;
    vector<vector<pixel>> bitmap;

public:
    
    unsigned int height;
    unsigned int width;
    unsigned int max_col_val;
    unsigned int size;

    ppm();

    //create a PPM object and fill it with data stored in fname 
	ppm(const std::string &fname) {
		ifstream file = OpenPPMFile(fname);
		unsigned int width, height, max_col_val;
		file >> width;
		file >> height;
		init(width, height);
		for (vector<pixel> pixeles : this->bitmap) {
			for (pixel p : pixeles) {

			}
		}
	};

    //create an "epmty" PPM image with a given width and height;the R,G,B arrays are filled with zeros
	ppm(int _width, int _height) {
		init(_width, _height);
	};

    //read the PPM image from fname
    void read(const string &fname);

    //write the PPM image in fname
    void write(const string &fname);

    void setPixel(int i, int j, pixel p){ bitmap[i][j] = p; }
  
    pixel getPixel(int i, int j){ return bitmap[i][j]; }

};

#endif