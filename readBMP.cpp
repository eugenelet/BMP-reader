#include "readBMP.h"

unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader, BITMAPFILEHEADER *bitmapFileHeader)
{
    FILE *filePtr; // file pointer

    unsigned char *bitmapImage;  // image data
    int imageIdx=0;  // image index counter
    unsigned char tempRGB;  // swap variable

    //open filename in read binary mode
    filePtr = fopen(filename,"rb");
    if (filePtr == NULL)
        return NULL;

    //read the bitmap file header
    fread(bitmapFileHeader, sizeof(BITMAPFILEHEADER),1,filePtr);

    //verify that this is a bmp file by check bitmap id
    if (bitmapFileHeader->bfType !=0x4D42)
    {
        fclose(filePtr);
        return NULL;
    }

    //read the bitmap info header
    fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER),1,filePtr); 

    //move file point to the begging of bitmap data
    fseek(filePtr, bitmapFileHeader->bfOffBits, SEEK_SET);

    //allocate enough memory for the bitmap image data
    bitmapImage = (unsigned char*)malloc(bitmapInfoHeader->biSizeImage);

    //verify memory allocation
    if (!bitmapImage)
    {
        free(bitmapImage);
        fclose(filePtr);
        return NULL;
    }

    //read in the bitmap image data
    fread(bitmapImage,bitmapInfoHeader->biSizeImage,1,filePtr);

    //make sure bitmap image data was read
    if (bitmapImage == NULL)
    {
        fclose(filePtr);
        return NULL;
    }

    //swap the r and b values to get RGB (bitmap is BGR)
    for (imageIdx = 0;imageIdx < bitmapInfoHeader->biSizeImage;imageIdx+=3)
    {
        tempRGB = bitmapImage[imageIdx];
        bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
        bitmapImage[imageIdx + 2] = tempRGB;
    }

    //close file and return bitmap iamge data
    fclose(filePtr);
    return bitmapImage;
}