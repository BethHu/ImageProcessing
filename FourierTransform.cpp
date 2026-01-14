// FourierTransform.cpp - minimal implementations so the project compiles
#include "stdafx.h"
#include "FourierTransform.h"
#include <complex>
#include <vector>
#include <cmath>
#include <limits>
#include <iostream>

using std::vector;

CFourierTransform::CFourierTransform()
{
}

CFourierTransform::~CFourierTransform()
{
}

// naive 1D DFT (forward when inverse==false, inverse otherwise)
void CFourierTransform::DFT1D(const vector<std::complex<double>>& input,
                              vector<std::complex<double>>& output,
                              int N, bool inverse)
{
    output.assign(N, std::complex<double>(0.0, 0.0));
    const double PI = 3.14159265358979323846;
    for (int k = 0; k < N; ++k)
    {
        std::complex<double> sum(0.0, 0.0);
        for (int n = 0; n < N; ++n)
        {
            double angle = 2.0 * PI * k * n / (double)N;
            if (!inverse)
                sum += input[n] * std::polar(1.0, -angle);
            else
                sum += input[n] * std::polar(1.0, angle);
        }
        if (inverse) sum /= (double)N;
        output[k] = sum;
    }
}

BOOL CFourierTransform::DFT2D(CImageDataset &imgIn, vector<std::complex<double>>& dftResult, int &width, int &height)
{
    if (imgIn.empty()) return FALSE;
    width = imgIn.m_xsize;
    height = imgIn.m_ysize;

    int W = width;
    int H = height;
    dftResult.assign(W * H, std::complex<double>(0.0, 0.0));

    // intermediate: DFT on rows
    vector<std::complex<double>> rowIn(W), rowOut(W);
    vector<std::complex<double>> temp(W * H);

    for (int y = 0; y < H; ++y)
    {
        for (int x = 0; x < W; ++x)
            rowIn[x] = std::complex<double>(imgIn.m_data[y * W + x], 0.0);

        DFT1D(rowIn, rowOut, W, false);

        for (int x = 0; x < W; ++x)
            temp[y * W + x] = rowOut[x];
    }

    // DFT on columns
    vector<std::complex<double>> colIn(H), colOut(H);
    for (int x = 0; x < W; ++x)
    {
        for (int y = 0; y < H; ++y)
            colIn[y] = temp[y * W + x];

        DFT1D(colIn, colOut, H, false);

        for (int y = 0; y < H; ++y)
            dftResult[y * W + x] = colOut[y];
    }

    return TRUE;
}

BOOL CFourierTransform::IDFT2D(vector<std::complex<double>>& dftResult, CImageDataset &imgOut, int width, int height)
{
    if (dftResult.empty()) return FALSE;
    int W = width;
    int H = height;
    if (!imgOut.create(W, H, 1, 0.0)) return FALSE;

    // inverse: columns then rows
    vector<std::complex<double>> colIn(H), colOut(H);
    vector<std::complex<double>> temp(W * H);

    for (int x = 0; x < W; ++x)
    {
        for (int y = 0; y < H; ++y)
            colIn[y] = dftResult[y * W + x];

        DFT1D(colIn, colOut, H, true);

        for (int y = 0; y < H; ++y)
            temp[y * W + x] = colOut[y];
    }

    vector<std::complex<double>> rowIn(W), rowOut(W);
    for (int y = 0; y < H; ++y)
    {
        for (int x = 0; x < W; ++x)
            rowIn[x] = temp[y * W + x];

        DFT1D(rowIn, rowOut, W, true);

        for (int x = 0; x < W; ++x)
        {
            double val = rowOut[x].real();
            if (val < 0.0) val = 0.0;
            if (val > 255.0) val = 255.0;
            imgOut.m_data[y * W + x] = val;
        }
    }

    return TRUE;
}

