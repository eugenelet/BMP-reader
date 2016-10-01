#include "bitmapRW.h"

unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader,
 BITMAPFILEHEADER *bitmapFileHeader)
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
    if(bitmapInfoHeader->biBitCount!=32 && (bitmapInfoHeader->biWidth%4 != 0))
        bitmapImage = (unsigned char*)malloc(bitmapInfoHeader->biSizeImage 
            - (4 - (bitmapInfoHeader->biSizeImage%4))*bitmapInfoHeader->biHeight);
    else
        bitmapImage = (unsigned char*)malloc(bitmapInfoHeader->biSizeImage);

    //verify memory allocation
    if (!bitmapImage)
    {
        free(bitmapImage);
        fclose(filePtr);
        return NULL;
    }

    int bytesPerPixel = (bitmapInfoHeader->biBitCount) / 8;

    //read in the bitmap image data
    int padding = 4 - (bytesPerPixel*bitmapInfoHeader->biWidth)%4;
    if(bitmapInfoHeader->biBitCount!=32 && (bytesPerPixel*bitmapInfoHeader->biWidth%4 != 0)){
        for(int row = 0; row < bitmapInfoHeader->biHeight; row++){
            for(int col = 0; col < bytesPerPixel*bitmapInfoHeader->biWidth; col++)
                fread(&bitmapImage[row*bytesPerPixel*bitmapInfoHeader->biWidth + col],1,1,filePtr);
            for(int pad = 0; pad < padding; pad++)
                fseek(filePtr,1,SEEK_CUR);
        }
    }
    else
        fread(bitmapImage,bitmapInfoHeader->biSizeImage,1,filePtr);

    //make sure bitmap image data was read
    if (bitmapImage == NULL)
    {
        fclose(filePtr);
        return NULL;
    }

    //swap the r and b values to get RGB (bitmap is BGR)
    for (imageIdx = 0;imageIdx < bitmapInfoHeader->biWidth*bitmapInfoHeader->biHeight*bytesPerPixel;imageIdx+=bytesPerPixel)
    {
        tempRGB = bitmapImage[imageIdx];
        bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
        bitmapImage[imageIdx + 2] = tempRGB;
    }

    //close file and return bitmap iamge data
    fclose(filePtr);
    return bitmapImage;
}

void WriteBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader, 
    BITMAPFILEHEADER *bitmapFileHeader, unsigned char *bitmapImage)
{
    FILE *filePtr; // file pointer

    int imageIdx=0;  // image index counter
    unsigned char tempRGB;  // swap variable

    //open filename in write binary mode
    filePtr = fopen(filename,"wb");
    if (filePtr == NULL)
        return;

    //verify that this is a bmp file by check bitmap id
    if (bitmapFileHeader->bfType !=0x4D42)
    {
        fclose(filePtr);
        return;
    }

    //write the bitmap file header
    fwrite(bitmapFileHeader, sizeof(BITMAPFILEHEADER),1,filePtr);


    //write the bitmap info header
    fwrite(bitmapInfoHeader, sizeof(BITMAPINFOHEADER),1,filePtr); 

    //move file point to the begging of bitmap data
    fseek(filePtr, bitmapFileHeader->bfOffBits, SEEK_SET);


    //make sure bitmap image data is not empty
    if (bitmapImage == NULL)
    {
        std::cout << "Image empty!" << std::endl;
        fclose(filePtr);
        return;
    }

    int bytesPerPixel = (bitmapInfoHeader->biBitCount) / 8;
    //swap the r and b values to get RGB (bitmap is BGR)
    for (imageIdx = 0;imageIdx < bitmapInfoHeader->biWidth*bitmapInfoHeader->biHeight*bytesPerPixel;imageIdx+=bytesPerPixel)
    {
        tempRGB = bitmapImage[imageIdx];
        bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
        bitmapImage[imageIdx + 2] = tempRGB;
    }

    //write the bitmap image data
    unsigned char* null_char = new unsigned char;
    *null_char = 0;
    int padding = 4 - (bytesPerPixel*bitmapInfoHeader->biWidth)%4;
    if(bitmapInfoHeader->biBitCount!=32 && (bytesPerPixel*bitmapInfoHeader->biWidth%4 != 0)){
        for(int row = 0; row < bitmapInfoHeader->biHeight; row++){
            for(int col = 0; col < bytesPerPixel*bitmapInfoHeader->biWidth; col++)
                fwrite(&bitmapImage[row*bytesPerPixel*bitmapInfoHeader->biWidth 
                    + col],1,1,filePtr);
            for(int pad = 0; pad < padding; pad++)
                fwrite(null_char,1,1,filePtr);
        }
    }
    else
        fwrite(bitmapImage,bitmapInfoHeader->biSizeImage,1,filePtr);



    //close file and return bitmap iamge data
    fclose(filePtr);
}