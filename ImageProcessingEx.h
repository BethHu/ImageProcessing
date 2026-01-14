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

    // 直方图均衡化与匹配（保持原有实现与接口）
    static BOOL histeq(CImageDataset &imgIn, CImageDataset &imgOut);
    static BOOL histMatch(CImageDataset &imgSrc, CImageDataset &imgRef, CImageDataset &imgOut);

    // 空间域滤波（保持原有实现与接口）
    static BOOL medianFilter(CImageDataset &imgIn, CImageDataset &imgOut, int windowSize);
    static BOOL bilateralFilter(CImageDataset &imgIn, CImageDataset &imgOut,
                                double sigmaS, double sigmaR, int windowSize);
    static BOOL laplacianSharpen(CImageDataset &imgIn, CImageDataset &imgOut);

    // 几何/测试图像生成
    static BOOL createRectangleImage(CImageDataset &imgOut,
                                    int width, int height,
                                    int rectX, int rectY,
                                    int rectWidth, int rectHeight,
                                    BYTE backgroundColor = 0,
                                    BYTE rectangleColor = 255);

    static BOOL createSineImage(CImageDataset &imgOut,
                                int width, int height,
                                double frequency,
                                double phase = 0.0);

    static BOOL createCheckerboardImage(CImageDataset &imgOut,
                                       int width, int height,
                                       int blockSize);

    // 傅里叶变换相关函数（保持原有接口）
    static BOOL DFT2D(CImageDataset &imgIn,
                      std::vector<std::complex<double>> &dftResult,
                      int &width, int &height);

    static BOOL IDFT2D(std::vector<std::complex<double>> &dftResult,
                       CImageDataset &imgOut,
                       int width, int height);

    // 频谱计算
    static BOOL ComputeMagnitudeSpectrum(std::vector<std::complex<double>> &dftResult,
                                        CImageDataset &spectrumImage,
                                        int width, int height);

    static BOOL ComputePhaseSpectrum(std::vector<std::complex<double>> &dftResult,
                                     CImageDataset &spectrumImage,
                                     int width, int height);

    static BOOL ComputePowerSpectrum(std::vector<std::complex<double>> &dftResult,
                                     CImageDataset &spectrumImage,
                                     int width, int height);

    // 角谱和径向谱
    static BOOL ComputeAngularSpectrum(std::vector<std::complex<double>> &dftResult,
                                      CImageDataset &angularImage,
                                      int width, int height,
                                      int numBins = 360);

    static BOOL ComputeRadialSpectrum(std::vector<std::complex<double>> &dftResult,
                                     CImageDataset &radialImage,
                                     int width, int height,
                                     int numBins = 100);

    // 显示与归一化
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

    // ===== 新增：PCA / RGB-IHS / Canny 算法（从 myAlg_2 引入，保持独立） =====

    // PCA 变换
    static BOOL pcaForward(CImageDataset &imgIn, CImageDataset &imgPCA,
                           CMatrix &matT, CMatrix &matEigValues, CMatrix &matMean);
    static BOOL pcaBackward(CImageDataset &imgPCA, CMatrix &matT, CMatrix &matMean,
                            int nCustomP, CImageDataset &imgRecon);
    static double calcPcaRmse(CImageDataset &imgOrig, CImageDataset &imgRecon);

    // RGB-IHS 互变换与线性拉伸
    static BOOL rgb2ihs(CImageDataset &imgIn, CImageDataset &imgIHS);
    static BOOL ihs2rgb(CImageDataset &imgIHS, CImageDataset &imgOut);
    static BOOL linearStretch(CImageDataset &imgIn, CImageDataset &imgOut);

    // Canny 边缘检测
    static BOOL cannyEdgeDetection(CImageDataset &imgIn, CImageDataset &imgOut,
                                   double sigma, double lowThresh, double highThresh);
    static void getAutoCannyThresholds(CImageDataset &imgIn,
                                      double &outLow, double &outHigh);
};