BOOL CFourierTransform::ComputeMagnitudeSpectrum(vector<std::complex<double>>& dftResult, 
                                                CImageDataset &spectrumOut, 
                                                int width, int height, 
                                                bool bNormalize)  // 添加可选参数
{
    if (dftResult.empty()) return FALSE;
    int W = width, H = height;
    if (!spectrumOut.create(W, H, 1, 0.0)) return FALSE;

    if (bNormalize)
    {
        // 归一化版本
        double maxMag = 0.0;
        for (int i = 0; i < W * H; ++i)
        {
            double m = std::abs(dftResult[i]);
            if (m > maxMag) maxMag = m;
        }
        if (maxMag <= 0.0) maxMag = 1.0;

        for (int i = 0; i < W * H; ++i)
        {
            double v = std::abs(dftResult[i]) / maxMag;
            spectrumOut.m_data[i] = v * 255.0;
        }
    }
    else
    {
        // 非归一化版本 - 直接使用幅度值
        // 找到最小值和最大值用于缩放
        double minMag = std::abs(dftResult[0]);
        double maxMag = minMag;
        
        for (int i = 1; i < W * H; ++i)
        {
            double m = std::abs(dftResult[i]);
            if (m < minMag) minMag = m;
            if (m > maxMag) maxMag = m;
        }
        
        double range = maxMag - minMag;
        if (range <= 0.0) range = 1.0;
        
        // 线性拉伸到 0-255
        for (int i = 0; i < W * H; ++i)
        {
            double m = std::abs(dftResult[i]);
            double v = (m - minMag) / range;
            spectrumOut.m_data[i] = v * 255.0;
        }
    }

    return TRUE;
}
BOOL CFourierTransform::ComputePhaseSpectrum(vector<std::complex<double>>& dftResult, 
                                            CImageDataset &phaseOut, 
                                            int width, int height,
                                            int nPhaseRange)  // 添加相位范围参数
{
    if (dftResult.empty()) return FALSE;
    int W = width, H = height;
    if (!phaseOut.create(W, H, 1, 0.0)) return FALSE;

    const double PI = 3.14159265358979323846;
    
    for (int i = 0; i < W * H; ++i)
    {
        double ph = std::arg(dftResult[i]); // -pi..pi
        
        switch (nPhaseRange)
        {
        case 0: // [-π, π] -> [0, 255]
            {
                double v = (ph + PI) / (2.0 * PI); // 0..1
                phaseOut.m_data[i] = v * 255.0;
            }
            break;
            
        case 1: // [0, 2π] -> [0, 255]
            {
                // 将 [-π, π] 转换为 [0, 2π]
                double ph2 = ph;
                if (ph2 < 0) ph2 += 2.0 * PI;
                double v = ph2 / (2.0 * PI); // 0..1
                phaseOut.m_data[i] = v * 255.0;
            }
            break;
            
        case 2: // [-180, 180] 度 -> [0, 255]
            {
                // 弧度转角度
                double degrees = ph * 180.0 / PI;
                // [-180, 180] -> [0, 1]
                double v = (degrees + 180.0) / 360.0;
                phaseOut.m_data[i] = v * 255.0;
            }
            break;
            
        default: // 默认使用 [-π, π]
            {
                double v = (ph + PI) / (2.0 * PI);
                phaseOut.m_data[i] = v * 255.0;
            }
            break;
        }
    }
    
    return TRUE;
}

BOOL CFourierTransform::ComputePowerSpectrum(vector<std::complex<double>>& dftResult, CImageDataset &powerOut, int width, int height)
{
    if (dftResult.empty()) return FALSE;
    int W = width, H = height;
    if (!powerOut.create(W, H, 1, 0.0)) return FALSE;

    double maxP = 0.0;
    for (int i = 0; i < W * H; ++i)
    {
        double p = std::norm(dftResult[i]);
        if (p > maxP) maxP = p;
    }
    if (maxP <= 0.0) maxP = 1.0;

    for (int i = 0; i < W * H; ++i)
    {
        double p = std::norm(dftResult[i]) / maxP;
        powerOut.m_data[i] = p * 255.0;
    }
    return TRUE;
}

