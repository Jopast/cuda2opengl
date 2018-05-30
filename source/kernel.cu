#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "defines.h"

#define GVC_CLIP3(L, H, v)  min((H), max((v), (L)))

__global__ void YUV2RGBConver(gpel_t *pYdata, gpel_t *pUdata, gpel_t *pVdata, uchar3 *OutData, int width, int height)
{
    const int x = blockIdx.x * blockDim.x + threadIdx.x;
    const int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x < width && y < height){
        int Y, U, V;
        int idx_y = (height - 1 - y) * width + x;
        int idx_uv = ((height >> 1) - 1 - (y >> 1)) * (width >> 1) + (x >> 1);
        int out_pos = y * width + x;
        Y = pYdata[idx_y];
        U = pUdata[idx_uv] - 128;
        V = pVdata[idx_uv] - 128;

        int R = Y + V + ((V * 103) >> 8);
        int G = Y - ((U * 88) >> 8) - ((V * 183) >> 8);
        int B = Y + U + ((U * 198) >> 8);

        OutData[out_pos].x = GVC_CLIP3(0, 255, R);//R
        OutData[out_pos].y = GVC_CLIP3(0, 255, G);//G
        OutData[out_pos].z = GVC_CLIP3(0, 255, B);//B
    }
}

extern "C"
int gvcd_yuv2rgb(gpel_t *pYdata, gpel_t *pUdata, gpel_t *pVdata, uchar3 *OutData,
    int width, int height)
{
    dim3 grids((width + 63) >> 6, (height + 15) >> 4);
    dim3 threads(64, 16);

    YUV2RGBConver << <grids, threads >> >(pYdata, pUdata, pVdata, OutData, width, height);

    return 0;
}