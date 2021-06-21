#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream.h> //-----------------------------
using namespace std;

#pragma pack(push, 2)
struct BITMAPHEADER {
    unsigned char bfType[2];
    unsigned char bfSize[4];
    unsigned char bfReserved1[2];
    unsigned char bfReserved2[2];
    unsigned char bfOffsetBits[4];
    
    unsigned char biSize[4];
    unsigned char biWidth[4];
    unsigned char biHeight[4];
    unsigned char biPlanes[2];
    unsigned char biBitCount[2];
    unsigned char biCompression[4];
    unsigned char biSizeImage[4];
    unsigned char biXpelsPerMeter[4];
    unsigned char biYpelsPerMeter[4];
    unsigned char biClrUsed[4];
    unsigned char biClrImportant[4];
 
} bmpHeader;
#pragma pack(pop)

struct PIXEL {
    unsigned char Blue;
    unsigned char Green;
    unsigned char Red;
};

class BitmapFile {
    BITMAPHEADER head;
    PIXEL* img;
    int width;
    int height;
    int size;
    
    public:
        void bmp_read(char *filename);
        void bmp_write(char *filename);
        void bmp_invert_horizontal();
        
        void bmp_increase_Y(int arg);
};

void BitmapFile::bmp_read(char *filename) {
    unsigned int i;
    FILE* file;
    file = fopen(filename, "rb");
    
    fread(&bmpHeader, sizeof(bmpHeader), 1, file);
    
    width = *(int*)bmpHeader.biWidth;
    height = *(int*)bmpHeader.biHeight;
    size = width * height;
    
    img = new PIXEL[size];
    for(i = 0; i < size; i++)
        fread(&img[i], sizeof(PIXEL), 1, file);
        
    fclose(file);
}

void BitmapFile::bmp_write(char *filename) {
    unsigned int i;
    FILE* file;
    file = fopen(filename, "wb");
    
    fwrite(&bmpHeader, sizeof(bmpHeader), 1, file);
    for(i = 0; i < size; i++)
        fwrite(&img[i], sizeof(PIXEL), 1, file);
    
    fclose(file);
}


void YCbCrfromRGB(double& Y, double& Cb, double& Cr, const double r, const double g, const double b)
{
  Y =  0.299*r + 0.587*g + 0.114*b;
  Cb = 0.564 * (b - Y);
  Cr =  0.713 * (r - Y);
  
  if(Y>255) Y=255;
  else if(Y<0) Y=0;
  if(Cb>255) Cb=255;
  else if(Cb<0) Cb=0;
  if(Cr>255) Cr=255;
  else if(Cr<0) Cr=0;
}
void RGBfromYCbCr(double& r, double& g, double& b, double Y, double Cb, double Cr)
{
     r = Y + 1.402*Cr;
     g = Y - 0.344*Cb - 0.714*Cr;
     b = Y + 1.772*Cb;
     
     
     if(r>255) r=255;
     else if(r<0) r=0;
     if(g>255) g=255;
     else if(g<0) g=0;
     if(b>255) b=255;
     else if(b<0) b=0;
}

void BitmapFile::bmp_increase_Y(int arg) {
    unsigned int i;
    double R, G, B;
    double Y, Cb, Cr;
    
    for(i = 0; i < size; i++) {
        R = img[i].Red;
        G = img[i].Green;
        B = img[i].Blue;
        
        YCbCrfromRGB(Y, Cb, Cr, R, G, B);
        Y += arg;
        RGBfromYCbCr(R, G, B, Y, Cb, Cr);

        img[i].Red = (unsigned char)round(R);
        img[i].Green = (unsigned char)round(G);
        img[i].Blue = (unsigned char)round(B);
    };
}

int main(int argc, char **argv)
{
    int i;
    int arg;
    BitmapFile picture;
    
    if (argc != 3) {
		printf("Usage: .exe <picture.bmp> <modifier>\n");
		printf("Modifiers:\n");
		printf("h - inverts picture horizontally\n");
		printf("<int> - modifies brightness by given value\n");
		return -1;
	}
    
    picture.bmp_read(argv[1]);
    

        arg = 0;
        if(argv[2][0] == '-') {
            for (i = 1; argv[2][i] != '\0'; ++i)
                arg = arg*10 + argv[2][i] - '0';
            arg = -arg;
        } else {
            for (i = 0; argv[2][i] != '\0'; ++i)
                arg = arg*10 + argv[2][i] - '0';
        }
        
        picture.bmp_increase_Y(arg);
    
    picture.bmp_write(argv[1]);
    
   
    return 0;
}