BOOL CFourierTransform::ComputeAngularSpectrum(
    vector<std::complex<double>>& dftResult,
    CImageDataset &angularOut,
    int width,
    int height,
    int numBins,
    bool bNormalize)
{
    if (dftResult.empty() || numBins <= 0)
        return FALSE;

    // 输出为 1×numBins
    if (!angularOut.create(numBins, 1, 1, 0.0))
        return FALSE;

    const double PI = 3.14159265358979323846;
    const double TWO_PI = 2.0 * PI;

    int centerX = width / 2;
    int centerY = height / 2;

    vector<double> angularSum(numBins, 0.0);
    vector<int> angularCount(numBins, 0);

    // 遍历频谱
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int idx = y * width + x;

            int dx = x - centerX;
            int dy = centerY - y;  // 注意图像坐标系

            // 跳过直流分量
            if (dx == 0 && dy == 0)
                continue;

            double power = std::norm(dftResult[idx]);
            if (power <= 0.0)
                continue;

            // 方向角（0 ~ 2π）
            double angle = atan2((double)dy, (double)dx);
            if (angle < 0) angle += TWO_PI;

            int bin = (int)(angle * numBins / TWO_PI);
            if (bin >= numBins) bin = numBins - 1;

            angularSum[bin] += power;
            angularCount[bin]++;
        }
    }

    // 计算平均功率（对数压缩）
    double minVal = 1e100, maxVal = 0.0;
    for (int i = 0; i < numBins; ++i)
    {
        if (angularCount[i] > 0)
        {
            double avgPower = angularSum[i] / angularCount[i];
            double v = log10(1.0 + avgPower);
            angularOut.m_data[i] = v;

            minVal = min(minVal, v);
            maxVal = max(maxVal, v);
        }
        else
        {
            angularOut.m_data[i] = 0.0;
        }
    }

    // 可选归一化，仅用于显示
    if (bNormalize && maxVal > minVal)
    {
        for (int i = 0; i < numBins; ++i)
        {
            angularOut.m_data[i] =
                (angularOut.m_data[i] - minVal) * 255.0 / (maxVal - minVal);
        }
    }

    return TRUE;
}


BOOL CFourierTransform::ComputeRadialSpectrum(vector<std::complex<double>>& dftResult, 
                                             CImageDataset &radialOut, 
                                             int width, int height, 
                                             int numBins,
                                             bool bNormalize)
{
    try
    {
        TRACE(_T("[ComputeRadialSpectrum] 开始，尺寸=%dx%d, bins=%d, normalize=%d\n"), 
              width, height, numBins, bNormalize ? 1 : 0);
        
        if (dftResult.empty() || numBins <= 0)
            return FALSE;
        
        // 创建输出图像（1行，numBins列）
        if (!radialOut.create(numBins, 1, 1, 0.0))
            return FALSE;
        
        TRACE(_T("径向谱输出图像创建成功：%dx%d\n"), radialOut.m_xsize, radialOut.m_ysize);
        
        // 计算中心坐标和最大半径
        int centerX = width / 2;
        int centerY = height / 2;
        
        // 计算最大半径（图像中心到角落的距离）
        double maxRadius = sqrt((double)(centerX * centerX + centerY * centerY));
        if (maxRadius <= 0.0) maxRadius = 1.0;
        
        TRACE(_T("图像中心：(%d, %d)，最大半径：%f\n"), centerX, centerY, maxRadius);
        
        // 初始化数组
        vector<double> radialSum(numBins, 0.0);
        vector<int> radialCount(numBins, 0);
        
        // 遍历DFT结果
        double totalPower = 0.0;
        int validPixels = 0;
        
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int idx = y * width + x;
                if (idx >= (int)dftResult.size())
                    continue;
                
                // 使用功率（幅度的平方）
                double power = std::norm(dftResult[idx]);
                if (power <= 0.0) continue;
                
                // 计算相对中心的坐标
                int dx = x - centerX;
                int dy = y - centerY;
                
                // 跳过中心点（直流分量）
                if (dx == 0 && dy == 0) continue;
                
                // 计算半径
                double radius = sqrt((double)(dx * dx + dy * dy));
                
                // 确定bin索引
                int bin = (int)(radius * numBins / maxRadius);
                if (bin >= numBins) bin = numBins - 1;
                if (bin < 0) bin = 0;
                
                // 累加功率
                radialSum[bin] += power;
                radialCount[bin]++;
                totalPower += power;
                validPixels++;
            }
        }
        
        TRACE(_T("有效像素数：%d，总功率：%f\n"), validPixels, totalPower);
        
        // 计算平均值并存储到图像
        double maxRadialValue = 0.0;
        for (int i = 0; i < numBins; i++)
        {
            if (radialCount[i] > 0)
            {
                radialOut.m_data[i] = radialSum[i] / radialCount[i];
                if (radialOut.m_data[i] > maxRadialValue)
                    maxRadialValue = radialOut.m_data[i];
            }
            else
            {
                radialOut.m_data[i] = 0.0;
            }
        }
        
        TRACE(_T("径向谱原始最大值：%f\n"), maxRadialValue);
        
        // 归一化处理
        if (bNormalize && maxRadialValue > 0.0)
        {
            for (int i = 0; i < numBins; i++)
            {
                radialOut.m_data[i] = radialOut.m_data[i] / maxRadialValue * 255.0;
            }
            TRACE(_T("已归一化到[0,255]\n"));
        }
        
        TRACE(_T("径向谱计算完成\n"));
        
        // 输出前10个bin的值用于调试
        TRACE(_T("前10个bin的值："));
        for (int i = 0; i < min(10, numBins); i++)
        {
            TRACE(_T("%f "), radialOut.m_data[i]);
        }
        TRACE(_T("\n"));
        
        return TRUE;
    }
    catch (...)
    {
        TRACE(_T("ComputeRadialSpectrum: 发生异常\n"));
        return FALSE;
    }
}

