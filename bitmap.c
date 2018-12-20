#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"


/*
 * Read in the location of the pixel array, the image width, and the image 
 * height in the given bitmap file.
 */
void read_bitmap_metadata(FILE *image, int *pixel_array_offset, int *width, int *height) {
    fseek(image,10,SEEK_SET);
    if (fread(pixel_array_offset,sizeof(int),1,image) != 1){
        fprintf(stderr,"Error: cannot read the offset\n");
        exit(1);
    }

    fseek(image,18,SEEK_SET);
    if (fread(width,sizeof(int),1,image) != 1){
        fprintf(stderr,"Error: cannot read the width\n");
        exit(1);
    }

    fseek(image,22,SEEK_SET);
    if (fread(height,sizeof(int),1,image) != 1){
        fprintf(stderr,"Error: cannot read the height\n");
        exit(1);
    }
}

/*
 * Read in pixel array by following these instructions:
 *
 * 1. First, allocate space for m `struct pixel *` values, where m is the 
 *    height of the image.  Each pointer will eventually point to one row of 
 *    pixel data.
 * 2. For each pointer you just allocated, initialize it to point to 
 *    heap-allocated space for an entire row of pixel data.
 * 3. Use the given file and pixel_array_offset to initialize the actual 
 *    struct pixel values. Assume that `sizeof(struct pixel) == 3`, which is 
 *    consistent with the bitmap file format.
 *    NOTE: We've tested this assumption on the Teaching Lab machines, but 
 *    if you're trying to work on your own computer, we strongly recommend 
 *    checking this assumption!
 * 4. Return the address of the first `struct pixel *` you initialized.
 */
struct pixel **read_pixel_array(FILE *image, int pixel_array_offset, int width, int height) {
    struct pixel **pixels = malloc(sizeof(struct pixel*) * height);
    char blue,green,red;
    fseek(image,pixel_array_offset,SEEK_SET);
    for (int i = 0; i < height ; i ++){
        pixels[i] = malloc(sizeof(struct pixel) * width);
        for (int j = 0; j < width ; j ++){
            if (fread(&blue,1,1,image) != 1){
                fprintf(stderr,"Error: cannot read the blue pixel \n");
                exit(1);
            }
            if (fread(&green,1,1,image) != 1){
                fprintf(stderr,"Error: cannot read the green pixel \n");
                exit(1);
            }
            if (fread(&red,1,1,image) != 1){
                fprintf(stderr,"Error: cannot read the red pixel \n");
                exit(1);
            }
            pixels[i][j].blue = blue;
            pixels[i][j].green = green;
            pixels[i][j].red = red;  
        }
    }
    return pixels;
}


/*
 * Print the blue, green, and red colour values of a pixel.
 * You don't need to change this function.
 */
void print_pixel(struct pixel p) {
    printf("(%u, %u, %u)\n", p.blue, p.green, p.red);
}
