#include "Gldisplay.h"
#include <device_launch_parameters.h>
#include <cuda_runtime.h>
#include <cuda.h>

GlDisplay* GlDisplay::cur_dis = NULL;//防止静态函数不能访问非静态成员

void GlDisplay::Glinit(int *argc, char **argv, int w, int h)
{
    width = w;
    height = h;
    img_size = w * h * 3;  //RGB

                           // step 2:  
                           //cudaDeviceProp prop;
                           //int dev;

                           //memset(&prop, 0, sizeof(cudaDeviceProp));
                           //prop.major = 1;
                           //prop.minor = 0;
                           //cudaChooseDevice(&dev, &prop);
    cudaGLSetGLDevice(0); //默认为device 0

    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(w, h);
    glutCreateWindow("display window");

    // step 3:  
    glBindBuffer = (PFNGLBINDBUFFERARBPROC)GET_PROC_ADDRESS("glBindBuffer");
    glDeleteBuffers = (PFNGLDELETEBUFFERSARBPROC)GET_PROC_ADDRESS("glDeleteBuffers");
    glGenBuffers = (PFNGLGENBUFFERSARBPROC)GET_PROC_ADDRESS("glGenBuffers");
    glBufferData = (PFNGLBUFFERDATAARBPROC)GET_PROC_ADDRESS("glBufferData");

    glGenBuffers(1, &bufferObj);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, bufferObj);
    glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, img_size, NULL, GL_DYNAMIC_DRAW_ARB);
}

void GlDisplay::drawFunc()
{
    glDrawPixels(cur_dis->width, cur_dis->height, GL_RGB, GL_UNSIGNED_BYTE, 0);
    uchar3* devPtr;
    size_t size;
    cudaGraphicsMapResources(1, &cur_dis->resource, NULL);
    cudaGraphicsResourceGetMappedPointer((void**)&devPtr, &size, cur_dis->resource);

    gvcd_yuv2rgb(cur_dis->img_buf[0], cur_dis->img_buf[1], cur_dis->img_buf[2], devPtr, cur_dis->width, cur_dis->height);

    cudaGraphicsUnmapResources(1, &cur_dis->resource, NULL);
    glutSwapBuffers();
}

void GlDisplay::start_display()
{
    // step 4:  
    cudaGraphicsGLRegisterBuffer(&resource, bufferObj, cudaGraphicsMapFlagsNone);
    glutDisplayFunc(drawFunc);
    timeFunc(fps);
    glutMainLoop();
}