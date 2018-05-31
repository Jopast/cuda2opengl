#include <stdio.h>
#include <device_launch_parameters.h>
#include <cuda_runtime.h>
#include "Gldisplay.h"

FILE *infile = NULL;

void get_img(void *args)
{
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
        lock_guard<mutex> locker(display->mutex_lock);
        while (!display->wait_img){                        //如果未获取到图像                
            display->m_t.wait(display->mutex_lock);        //将当前线程阻塞，注意：此时会释放锁
        }
        cudaMemcpy(g_buf, c_buf, img_size, cudaMemcpyHostToDevice);
        gpel_t *Y = g_buf;
        gpel_t *U = g_buf + width*height * sizeof(gpel_t);
        gpel_t *V = U + width*height * sizeof(gpel_t) / 4;
        display->set_img(Y, U, V, width, width >> 1);
        cout << "aaa" << endl;
        display->wait_img = false;          //等待显示后再读取图像
        display->m_t.notify_all();          //通知其余阻塞的使用者可以使用了

    }
    cudaFree(g_buf);
    free(c_buf);
    fclose(infile);
}

void show_img(void *args)
{
    GlDisplay *display = (GlDisplay *)args;
    display->set_framerate(20);
    display->start_display(0);
}

int main(int argc, char* argv[])
{
    if ((infile = fopen("F:/Seq/tmzLargeMotion5_1920x1080_23fps.yuv", "rb")) == NULL){
        printf("cannot open this file\n");
        return -1;
    }
    int width = 1920;
    int height = 1080;
    int img_size = width*height * sizeof(gpel_t) * 3 / 2;
    GlDisplay *display = new GlDisplay(width, height);
    thread t(get_img, display);
    t.detach();

    thread t1(show_img, display);
    t1.detach();
    //show_img(display);
    getchar();
    return 0;
}
