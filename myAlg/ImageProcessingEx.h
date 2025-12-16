#pragma once
#include "rsdiplib.h"
#include <vector>
#include <complex>


class CImageProcessingEx :
	public CImageProcessing
{
public:
	CImageProcessingEx(void);
	~CImageProcessingEx(void);

	//直方图均衡化函数申明
	static BOOL histeq(CImageDataset &imgIn,CImageDataset &imgOut);
	static BOOL histMatch(CImageDataset &imgSrc, CImageDataset &imgRef, CImageDataset &imgOut);
	static BOOL medianFilter(CImageDataset &imgIn, CImageDataset &imgOut, int windowSize);
	static BOOL bilateralFilter(CImageDataset &imgIn, CImageDataset &imgOut, 
                                double sigmaS, double sigmaR, int windowSize);
	static BOOL laplacianSharpen(CImageDataset &imgIn, CImageDataset &imgOut);
	// 矩形图像生成函数
	static BOOL createRectangleImage(CImageDataset &imgOut,
									int width, int height,
									int rectX, int rectY,
									int rectWidth, int rectHeight,
									BYTE backgroundColor = 0,
									BYTE rectangleColor = 255);
                                
	// 正弦波图像生成函数
	static BOOL createSineImage(CImageDataset &imgOut,
							   int width, int height,
							   double frequency,
							   double phase = 0.0);
                           
	// 棋盘格图像生成函数
	static BOOL createCheckerboardImage(CImageDataset &imgOut,
									   int width, int height,
									   int blockSize);
    
    // 傅里叶变换相关函数
    static BOOL DFT2D(CImageDataset &imgIn, 
                     std::vector<std::complex<double>> &dftResult,
                     int &width, int &height);
    
    static BOOL IDFT2D(std::vector<std::complex<double>> &dftResult,
                      CImageDataset &imgOut,
                      int width, int height);

    // 谱计算函数
    static BOOL ComputeMagnitudeSpectrum(std::vector<std::complex<double>> &dftResult,
                                        CImageDataset &spectrumImage,  // 注意：参数类型改为 CImageDataset
                                        int width, int height);
    
    static BOOL ComputePhaseSpectrum(std::vector<std::complex<double>> &dftResult,
                                    CImageDataset &spectrumImage,  // 注意：参数类型改为 CImageDataset
                                    int width, int height);
    
    static BOOL ComputePowerSpectrum(std::vector<std::complex<double>> &dftResult,
                                    CImageDataset &spectrumImage,  // 注意：参数类型改为 CImageDataset
                                    int width, int height);
    
    // 角谱和径向谱计算
    static BOOL ComputeAngularSpectrum(std::vector<std::complex<double>> &dftResult,
                                      CImageDataset &angularImage,
                                      int width, int height,
                                      int numBins = 360);
    
    static BOOL ComputeRadialSpectrum(std::vector<std::complex<double>> &dftResult,
                                     CImageDataset &radialImage,
                                     int width, int height,
                                     int numBins = 100);
    
    // 显示处理函数
    static BOOL ApplyLogTransform(CImageDataset &image,
                                 double base = 10.0,
                                 double offset = 1.0);
    
    static BOOL NormalizeToRange(CImageDataset &image,
                                double minVal, double maxVal);
    
    // 辅助函数
    static BOOL ConvertTo8Bit(const std::vector<double> &spectrum,
                             std::vector<BYTE> &output,
                             int width, int height);
    
    static void DisplayImageData(std::vector<BYTE> &data, 
                                int width, int height,
                                CWnd* pWnd, CRect rect);
};