BOOL CFourierTransform::ApplyLogTransform(CImageDataset &spectrumIn, CImageDataset &spectrumOut, double scale)
{
    if (spectrumIn.empty()) return FALSE;
    int W = spectrumIn.m_xsize;
    int H = spectrumIn.m_ysize;
    if (!spectrumOut.create(W, H, 1, 0.0)) return FALSE;

    for (int i = 0; i < W * H; ++i)
    {
        double v = spectrumIn.m_data[i];
        double t = log(1.0 + v * scale);
        spectrumOut.m_data[i] = t;
    }
    return TRUE;
}

void CFourierTransform::ShiftDFTCenter(vector<std::complex<double>>& dftResult, int width, int height)
{
    int W = width, H = height;
    int halfW = W / 2;
    int halfH = H / 2;
    for (int y = 0; y < halfH; ++y)
    {
        for (int x = 0; x < halfW; ++x)
        {
            int a = y * W + x;
            int b = (y + halfH) * W + (x + halfW);
            std::swap(dftResult[a], dftResult[b]);

            int c = y * W + (x + halfW);
            int d = (y + halfH) * W + x;
            std::swap(dftResult[c], dftResult[d]);
        }
    }
}

BOOL CFourierTransform::ComputeError(CImageDataset &img1, CImageDataset &img2, double &mse, double &psnr, double &maxError, CImageDataset &errorImage)
{
    if (img1.empty() || img2.empty()) return FALSE;
    if (img1.m_xsize != img2.m_xsize || img1.m_ysize != img2.m_ysize) return FALSE;
    int W = img1.m_xsize, H = img1.m_ysize;
    mse = 0.0; maxError = 0.0;
    if (!errorImage.create(W, H, 1, 0.0)) return FALSE;
    for (int i = 0; i < W * H; ++i)
    {
        double diff = img1.m_data[i] - img2.m_data[i];
        double e = diff * diff;
        if (fabs(diff) > maxError) maxError = fabs(diff);
        mse += e;
        errorImage.m_data[i] = fabs(diff);
    }
    mse /= (double)(W * H);
    if (mse <= 0.0) psnr = std::numeric_limits<double>::infinity(); else psnr = 10.0 * log10(255.0 * 255.0 / mse);
    return TRUE;
}

// 在 FourierTransform.cpp 末尾添加这两个函数的实现

