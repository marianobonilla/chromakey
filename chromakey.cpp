/*********************************************************************
 File:     chromakey.cpp

 Author: Mariano Bonilla 

 Email address: marianob@usc.edu

 Usage: program_name in.bmp background.bmp dist_threshold out1.bmp out2.bmp

 Notes:
 This program performs the chroma key operation on an input 
 using two different methods.

********************************************************************/


#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include "bmplib.h"

using namespace std;

void method1(unsigned char inImage[][SIZE][RGB], 
	     bool mask[][SIZE],
	     double threshold);

void method2(unsigned char inImage[][SIZE][RGB], 
	     bool mask[][SIZE]);

void replace(bool mask[][SIZE],
	     unsigned char inImage[][SIZE][RGB],
	     unsigned char bgImage[][SIZE][RGB],
	     unsigned char outImage[][SIZE][RGB]);

int main(int argc, char *argv[])
{

  static unsigned char inputImage[SIZE][SIZE][RGB];
  static unsigned char bgrndImage[SIZE][SIZE][RGB];
  static unsigned char outputImage[SIZE][SIZE][RGB];
  static bool chromaMask[SIZE][SIZE];

  double threshold;

  if (argc < 6) {
    cerr << "usage: program_name in.bmp background.bmp dist_threshold " 
         << "out1.bmp out2.bmp" << endl;
    return 0;
  }
	
  if (readRGBBMP(argv[1], inputImage)) {
    cerr << "Error reading file: " << argv[1] << endl;
    return 1;
  }

  if (readRGBBMP(argv[2], bgrndImage)) {
    cout << "Error reading file: " << argv[2] << endl;
    return 1;
  }
  

  threshold = atof(argv[3]);

  // Call Method 1 Function
  method1(inputImage, chromaMask,threshold );

  // Produce the output by calling replace()
  replace(chromaMask, inputImage, bgrndImage,
  	     outputImage);

  // Write the output image to a file using the filename argv[4]
  if (writeRGBBMP(argv[4], outputImage)) {
    cout << "Error writing file: " << argv[4] << endl;
    exit(1);
  }	

  //Call Method 2 Function
  method2(inputImage, 
  	     chromaMask);


  // Produce the output by calling replace()
  replace(chromaMask, inputImage, bgrndImage,
    	     outputImage);

  // Write the output image to a file using the filename argv[5]
  if (writeRGBBMP(argv[5], outputImage)) {
    cout << "Error writing file: " << argv[5] << endl;
    exit(1);
  }	

  return 0;
}



// Use user-provided threshold for chroma-key distance
// The "output" of this method is to produce a valid mask array
//  where entries in the mask array are 1 for foreground image
//  and 0 for 'background'
void method1(unsigned char inImage[][SIZE][RGB], 
	     bool mask[][SIZE],
	     double threshold)
{
	int averageRGB[3] = {0, 0, 0};
	  for(int y=0; y<SIZE; y++){
	    for(int z=0; z<3; z++){
		  averageRGB[z] += inImage[0][y][z];
		}
	  }

	  averageRGB[0] = averageRGB[0] / SIZE;
	  averageRGB[1] = averageRGB[1] / SIZE;
	  averageRGB[2] = averageRGB[2] / SIZE;
	  
	//int averageRGB[] = {62,121,22};
	//int averageRGB[] = {55,135,23};
	for(int i=0; i<SIZE; i++){	
		for(int j=0; j<SIZE; j++){


			if (sqrt(pow((inImage[i][j][0] - averageRGB[0]), 2) + 
						 pow((inImage[i][j][1] - averageRGB[1]), 2) + 
						 pow((inImage[i][j][2] - averageRGB[2]), 2)
				) < threshold ){
			  mask[i][j] = 0;
			} else {
			  mask[i][j] = 1;
			}
		}
	  }


}


// The "output" of this method is to produce a valid mask array
//  where entries in the mask array are 1 for foreground image
//  and 0 for 'background'
void method2(unsigned char inImage[][SIZE][RGB], 
	     bool mask[][SIZE])  
{
	int averageRGB[3] = {0, 0, 0};
	  for(int y=0; y<SIZE; y++){
	    for(int z=0; z<3; z++){
		  averageRGB[z] += inImage[0][y][z];
		}
	  }

	  averageRGB[0] = averageRGB[0] / SIZE;
	  averageRGB[1] = averageRGB[1] / SIZE;
	  averageRGB[2] = averageRGB[2] / SIZE;


double autoThreshold[3] = {0,0,0};
//lets assume that the image will more or less be near the center and calculated average rgb value.

	for (int j = 0; j < 225; ++j)
	{
		autoThreshold[0] += inImage[j][130][0];
		autoThreshold[1] += inImage[j][130][1];
		autoThreshold[2] += inImage[j][130][2];
	}


autoThreshold[0] = autoThreshold[0] /225;
autoThreshold[1] = autoThreshold[1] /225;
autoThreshold[2] = autoThreshold[2] /225;

double threshold_Auto = sqrt(pow(autoThreshold[0] - averageRGB[0],2) + pow(autoThreshold[1]-averageRGB[1],2)+ pow(autoThreshold[2] - averageRGB[2],2));

// Now calculating the average value of the chromakey rgb values to compare the threshold value obtained above

for(int i=0; i<SIZE; i++){	
	for(int j=0; j<SIZE; j++){

		if (sqrt(pow((inImage[i][j][0] - averageRGB[0]), 2) + 
					 pow((inImage[i][j][1] - averageRGB[1]), 2) + 
					 pow((inImage[i][j][2] - averageRGB[2]), 2)
			) < threshold_Auto ){
		  mask[i][j] = 0;
		} else {
		  mask[i][j] = 1;
		}

		if (sqrt(pow((inImage[i][j][0] - averageRGB[0]), 2) + 
					 pow((inImage[i][j][1] - averageRGB[1]), 2) + 
					 pow((inImage[i][j][2] - averageRGB[2]), 2)) < 50) // values that are two near should be chromakey
		{
			mask[i][j] = 0;
		}

	}
  }

}

// If mask[i][j] = 1 use the input image pixel for the output image
// Else if mask[i][j] = 0 use the background image pixel
void replace(bool mask[SIZE][SIZE],
	     unsigned char inImage[SIZE][SIZE][RGB],
	     unsigned char bgImage[SIZE][SIZE][RGB],
	     unsigned char outImage[SIZE][SIZE][RGB])
{


	for(int i=0; i<SIZE; i++){
	  for(int j=0; j<SIZE; j++){
		if(mask[i][j] == 1)		{ // if pixel is not chromakey then no change 

		  for(int color =0; color < 3; color++){
			outImage[i][j][color] = inImage[i][j][color];
		 }
		}
		else {  
								//replace chromakey with background 
		  for(int color=0; color<3; color++){
			outImage[i][j][color] = bgImage[i][j][color];
		 }
	}
}

}
	
}
