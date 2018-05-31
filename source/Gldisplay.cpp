#include "Gldisplay.h"
#include <device_launch_parameters.h>
#include <cuda_runtime.h>
#include <cuda.h>

GlDisplay* GlDisplay::cur_dis = NULL;//��ֹ��̬�������ܷ��ʷǾ�̬��Ա

GlDisplay::GlDisplay(int w, int h)
{
    width = w;
    height = h;
    img_size = w * h * 3;  //RGB

    wait_img = true; //��ʼ��������ͼ����Ҫ�ȴ�
    cur_dis = this;
}

GlDisplay::~GlDisplay(){
    cudaGraphicsUnregisterResource(resource);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
    glDeleteBuffers(1, &bufferObj);
}

void GlDisplay::drawFunc()
{
    glDrawPixels(cur_dis->width, cur_dis->height, GL_RGB, GL_UNSIGNED_BYTE, 0);
    lock_guard<mutex> locker(cur_dis->mutex_lock);
    while (cur_dis->wait_img){                         //�����ȡ��ͼ��                
        cur_dis->m_t.wait(cur_dis->mutex_lock);        //����ǰ�߳�������ע�⣺��ʱ���ͷ���
    }
    
    gpel3_t* devPtr;
    size_t size;
    cudaGraphicsMapResources(1, &cur_dis->resource, NULL);
    cudaGraphicsResourceGetMappedPointer((void**)&devPtr, &size, cur_dis->resource);

    gvcd_yuv2rgb(cur_dis->img_buf[0], cur_dis->img_buf[1], cur_dis->img_buf[2], devPtr,
                 cur_dis->width, cur_dis->height, cur_dis->stride[0], cur_dis->stride[1]);

    cudaGraphicsUnmapResources(1, &cur_dis->resource, NULL);
    glutSwapBuffers();
    cout << "bbb" << endl;
    cur_dis->wait_img = true;           //�ȴ���һ֡ͼ��
    cur_dis->m_t.notify_all();          //֪ͨ����������ʹ���߿���ʹ����
}

void GlDisplay::keyFunc(unsigned char key, int x, int y)
{
    switch (key){
    case 27:
        cudaGraphicsUnregisterResource(cur_dis->resource);
        cur_dis->glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
        cur_dis->glDeleteBuffers(1, &cur_dis->bufferObj);
        exit(0);
    }
}

void GlDisplay::start_display(int num_device)
{
    int argc = 1;
    char *argv[1];
    argv[0] = "display";
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(width, height);
    glutCreateWindow("display window");

    cudaGLSetGLDevice(num_device); //Ĭ��Ϊdevice 0

    // step 3:  
    glBindBuffer = (PFNGLBINDBUFFERARBPROC)GET_PROC_ADDRESS("glBindBuffer");
    glDeleteBuffers = (PFNGLDELETEBUFFERSARBPROC)GET_PROC_ADDRESS("glDeleteBuffers");
    glGenBuffers = (PFNGLGENBUFFERSARBPROC)GET_PROC_ADDRESS("glGenBuffers");
    glBufferData = (PFNGLBUFFERDATAARBPROC)GET_PROC_ADDRESS("glBufferData");

    glGenBuffers(1, &bufferObj);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, bufferObj);
    glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, img_size, NULL, GL_DYNAMIC_DRAW_ARB);

    // step 4:  
    cudaGraphicsGLRegisterBuffer(&resource, bufferObj, cudaGraphicsMapFlagsNone);
    //glutKeyboardFunc(keyFunc);
    glutDisplayFunc(drawFunc);
    timeFunc(fps);
    glutMainLoop();
}