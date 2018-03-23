#pragma once
#include"MyMat.h"
void myIntegral(MyMat *src, MyMat *sum, MyMat *sqsum, MyMat *tilted);
void GetGrayIntegralImage(unsigned char *Src, int *Integral, int Width, int Height, int Stride); //求解积分图
void GetGraySqImage(unsigned char *Src, int *Integral, int Width, int Height, int Stride); //求解平方积分图
void bubbleSort(int* pData, int *idx, int length);