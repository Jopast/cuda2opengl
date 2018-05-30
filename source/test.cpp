#include <iostream>
#include <thread>
#include <stdio.h>
#include <device_launch_parameters.h>
#include <cuda_runtime.h>
#include "Gldisplay.h"

using namespace std;

FILE *infile = NULL;

void test(void *args)
{
    cout << "fsdfsf" << endl;
    GlDisplay *display = (GlDisplay *)args;
    int img_size = display->get_size() >> 1;
    int width = display->get_width();
    int height = display->get_height();
    gpel_t *c_buf, *g_buf;
    c_buf = (gpel_t *)malloc(img_size);
    cudaMalloc((void **)&g_buf, img_size);
    while (true){
        if (fread(c_buf, 1, img_size, infile) != img_size){
            // Loop
            fseek(infile, 0, SEEK_SET);
            fread(c_buf, 1, img_size, infile);
        }
        cudaMemcpy(g_buf, c_buf, img_size, cudaMemcpyHostToDevice);
        gpel_t *Y = g_buf;
        gpel_t *U = g_buf + width*height * sizeof(gpel_t);
        gpel_t *V = U + width*height * sizeof(gpel_t) / 4;
        display->set_img(Y, U, V);
    }
    cudaFree(g_buf);
    free(c_buf);
}
int main(int argc, char* argv[])
{
    if ((infile = fopen("BasketballDrill_832x480_50.yuv", "rb")) == NULL){
        printf("cannot open this file\n");
        return -1;
    }

    int width = 832;
    int height = 480;
    int img_size = width*height * sizeof(gpel_t) * 3 / 2;
    GlDisplay *display = new GlDisplay(&argc, argv, width, height);

    thread t(test, display);
    t.detach();

    display->set_framerate(20);
    display->start_display();

    fclose(infile);
    return 0;
}
