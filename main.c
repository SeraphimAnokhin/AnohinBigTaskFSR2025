#include "stdio.h"
#include "stdlib.h"
#include "lodepng.h"


int main(int argc, char **argv) {
    unsigned int error;
    unsigned char* image;
    unsigned int width, height;
    const char* filename = argc > 1 ? argv[1] : "test.png";

    error = lodepng_decode32_file(&image, &width, &height, filename);

    if(error) printf("decoder error %u: %s\n", error, lodepng_error_text(error));

    int i, j, r, g, b, grey_image[height][width];

    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            r = image[(i * width + j) * 4];
            g = image[(i * width + j) * 4 + 1];
            b = image[(i * width + j) * 4 + 2];
            grey_image[i][j] = (int)(0.2126 * r + 0.7152 * g + 0.0722 * b);
        }
    }

    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            image[(i * width + j) * 4] = grey_image[i][j];
            image[(i * width + j) * 4 + 1] = grey_image[i][j];
            image[(i * width + j) * 4 + 2] = grey_image[i][j];
            image[(i * width + j) * 4 + 3] = 255;
        }
    }

    lodepng_encode32_file("out.png", image, width, height);

    free(image);
  return 0;
}