#include <stdio.h>
#include <device_launch_parameters.h>
#include <cuda_runtime.h>
#include "Gldisplay.h"

int main(int argc, char* argv[])
{
    FILE *infile = NULL;
    if ((infile = fopen("BasketballDrill_832x480_50.yuv", "rb")) == NULL){
        printf("cannot open this file\n");
        return -1;
    }

    int width = 832;
    int height = 480;
    int img_size = width*height * sizeof(gpel_t) * 3 / 2;
    gpel_t *c_buf, *g_buf;
    c_buf = (gpel_t *)malloc(img_size);
    cudaMalloc((void **)&g_buf, img_size);

    fread(c_buf, 1, img_size, infile);
    cudaMemcpy(g_buf, c_buf, img_size, cudaMemcpyHostToDevice);
    gpel_t *Y = g_buf;
    gpel_t *U = g_buf + width*height * sizeof(gpel_t);
    gpel_t *V = U + width*height * sizeof(gpel_t) / 4;

    GlDisplay *display = new GlDisplay(&argc, argv, width, height);

    display->set_img(Y, U, V);

    display->set_framerate(20);
    display->start_display();

    free(c_buf);
    cudaFree(g_buf);
    fclose(infile);
    return 0;
}
