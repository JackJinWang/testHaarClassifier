#pragma once
#include"MyMat.h"
void myIntegral(MyMat *src, MyMat *sum, MyMat *sqsum, MyMat *tilted);
void GetGrayIntegralImage(unsigned char *Src, int *Integral, int Width, int Height, int Stride); //������ͼ
void GetGraySqImage(unsigned char *Src, int *Integral, int Width, int Height, int Stride); //���ƽ������ͼ
void bubbleSort(int* pData, int *idx, int length);