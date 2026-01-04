#pragma once
#include "rsdiplib.h"

class CImageProcessingEx :
	public CImageProcessing
{
public:
	CImageProcessingEx(void);
	~CImageProcessingEx(void);


	//ֱ直方图均衡化
	static BOOL histeq(CImageDataset &imgIn,CImageDataset &imgOut);
	//ֱ直方图匹配
	static BOOL histmatch(CImageDataset &imgIn, CImageDataset &imgRef, CImageDataset &imgOut);
	//  拉普拉斯锐化
	static BOOL laplacianSharpen(CImageDataset &imgIn, CImageDataset &imgOut);
	//  中值滤波
    static BOOL medianFilter(CImageDataset &imgIn, CImageDataset &imgOut, int winSize = 3);
	//  双边滤波
    static BOOL bilateralFilter(CImageDataset &imgIn, CImageDataset &imgOut, 
                                int d, double sigmaS, double sigmaR);

	// PCA变换
      // 1. PCA正变换
     static BOOL pcaForward(CImageDataset &imgIn, CImageDataset &imgPCA, CMatrix &matT, CMatrix &matEigValues, CMatrix &matMean);
    // 2. PCA反变换
    static BOOL pcaBackward(CImageDataset &imgPCA, CMatrix &matT, CMatrix &matMean, int nCustomP, CImageDataset &imgRecon);
    // 计算MRSE
    static double calcPcaRmse(CImageDataset &imgOrig, CImageDataset &imgRecon);

    // RGB-IHS 正变换（RGB -> IHS）
    static BOOL rgb2ihs(CImageDataset &imgIn, CImageDataset &imgIHS);
    // RGB-IHS 反变换（IHS -> RGB）
    static BOOL ihs2rgb(CImageDataset &imgIHS, CImageDataset &imgOut);
    // 线性拉伸（用于显示I/H/S分量）
    static BOOL linearStretch(CImageDataset &imgIn, CImageDataset &imgOut);

    // Canny Edge Detection
    static BOOL cannyEdgeDetection(CImageDataset &imgIn, CImageDataset &imgOut, double sigma, double lowThresh, double highThresh);

    // 自动计算推荐阈值
    static void getAutoCannyThresholds(CImageDataset &imgIn, double &outLow, double &outHigh);
};

    
