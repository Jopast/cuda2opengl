#ifndef _GL_DISPLAY_H_
#define _GL_DISPLAY_H_

#include <windows.h>
#include <cuda_gl_interop.h>  
#include "GL\freeglut.h"  
#include "GL\glext.h"
#include "defines.h"  

#define GET_PROC_ADDRESS(str) wglGetProcAddress(str)

#ifdef __cplusplus
extern "C" {    // only need to export C interface if used by C++ source code
#endif
int gvcd_yuv2rgb(gpel_t *pYdata, gpel_t *pUdata, gpel_t *pVdata, uchar3 *OutData,
    int width, int height);
#ifdef __cplusplus
}
#endif


class GlDisplay
{
public:
    GlDisplay(int *argc, char **argv, int w, int h){
        Glinit(argc, argv, w, h);
        cur_dis = this;
    }
    ~GlDisplay(){
    }
    void set_framerate(int value){ fps = value; }
    void start_display();
    void set_img(gpel_t *Y, gpel_t *U, gpel_t *V){
        img_buf[0] = Y;
        img_buf[1] = U;
        img_buf[2] = V;
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
    int img_size;
    int fps;
    gpel_t *img_buf[3];

    static GlDisplay *cur_dis;//防止静态函数不能访问非静态成员
    void Glinit(int *argc, char **argv, int w, int h);
    static void drawFunc();
    static void timeFunc(int value){
        cur_dis->drawFunc();
        // Timer: value ms
        glutTimerFunc(value, timeFunc, value);
    }
};

#endif
