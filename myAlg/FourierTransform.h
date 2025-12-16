#pragma once
#include "RSDIPLib.h"
#include <complex>
#include <vector>

class CFourierTransform
{
public:
    // 构造函数和析构函数
    CFourierTransform();
    ~CFourierTransform();
    
    // 二维离散傅里叶变换
    static BOOL DFT2D(CImageDataset &imgIn, 
                     std::vector<std::complex<double>>& dftResult,
                     int &width, int &height);
    
    // 二维离散傅里叶反变换
    static BOOL IDFT2D(std::vector<std::complex<double>>& dftResult,
                      CImageDataset &imgOut,
                      int width, int height);
    
    // 计算幅度谱
   static BOOL ComputeMagnitudeSpectrum(std::vector<std::complex<double>>& dftResult, 
                                        CImageDataset &spectrumOut, 
                                        int width, int height,
                                        bool bNormalize = true); 
    
    // 计算相位谱
	static BOOL ComputePhaseSpectrum(std::vector<std::complex<double>>& dftResult, 
										CImageDataset &phaseOut, 
										int width, int height,
										int nPhaseRange = 0); 
    
    // 计算能量谱（功率谱）
    static BOOL ComputePowerSpectrum(std::vector<std::complex<double>>& dftResult,
                                    CImageDataset &powerOut,
                                    int width, int height);
    
    // 计算角谱
    static BOOL ComputeAngularSpectrum(std::vector<std::complex<double>>& dftResult, 
                                  CImageDataset &angularOut, 
                                  int width, int height, 
                                  int numBins = 360,
                                  bool bNormalize = true);  // 添加归一化选项

	static BOOL ComputeRadialSpectrum(std::vector<std::complex<double>>& dftResult, 
									 CImageDataset &radialOut, 
									 int width, int height, 
									 int numBins = 100,
									 bool bNormalize = true); 
    
    // 应用对数变换
    static BOOL ApplyLogTransform(CImageDataset &spectrumIn,
                                 CImageDataset &spectrumOut,
                                 double scale = 1.0);
    
    // 频域中心化
    static void ShiftDFTCenter(std::vector<std::complex<double>>& dftResult,
                              int width, int height);
    
    // 计算两个图像之间的误差
    static BOOL ComputeError(CImageDataset &img1,
                            CImageDataset &img2,
                            double &mse,
                            double &psnr,
                            double &maxError,
                            CImageDataset &errorImage);
	BOOL ComputeInverseDFT(std::vector<std::complex<double>>& dftResult, 
                          CImageDataset& imgOut, 
                          int width, 
                          int height,
                          bool bNeedToUncenter = false);  // 是否已中心化
    
    BOOL ComputeErrorAnalysis(CImageDataset& imgOriginal,
                             CImageDataset& imgReconstructed,
                             double& mse,
                             double& psnr,
                             double& maxError,
                             CImageDataset& errorImage);
	
    
    // 谱类型枚举
    enum SpectrumType {
        SPECTRUM_MAGNITUDE = 0,   // 幅度谱
        SPECTRUM_PHASE = 1,       // 相位谱
        SPECTRUM_POWER = 2,       // 能量谱/功率谱
        SPECTRUM_LOG = 3,         // 对数幅度谱
        SPECTRUM_ANGULAR = 4,     // 角谱
        SPECTRUM_RADIAL = 5       // 径向谱
    };
    
private:
    // 一维离散傅里叶变换（基础实现）
    static void DFT1D(const std::vector<std::complex<double>>& input,
                     std::vector<std::complex<double>>& output,
                     int N, bool inverse);
};