/////////////////////////////////////////////////////////////////////////////
// 计算逆DFT（重建图像）
/////////////////////////////////////////////////////////////////////////////
BOOL CFourierTransform::ComputeInverseDFT(vector<std::complex<double>>& dftResult,
                                         CImageDataset& imgOut,
                                         int width,
                                         int height,
                                         bool bNeedToUncenter)  // FALSE表示数据已是非中心化
{
    TRACE(_T("[CFourierTransform::ComputeInverseDFT] 开始，尺寸=%dx%d\n"), width, height);
    
    // 1. 参数检查
    if (dftResult.empty())
    {
        TRACE(_T("错误：DFT结果为空\n"));
        return FALSE;
    }
    
    int totalPixels = width * height;
    if ((int)dftResult.size() < totalPixels)
    {
        TRACE(_T("错误：DFT数据不足\n"));
        return FALSE;
    }
    
    // 2. 如果输入是中心化的，需要先反中心化
    vector<std::complex<double>> dftData = dftResult;
    if (bNeedToUncenter)
    {
        TRACE(_T("注意：输入数据是中心化的，执行反中心化\n"));
        ShiftDFTCenter(dftData, width, height);  // 对中心化数据执行fftshift就是反中心化
    }
    
    // 3. 直接调用现有的IDFT2D函数
    return IDFT2D(dftData, imgOut, width, height);
}

/////////////////////////////////////////////////////////////////////////////
// 计算误差分析
/////////////////////////////////////////////////////////////////////////////
BOOL CFourierTransform::ComputeErrorAnalysis(CImageDataset& imgOriginal,
                                            CImageDataset& imgReconstructed,
                                            double& mse,
                                            double& psnr,
                                            double& maxError,
                                            CImageDataset& errorImage)
{
    try
    {
        TRACE(_T("[CFourierTransform::ComputeErrorAnalysis] 开始\n"));
        
        // 1. 参数检查
        if (imgOriginal.empty() || imgReconstructed.empty())
        {
            TRACE(_T("错误：输入图像为空\n"));
            return FALSE;
        }
        
        int w1 = imgOriginal.m_xsize;
        int h1 = imgOriginal.m_ysize;
        int w2 = imgReconstructed.m_xsize;
        int h2 = imgReconstructed.m_ysize;
        
        if (w1 != w2 || h1 != h2)
        {
            TRACE(_T("错误：图像尺寸不匹配：原始=%dx%d，重建=%dx%d\n"),
                  w1, h1, w2, h2);
            return FALSE;
        }
        
        // 2. 计算误差指标
        int totalPixels = w1 * h1;
        double sumSquaredError = 0.0;
        maxError = 0.0;
        
        // 确保错误图像已创建
        if (!errorImage.create(w1, h1, 1, 0.0))
        {
            TRACE(_T("错误：无法创建误差图像\n"));
            return FALSE;
        }
        
        // 3. 逐像素计算误差
        for (int y = 0; y < h1; y++)
        {
            for (int x = 0; x < w1; x++)
            {
                int idx = y * w1 + x;
                
                // 获取像素值（假设单通道）
                double originalVal = imgOriginal.m_data[idx];
                double reconstructedVal = imgReconstructed.m_data[idx];
                
                // 计算绝对误差
                double absError = fabs(originalVal - reconstructedVal);
                
                // 更新最大误差
                if (absError > maxError)
                {
                    maxError = absError;
                }
                
                // 计算平方误差
                double squaredError = (originalVal - reconstructedVal) * 
                                     (originalVal - reconstructedVal);
                sumSquaredError += squaredError;
                
                // 设置误差图像像素值（限制在0-255）
                double errorPixel = absError;
                if (errorPixel > 255.0) errorPixel = 255.0;
                errorImage.m_data[idx] = errorPixel;
            }
        }
        
        // 4. 计算MSE和PSNR
        mse = sumSquaredError / totalPixels;
        
        if (mse > 0.0)
        {
            double maxVal = 255.0;  // 8位图像的最大值
            psnr = 10.0 * log10((maxVal * maxVal) / mse);
        }
        else
        {
            psnr = std::numeric_limits<double>::infinity();
        }
        
        TRACE(_T("[CFourierTransform::ComputeErrorAnalysis] 完成\n"));
        TRACE(_T("  图像尺寸: %dx%d\n"), w1, h1);
        TRACE(_T("  MSE: %f, PSNR: %f dB, MaxError: %f\n"), mse, psnr, maxError);
        
        return TRUE;
    }
    catch (const std::exception& e)
    {
        TRACE(_T("[CFourierTransform::ComputeErrorAnalysis] 异常: %s\n"), 
              CString(e.what()));
        return FALSE;
    }
    catch (...)
    {
        TRACE(_T("[CFourierTransform::ComputeErrorAnalysis] 未知异常\n"));
        return FALSE;
    }
}