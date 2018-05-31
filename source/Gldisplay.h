#ifndef _GL_DISPLAY_H_
#define _GL_DISPLAY_H_

#include <windows.h>
#include <thread>
#include <mutex> 
#include <condition_variable>
#include <iostream>
#include <cuda_gl_interop.h>  
#include "GL\freeglut.h"  
#include "GL\glext.h"
#include "defines.h"  

using namespace std;

#define GET_PROC_ADDRESS(str) wglGetProcAddress(str)

#ifdef __cplusplus
extern "C" {    // only need to export C interface if used by C++ source code
#endif
    int gvcd_yuv2rgb(gpel_t *pYdata, gpel_t *pUdata, gpel_t *pVdata, uchar3 *OutData,
                     int width, int height, int stride_y, int stride_uv);
#ifdef __cplusplus
}
#endif

class GlDisplay
{
public:
    GlDisplay(int w, int h){
        Glinit(w, h);
    }
    ~GlDisplay();

    mutex mutex_lock;
    condition_variable_any m_t;     //条件变量
    bool wait_img;

    int get_size(){ return img_size; }
    int get_width(){ return width; }
    int get_height(){ return height; }
    void set_framerate(int value){ fps = value; }
    void start_display(int num_device);
    void set_img(gpel_t *Y, gpel_t *U, gpel_t *V, int stride_y, int stride_uv){
        img_buf[0] = Y;
        img_buf[1] = U;
        img_buf[2] = V;
        stride[0] = stride_y;
        stride[1] = stride_uv;
    }
private:
    PFNGLBINDBUFFERARBPROC    glBindBuffer;
    PFNGLDELETEBUFFERSARBPROC glDeleteBuffers;
    PFNGLGENBUFFERSARBPROC    glGenBuffers;
    PFNGLBUFFERDATAARBPROC    glBufferData;
    GLuint bufferObj;
    cudaGraphicsResource *resource;

    int width;
    int height;
    int stride[2];  //图像横向跨度
    int img_size;
    int fps;
    gpel_t *img_buf[3];

    static GlDisplay *cur_dis;//防止静态函数不能访问非静态成员
    void Glinit(int w, int h);
    static void drawFunc();
    static void keyFunc(unsigned char key, int x, int y);
    static void timeFunc(int value){
        cur_dis->drawFunc();
        // Timer: value ms
        glutTimerFunc(value, timeFunc, value);
    }
};

#endif
