#pragma once
#include"myIntergal.h"
#include"basic.h"
#include"MyAutoBuffer.h"
#include"basicStruct.h"
/****************************************************************************************\
*                                计算积分图                                              *
\****************************************************************************************/
template<typename T, typename ST, typename QT>
void integral_(const T* src, size_t _srcstep, ST* sum, size_t _sumstep,
	QT* sqsum, size_t _sqsumstep, ST* tilted, size_t _tiltedstep,
	MySize size, int cn)
	//(const MyMat* src, size_t _srcstep, MyMat* sum, size_t _sumstep,
	//	MyMat* sqsum, size_t _sqsumstep, MyMat* tilted, size_t _tiltedstep,
	//	MySize size, int cn)
{
	int x, y, k;

	int srcstep = (int)(_srcstep / sizeof(T));
	int sumstep = (int)(_sumstep / sizeof(ST));
	int tiltedstep = (int)(_tiltedstep / sizeof(ST));
	int sqsumstep = (int)(_sqsumstep / sizeof(QT));

	size.width *= cn;

	memset(sum, 0, (size.width + cn) * sizeof(sum[0]));
	sum += sumstep + cn;

	if (sqsum)
	{
		memset(sqsum, 0, (size.width + cn) * sizeof(sqsum[0]));
		sqsum += sqsumstep + cn;
	}

	if (tilted)
	{
		memset(tilted, 0, (size.width + cn) * sizeof(tilted[0]));
		tilted += tiltedstep + cn;
	}

	if (sqsum == 0 && tilted == 0)
	{
		for (y = 0; y < size.height; y++, src += srcstep - cn, sum += sumstep - cn)
		{
			for (k = 0; k < cn; k++, src++, sum++)
			{
				ST s = sum[-cn] = 0;
				for (x = 0; x < size.width; x += cn)
				{
					s += src[x];
					sum[x] = sum[x - sumstep] + s;
				}
			}
		}
	}
	else if (tilted == 0)
	{
		for (y = 0; y < size.height; y++, src += srcstep - cn,
			sum += sumstep - cn, sqsum += sqsumstep - cn)
		{
			for (k = 0; k < cn; k++, src++, sum++, sqsum++)
			{
				ST s = sum[-cn] = 0;
				QT sq = sqsum[-cn] = 0;
				for (x = 0; x < size.width; x += cn)
				{
					T it = src[x];
					s += it;
					sq += (QT)it*it;
					ST t = sum[x - sumstep] + s;
					QT tq = sqsum[x - sqsumstep] + sq;
					sum[x] = t;
					sqsum[x] = tq;
				}
			}
		}
	}
	else
	{
		MyAutoBuffer<ST> _buf(size.width + cn);
		ST* buf = _buf;
		ST s;
		QT sq;
		for (k = 0; k < cn; k++, src++, sum++, tilted++, buf++)
		{
			sum[-cn] = tilted[-cn] = 0;

			for (x = 0, s = 0, sq = 0; x < size.width; x += cn)
			{
				T it = src[x];
				buf[x] = tilted[x] = it;
				s += it;
				sq += (QT)it*it;
				sum[x] = s;
				if (sqsum)
					sqsum[x] = sq;
			}

			if (size.width == cn)
				buf[cn] = 0;

			if (sqsum)
			{
				sqsum[-cn] = 0;
				sqsum++;
			}
		}

		for (y = 1; y < size.height; y++)
		{
			src += srcstep - cn;
			sum += sumstep - cn;
			tilted += tiltedstep - cn;
			buf += -cn;

			if (sqsum)
				sqsum += sqsumstep - cn;

			for (k = 0; k < cn; k++, src++, sum++, tilted++, buf++)
			{
				T it = src[0];
				ST t0 = s = it;
				QT tq0 = sq = (QT)it*it;

				sum[-cn] = 0;
				if (sqsum)
					sqsum[-cn] = 0;
				tilted[-cn] = tilted[-tiltedstep];

				sum[0] = sum[-sumstep] + t0;
				if (sqsum)
					sqsum[0] = sqsum[-sqsumstep] + tq0;
				tilted[0] = tilted[-tiltedstep] + t0 + buf[cn];

				for (x = cn; x < size.width - cn; x += cn)
				{
					ST t1 = buf[x];
					buf[x - cn] = t1 + t0;
					t0 = it = src[x];
					tq0 = (QT)it*it;
					s += t0;
					sq += tq0;
					sum[x] = sum[x - sumstep] + s;
					if (sqsum)
						sqsum[x] = sqsum[x - sqsumstep] + sq;
					t1 += buf[x + cn] + t0 + tilted[x - tiltedstep - cn];
					tilted[x] = t1;
				}

				if (size.width > cn)
				{
					ST t1 = buf[x];
					buf[x - cn] = t1 + t0;
					t0 = it = src[x];
					tq0 = (QT)it*it;
					s += t0;
					sq += tq0;
					sum[x] = sum[x - sumstep] + s;
					if (sqsum)
						sqsum[x] = sqsum[x - sqsumstep] + sq;
					tilted[x] = t0 + t1 + tilted[x - tiltedstep - cn];
					buf[x] = t0;
				}

				if (sqsum)
					sqsum++;
			}
		}
	}
}

void myIntegral(MyMat *src, MyMat *sum, MyMat *sqsum, MyMat *tilted)
{
	MySize mysize;
	mysize = mySize(src->width, src->height);
	integral_(src->data.ptr, src->step, sum->data.i, sum->step, sqsum->data.db, sqsum->step, tilted->data.i, tilted->step, mysize, src->channel_type);
}
/*
*求解积分图
*/
void GetGrayIntegralImage(unsigned char *Src, int *Integral, int Width, int Height, int Stride)
{
	int *ColSum = (int *)calloc(Width, sizeof(int));        //    用的calloc函数哦，自动内存清0
	memset(Integral, 0, (Width + 1) * sizeof(int));
	for (int Y = 0; Y < Height; Y++)
	{
		unsigned char *LinePS = Src + Y * Stride;
		int *LinePL = Integral + Y * (Width + 1) + 1;
		int *LinePD = Integral + (Y + 1) * (Width + 1) + 1;
		LinePD[-1] = 0;
		for (int X = 0; X < Width; X++)
		{
			ColSum[X] += LinePS[X];
			LinePD[X] = LinePD[X - 1] + ColSum[X];
		}
	}
	free(ColSum);
}
//求解平方积分图
void GetGraySqImage(unsigned char *Src, int *Integral, int Width, int Height, int Stride)
{
	uchar *copy = new uchar[Width*Height];
	for (int i = 0;i < Width;i++)
	{
		for (int j = 0;j < Height;j++)
		{
			copy[j + i * Width] = Src[j + i * Width] * Src[j + i * Width];
		}
	}
	GetGrayIntegralImage(copy,Integral,Width,Height,Stride);
	delete []copy;
}
/*
*冒泡排序
*idx 索引数组
*data_array 排序数组
*/
void bubbleSort(int* pData, int *idx, int length)
{
	int temp, idxt;
	for (int i = 0;i != length;++i)
	{
		for (int j = 0; j != length; ++j)
		{
			if (pData[i] < pData[j])
			{
				temp = pData[i];
				idxt = idx[i];
				pData[i] = pData[j];
				idx[i] = idx[j];
				pData[j] = temp;
				idx[j] = idxt;
			}
		}
	}
}