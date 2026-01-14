#include "StdAfx.h"
#include "ImageProcessingEx.h"
#include "FourierTransform.h" 
#include <cmath>
#include <algorithm>  // 使用 std::sort
#include <vector> 


CImageProcessingEx::CImageProcessingEx(void)
{
}


CImageProcessingEx::~CImageProcessingEx(void)
{
}

BOOL CImageProcessingEx::histeq(CImageDataset &imgIn,CImageDataset &imgOut)
{
	const int LEVEL=256;
	int k,row,col;
	double hist[LEVEL],sk[LEVEL];
	if (imgIn.empty())
	{
		return FALSE;
	}
	if(FALSE==imgIn.duplicate(imgOut))
	{
		return FALSE;
	}
	double *data=imgOut.m_data;
	for(k=0;k<LEVEL;k++)
	{
		hist[k]=0;
	}
	for(row=0;row<imgIn.m_ysize;row++)
	{
		for(col=0;col<imgIn.m_xsize;col++)
		{
			hist[UINT8(data[row*imgIn.m_xsize+col])]++;

		}
	}

	sk[0]=hist[0]/(imgIn.m_ysize*imgIn.m_xsize);
	for(k=1;k<LEVEL-1;k++)
	{
		sk[k]=sk[k-1]+hist[k]/(imgIn.m_ysize*imgIn.m_xsize);
	}
	sk[255]=1;
	
	for(row=0;row<imgOut.m_ysize;row++)
	{
		for(col=0;col<imgOut.m_xsize;col++)
		{
			for(k=0;k<LEVEL;k++)
			{
				if(data[row*imgOut.m_xsize+col]==k)
				{
					data[row*imgOut.m_xsize+col]=int((LEVEL-1)*sk[k]+0.5);
					k=LEVEL;
				}
			}
		}
	}
	return TRUE;
}



BOOL CImageProcessingEx::histMatch(CImageDataset &imgSrc, CImageDataset &imgRef, CImageDataset &imgOut)
{
    const int LEVEL = 256;

    if (imgSrc.empty() || imgRef.empty())
    {
        return FALSE;
    }
    
    // 2. 复制源图像到输出
    if (FALSE == imgSrc.duplicate(imgOut))
    {
        return FALSE;
    }
    
    // 3. 计算源图像的直方图与累积分布
    double histSrc[LEVEL] = {0};
    double cdfSrc[LEVEL] = {0};
    
    for (int row = 0; row < imgSrc.m_ysize; row++)
    {
        for (int col = 0; col < imgSrc.m_xsize; col++)
        {
            int pixelValue = (int)imgSrc.m_data[row * imgSrc.m_xsize + col];
            if (pixelValue >= 0 && pixelValue < LEVEL)
            {
                histSrc[pixelValue]++;
            }
        }
    }
    
    // 计算源图像的累积分布
    double totalPixelsSrc = (double)(imgSrc.m_xsize * imgSrc.m_ysize);
    cdfSrc[0] = histSrc[0] / totalPixelsSrc;
    for (int i = 1; i < LEVEL; i++)
    {
        cdfSrc[i] = cdfSrc[i-1] + histSrc[i] / totalPixelsSrc;
    }
    
    // 4. 计算参考图像的直方图与累积分布
    double histRef[LEVEL] = {0};
    double cdfRef[LEVEL] = {0};
    
    for (int row = 0; row < imgRef.m_ysize; row++)
    {
        for (int col = 0; col < imgRef.m_xsize; col++)
        {
            int pixelValue = (int)imgRef.m_data[row * imgRef.m_xsize + col];
            if (pixelValue >= 0 && pixelValue < LEVEL)
            {
                histRef[pixelValue]++;
            }
        }
    }
    
    // 计算参考图像的累积分布
    double totalPixelsRef = (double)(imgRef.m_xsize * imgRef.m_ysize);
    cdfRef[0] = histRef[0] / totalPixelsRef;
    for (int i = 1; i < LEVEL; i++)
    {
        cdfRef[i] = cdfRef[i-1] + histRef[i] / totalPixelsRef;
    }
    
    // 5. 构建映射表：将源直方图级别映射到参考图像的级别
    int mapping[LEVEL] = {0};
    
    for (int i = 0; i < LEVEL; i++)
    {
        double cdfValue = cdfSrc[i];
        int matchedLevel = 0;
        
        // 在参考CDF中搜索最近匹配值
        double minDiff = fabs(cdfValue - cdfRef[0]);
        for (int j = 1; j < LEVEL; j++)
        {
            double diff = fabs(cdfValue - cdfRef[j]);
            if (diff < minDiff)
            {
                minDiff = diff;
                matchedLevel = j;
            }
        }
        mapping[i] = matchedLevel;
    }
    
    // 6. 将映射应用到输出图像
    double* data = imgOut.m_data;
    for (int row = 0; row < imgOut.m_ysize; row++)
    {
        for (int col = 0; col < imgOut.m_xsize; col++)
        {
            int pixelValue = (int)data[row * imgOut.m_xsize + col];
            if (pixelValue >= 0 && pixelValue < LEVEL)
            {
                data[row * imgOut.m_xsize + col] = (double)mapping[pixelValue];
            }
        }
    }
    
    return TRUE;
}
BOOL CImageProcessingEx::medianFilter(CImageDataset &imgIn, CImageDataset &imgOut, int windowSize)
{
    // 验证输入
    if (imgIn.empty())
    {
        return FALSE;
    }
    
    // 检查窗口大小是否合法（必须为奇数且大于0）
    if (windowSize % 2 == 0 || windowSize < 1)
    {
        return FALSE;
    }
    
    // 窗口不能大于图像尺寸
    if (windowSize > imgIn.m_xsize || windowSize > imgIn.m_ysize)
    {
        return FALSE;
    }
    
    // 复制图像到输出
    if (FALSE == imgIn.duplicate(imgOut))
    {
        return FALSE;
    }
    
    int halfSize = windowSize / 2;
    int totalPixels = windowSize * windowSize;
    
    // 使用 vector 存储邻域像素，避免栈溢出
    std::vector<double> neighbors(totalPixels);
    
    // 对每个像素进行处理，跳过边界
    for (int y = halfSize; y < imgIn.m_ysize - halfSize; y++)
    {
        for (int x = halfSize; x < imgIn.m_xsize - halfSize; x++)
        {

            int index = 0;
            for (int j = -halfSize; j <= halfSize; j++)
            {
                for (int i = -halfSize; i <= halfSize; i++)
                {
                    // 收集邻域像素
                    double pixelValue = imgIn.m_data[(y + j) * imgIn.m_xsize + (x + i)];
                    neighbors[index++] = pixelValue;
                }
            }
            
            // 对邻域像素进行排序
            std::sort(neighbors.begin(), neighbors.end());
            
            // 取中值
            double medianValue = neighbors[totalPixels / 2];
            
            // 写入输出
            imgOut.m_data[y * imgOut.m_xsize + x] = medianValue;
        }
    }
    

    
    return TRUE;
}

BOOL CImageProcessingEx::bilateralFilter(CImageDataset &imgIn, CImageDataset &imgOut,
                                         double sigmaS, double sigmaR, int windowSize)
{
    // 验证输入
    if (imgIn.empty())
        return FALSE;
    
    if (sigmaS <= 0 || sigmaR <= 0)
        return FALSE;
    
    if (windowSize % 2 == 0 || windowSize < 1)
        return FALSE;
    
    // 复制图像到输出
    if (FALSE == imgIn.duplicate(imgOut))
        return FALSE;
    
    int halfSize = windowSize / 2;
    double sigmaS2 = 2.0 * sigmaS * sigmaS;  
    double sigmaR2 = 2.0 * sigmaR * sigmaR;  
    
    // 预计算空间权重（高斯）
    std::vector<double> spatialWeight(windowSize * windowSize);
    int index = 0;
    for (int dy = -halfSize; dy <= halfSize; dy++)
    {
        for (int dx = -halfSize; dx <= halfSize; dx++)
        {
            double dist2 = dx * dx + dy * dy;
            spatialWeight[index++] = exp(-dist2 / sigmaS2);
        }
    }
    
    // 双边滤波主循环
    for (int y = halfSize; y < imgIn.m_ysize - halfSize; y++)
    {
        for (int x = halfSize; x < imgIn.m_xsize - halfSize; x++)
        {
            double centerPixel = imgIn.m_data[y * imgIn.m_xsize + x];
            double sumWeights = 0.0;
            double sumValues = 0.0;
            
            index = 0;
            for (int dy = -halfSize; dy <= halfSize; dy++)
            {
                for (int dx = -halfSize; dx <= halfSize; dx++)
                {
                    double neighborPixel = imgIn.m_data[(y + dy) * imgIn.m_xsize + (x + dx)];
                    
                    // 计算范围权重
                    double diff = centerPixel - neighborPixel;
                    double rangeWeight = exp(-(diff * diff) / sigmaR2);
                    
                    // 双边权重 = 空间权重 * 范围权重
                    double bilateralWeight = spatialWeight[index] * rangeWeight;
                    
                    sumWeights += bilateralWeight;
                    sumValues += bilateralWeight * neighborPixel;
                    
                    index++;
                }
            }
            
            // 归一化并写回像素值
            if (sumWeights > 0)
            {
                imgOut.m_data[y * imgOut.m_xsize + x] = sumValues / sumWeights;
            }
            else
            {
                imgOut.m_data[y * imgOut.m_xsize + x] = centerPixel;
            }
        }
    }
    
    return TRUE;
}

// Laplacian 锐化实现
BOOL CImageProcessingEx::laplacianSharpen(CImageDataset &imgIn, CImageDataset &imgOut)
{
    // 保持原有实现：简单 4 邻域拉普拉斯锐化
    if (imgIn.empty())
    {
        return FALSE;
    }

    if (FALSE == imgIn.duplicate(imgOut))
    {
        return FALSE;
    }

    double laplacianKernel[9] = {
        0, -1,  0,
        -1,  4, -1,
        0, -1,  0
    };

    int kernelSize = 3;
    int halfSize = kernelSize / 2;

    for (int y = halfSize; y < imgIn.m_ysize - halfSize; y++)
    {
        for (int x = halfSize; x < imgIn.m_xsize - halfSize; x++)
        {
            double laplacianValue = 0.0;
            int kernelIndex = 0;
            for (int dy = -halfSize; dy <= halfSize; dy++)
            {
                for (int dx = -halfSize; dx <= halfSize; dx++)
                {
                    double pixelValue = imgIn.m_data[(y + dy) * imgIn.m_xsize + (x + dx)];
                    laplacianValue += pixelValue * laplacianKernel[kernelIndex++];
                }
            }

            double sharpenedValue = imgIn.m_data[y * imgIn.m_xsize + x] - laplacianValue;
            if (sharpenedValue < 0) sharpenedValue = 0;
            if (sharpenedValue > 255) sharpenedValue = 255;
            imgOut.m_data[y * imgOut.m_xsize + x] = sharpenedValue;
        }
    }

    return TRUE;
}

BOOL CImageProcessingEx::createRectangleImage(CImageDataset &imgOut,
                                            int width, int height,
                                            int rectX, int rectY,
                                            int rectWidth, int rectHeight,
                                            BYTE backgroundColor,
                                            BYTE rectangleColor)
{
    // 版本/开始标记
    TRACE(_T("\n"));
    TRACE(_T("========================================\n"));
    TRACE(_T("=== createRectangleImage start ===\n"));

    // 1. 参数信息
    TRACE(_T("Parameters:\n"));
    TRACE(_T("  image size: width=%d, height=%d\n"), width, height);
    TRACE(_T("  rectangle pos: rectX=%d, rectY=%d\n"), rectX, rectY);
    TRACE(_T("  rectangle size: rectWidth=%d, rectHeight=%d\n"), rectWidth, rectHeight);
    TRACE(_T("  colors: background=%d, rectangle=%d\n"), backgroundColor, rectangleColor);

    // 2. 复制并初始化输出图像
    imgOut.clear();

    // 使用 0..255 原始像素值范围
    double bgValue = (double)backgroundColor;

    BOOL createResult = imgOut.create(width, height, 1, bgValue);
    TRACE(_T("imgOut.create() result: %s\n"), createResult ? _T("TRUE") : _T("FALSE"));

    if (!createResult)
    {
        TRACE(_T("Error: image creation failed\n"));
        return FALSE;
    }

    // 5. 输出基本信息
    TRACE(_T("Image created:\n"));
    TRACE(_T("  m_xsize=%d, m_ysize=%d\n"), imgOut.m_xsize, imgOut.m_ysize);
    TRACE(_T("  m_data ptr: %p\n"), imgOut.m_data);
    TRACE(_T("  empty(): %s\n"), imgOut.empty() ? _T("TRUE") : _T("FALSE"));

    if (imgOut.empty() || imgOut.m_data == NULL)
    {
        TRACE(_T("Error: image buffer is NULL\n"));
        return FALSE;
    }

    // 6. 初始化检查
    double* pData = imgOut.m_data;
    double samplePixel = pData[0];

    // 7. 计算矩形范围
    int startX = max(0, rectX);
    int startY = max(0, rectY);
    int endX = min(width, rectX + rectWidth);
    int endY = min(height, rectY + rectHeight);

    TRACE(_T("Rectangle bounds:\n"));
    TRACE(_T("  X: %d .. %d (width=%d)\n"), startX, endX, endX - startX);
    TRACE(_T("  Y: %d .. %d (height=%d)\n"), startY, endY, endY - startY);
    
    if (startX >= endX || startY >= endY)
    {
        TRACE(_T("Warning: rectangle bounds are invalid\n"));
        TRACE(_T("Rectangle may be outside image boundaries\n"));
    }
    


    // store rectangle color in 0..255 range
    double rectValue = (double)rectangleColor;
    TRACE(_T("Rectangle color value: %d -> %f\n"), rectangleColor, rectValue);
    
    int pixelCount = 0;
    for (int y = startY; y < endY; y++)
    {
        for (int x = startX; x < endX; x++)
        {
            int index = y * width + x;
            pData[index] = rectValue;
            pixelCount++;
            
            // log first modified pixel
            if (pixelCount == 1)
            {
                TRACE(_T("first modified pixel: index=%d, x=%d, y=%d, value=%f\n"), 
                    index, x, y, rectValue);
            }
        }
    }
    
        TRACE(_T("Total modified pixels: %d\n"), pixelCount);

        // 9. 验证填充
        TRACE(_T("Validate fill:\n"));

        // check a center pixel
    if (pixelCount > 0)
    {
        int midX = startX + (endX - startX) / 2;
        int midY = startY + (endY - startY) / 2;
        int midIndex = midY * width + midX;
        double midValue = pData[midIndex];
    }
    

    int outsideX = (startX > 10) ? 0 : width - 1;
    int outsideY = (startY > 10) ? 0 : height - 1;
    int outsideIndex = outsideY * width + outsideX;
    double outsideValue = pData[outsideIndex];
    
    return TRUE;
}


BOOL CImageProcessingEx::createSineImage(CImageDataset &imgOut,
                                       int width, int height,
                                       double frequency,
                                       double phase)
{
    // validate parameters
    if (width <= 0 || height <= 0 || frequency <= 0)
    {
        return FALSE;
    }
    
    // create output image buffer
    if (!imgOut.create(width, height, 1, 0.0))
    {
        return FALSE;
    }
    
    double* pData = imgOut.m_data;
    
    // compute normalized frequency per row
    double normalizedFreq = frequency / height;
    
    // generate sine pattern (horizontal bands)
    for (int y = 0; y < height; y++)
    {
        // value in range -1..1
        double value = sin(2.0 * 3.141592653589793 * normalizedFreq * y + phase);

        // map value (-1..1) -> 0..255 range (double)
        double grayNorm = (value + 1.0) * 0.5;
        if (grayNorm < 0.0) grayNorm = 0.0;
        if (grayNorm > 1.0) grayNorm = 1.0;
        double grayValue = grayNorm * 255.0;

        for (int x = 0; x < width; x++)
        {
            pData[y * width + x] = grayValue;
        }
    }
    
    return TRUE;
}

// create checkerboard image
BOOL CImageProcessingEx::createCheckerboardImage(CImageDataset &imgOut,
                                               int width, int height,
                                               int blockSize)
{
    // validate parameters
    if (width <= 0 || height <= 0 || blockSize <= 0)
    {
        return FALSE;
    }

    // create output image buffer
    if (!imgOut.create(width, height, 1, 0.0))
    {
        return FALSE;
    }

    double* pData = imgOut.m_data;

    // black and white values in 0..255
    double blackValue = 0.0;    // corresponds to BYTE 0
    double whiteValue = 255.0;  // corresponds to BYTE 255

    // fill checkerboard pattern
    for (int y = 0; y < height; y++)
    {
        int blockY = y / blockSize;

        for (int x = 0; x < width; x++)
        {
            int blockX = x / blockSize;

            // choose color based on block indices
            if ((blockX + blockY) % 2 == 0)
            {
                pData[y * width + x] = blackValue;
            }
            else
            {
                pData[y * width + x] = whiteValue;
            }
        }
    }

    return TRUE;
}

// 在 ImageProcessingEx.cpp 中添加以下函数实现


// DFT2D 实现 - 调用 CFourierTransform 类的函数
BOOL CImageProcessingEx::DFT2D(CImageDataset &imgIn, 
                              std::vector<std::complex<double>> &dftResult,
                              int &width, int &height)
{
    if (imgIn.empty())
    {
        TRACE(_T("Error: Input image is empty in DFT2D\n"));
        return FALSE;
    }
    
    width = imgIn.m_xsize;
    height = imgIn.m_ysize;
    
    TRACE(_T("CImageProcessingEx::DFT2D - Image size: %dx%d\n"), width, height);
    
    // 调用 CFourierTransform 类的静态函数
    BOOL result = CFourierTransform::DFT2D(imgIn, dftResult, width, height);
    
    if (!result)
    {
        TRACE(_T("Error: CFourierTransform::DFT2D failed\n"));
        return FALSE;
    }
    
    TRACE(_T("CImageProcessingEx::DFT2D - DFT result size: %d\n"), (int)dftResult.size());
    return TRUE;
}

// IDFT2D 实现
BOOL CImageProcessingEx::IDFT2D(std::vector<std::complex<double>> &dftResult,
                               CImageDataset &imgOut,
                               int width, int height)
{
    if (dftResult.empty())
    {
        TRACE(_T("Error: DFT result is empty in IDFT2D\n"));
        return FALSE;
    }
    
    if (width <= 0 || height <= 0)
    {
        TRACE(_T("Error: Invalid dimensions in IDFT2D: %dx%d\n"), width, height);
        return FALSE;
    }
    
    TRACE(_T("CImageProcessingEx::IDFT2D - Dimensions: %dx%d\n"), width, height);
    
    // 调用 CFourierTransform 类的静态函数
    BOOL result = CFourierTransform::IDFT2D(dftResult, imgOut, width, height);
    
    if (!result)
    {
        TRACE(_T("Error: CFourierTransform::IDFT2D failed\n"));
        return FALSE;
    }
    
    return TRUE;
}

// =======================
// PCA 相关实现（来自 myAlg_2）
// =======================

BOOL CImageProcessingEx::pcaForward(CImageDataset &imgIn, CImageDataset &imgPCA,
                                     CMatrix &matT, CMatrix &matEigValues, CMatrix &matMean)
{
    if (imgIn.empty() || imgIn.m_rastercount < 2)
    {
        AfxMessageBox(_T("PCA forward failed: need multi-band image (e.g., lanier.img)."));
        return FALSE;
    }
    int m = imgIn.m_rastercount;
    int n = imgIn.m_xsize * imgIn.m_ysize;

    float **matX = new float*[m];
    for (int i = 0; i < m; i++)
        matX[i] = new float[n]();

    float *pInDataFloat = new float[m * n]();
    double *pInData = imgIn.m_data;
    for (int i = 0; i < m * n; i++)
        pInDataFloat[i] = static_cast<float>(pInData[i]);

    for (int band = 0; band < m; band++)
        for (int col = 0; col < n; col++)
            matX[band][col] = pInDataFloat[band * n + col];
    delete[] pInDataFloat;

    float *pBandMean = new float[m]();
    matMean.create(1, m, 0.0);
    for (int band = 0; band < m; band++)
    {
        float fSum = 0.0f;
        for (int col = 0; col < n; col++)
            fSum += matX[band][col];
        pBandMean[band] = fSum / n;
        matMean.m_data[band] = static_cast<double>(pBandMean[band]);
        for (int col = 0; col < n; col++)
            matX[band][col] -= pBandMean[band];
    }
    delete[] pBandMean;

    float **matS = new float*[m];
    for (int i = 0; i < m; i++)
        matS[i] = new float[m]();
    for (int i = 0; i < m; i++)
        for (int j = 0; j < m; j++)
        {
            float fCov = 0.0f;
            for (int col = 0; col < n; col++)
                fCov += matX[i][col] * matX[j][col];
            matS[i][j] = fCov / (n - 1);
        }

    float *pEigVals = new float[m]();
    float *pEigVecs = new float[m * m]();

    CMatrix matSCtemp;
    matSCtemp.create(m, m, 0.0);
    for (int i = 0; i < m; i++)
        for (int j = 0; j < m; j++)
            matSCtemp.m_data[i * m + j] = static_cast<double>(matS[i][j]);

    if (!matSCtemp.eig(pEigVals, pEigVecs))
    {
        AfxMessageBox(_T("PCA forward failed: eigen decomposition failed."));
        for (int i = 0; i < m; i++) { delete[] matX[i]; delete[] matS[i]; }
        delete[] matX; delete[] matS; delete[] pEigVals; delete[] pEigVecs;
        return FALSE;
    }

    int *pIndex = new int[m]();
    for (int i = 0; i < m; i++) pIndex[i] = i;
    for (int i = 0; i < m - 1; i++)
        for (int j = i + 1; j < m; j++)
            if (pEigVals[pIndex[i]] < pEigVals[pIndex[j]])
                std::swap(pIndex[i], pIndex[j]);

    float **matT_float = new float*[m];
    for (int i = 0; i < m; i++)
        matT_float[i] = new float[m]();
    for (int col = 0; col < m; col++)
    {
        int nOrigCol = pIndex[col];
        for (int row = 0; row < m; row++)
            matT_float[row][col] = pEigVecs[row * m + nOrigCol];
    }

    float **matY = new float*[m];
    for (int i = 0; i < m; i++)
        matY[i] = new float[n]();
    for (int rowT = 0; rowT < m; rowT++)
    {
        for (int colX = 0; colX < n; colX++)
        {
            float fSum = 0.0f;
            for (int k = 0; k < m; k++)
                fSum += matT_float[k][rowT] * matX[k][colX];
            matY[rowT][colX] = fSum;
        }
    }

    matT.create(m, m, 0.0);
    for (int i = 0; i < m; i++)
        for (int j = 0; j < m; j++)
            matT.m_data[i * m + j] = static_cast<double>(matT_float[i][j]);

    imgIn.duplicate(imgPCA);
    {
        float *pPCDataFloat = new float[m * n]();
        for (int bandPC = 0; bandPC < m; bandPC++)
            for (int col = 0; col < n; col++)
                pPCDataFloat[bandPC * n + col] = matY[bandPC][col];
        for (int i = 0; i < m * n; i++)
            imgPCA.m_data[i] = static_cast<double>(pPCDataFloat[i]);
        delete[] pPCDataFloat;
    }

    matEigValues.create(1, m, 0.0);
    for (int i = 0; i < m; i++)
        matEigValues.m_data[i] = static_cast<double>(pEigVals[pIndex[i]]);

    for (int i = 0; i < m; i++) { delete[] matX[i]; delete[] matS[i]; delete[] matT_float[i]; delete[] matY[i]; }
    delete[] matX; delete[] matS; delete[] matT_float; delete[] matY;
    delete[] pIndex; delete[] pEigVals; delete[] pEigVecs;
    return TRUE;
}

BOOL CImageProcessingEx::pcaBackward(CImageDataset &imgPCA, CMatrix &matT, CMatrix &matMean,
                                      int nCustomP, CImageDataset &imgRecon)
{
    int m = imgPCA.m_rastercount;
    int nPixel = imgPCA.m_xsize * imgPCA.m_ysize;
    if (imgPCA.empty() || matT.empty() || nCustomP < 1 || nCustomP > m)
    {
        CString strErr;
        strErr.Format(_T("PCA inverse failed: p must be in [1, %d] (p=%d)."), m, nCustomP);
        AfxMessageBox(strErr);
        return FALSE;
    }
    if (matT.m_rows != m || matT.m_cols != m)
    {
        AfxMessageBox(_T("PCA inverse failed: transform matrix must be m x m (m = band count)."));
        return FALSE;
    }

    double **pPCData = new double*[m];
    double **pTransformMat = new double*[m];
    double *pPCAData = imgPCA.m_data;
    for (int band = 0; band < m; band++)
    {
        pPCData[band] = new double[nPixel]();
        for (int pix = 0; pix < nPixel; pix++)
            pPCData[band][pix] = pPCAData[band * nPixel + pix];
    }

    // CMatrix stores data as float*, read it and promote to double per element
    float *pTData = matT.m_data;
    for (int row = 0; row < m; row++)
    {
        pTransformMat[row] = new double[m]();
        for (int col = 0; col < m; col++)
            pTransformMat[row][col] = pTData[row * m + col];
    }

    double **pReconCentered = new double*[m];
    for (int band = 0; band < m; band++)
    {
        pReconCentered[band] = new double[nPixel]();
        double dMean = matMean.m_data[band];
        for (int p = 0; p < nCustomP; p++)
        {
            for (int pix = 0; pix < nPixel; pix++)
                pReconCentered[band][pix] += pTransformMat[band][p] * pPCData[p][pix];
        }
        for (int pix = 0; pix < nPixel; pix++)
            pReconCentered[band][pix] += dMean;
    }

    if (!imgPCA.duplicate(imgRecon))
    {
        AfxMessageBox(_T("PCA inverse failed: cannot create reconstructed image."));
        for (int i = 0; i < m; i++) { delete[] pPCData[i]; delete[] pTransformMat[i]; delete[] pReconCentered[i]; }
        delete[] pPCData; delete[] pTransformMat; delete[] pReconCentered;
        return FALSE;
    }
    double *pReconData = imgRecon.m_data;
    for (int band = 0; band < m; band++)
    {
        for (int pix = 0; pix < nPixel; pix++)
            pReconData[band * nPixel + pix] = pReconCentered[band][pix];
    }

    for (int i = 0; i < m; i++) { delete[] pPCData[i]; delete[] pTransformMat[i]; delete[] pReconCentered[i]; }
    delete[] pPCData; delete[] pTransformMat; delete[] pReconCentered;
    return TRUE;
}

double CImageProcessingEx::calcPcaRmse(CImageDataset &imgOrig, CImageDataset &imgRecon)
{
    if (imgOrig.empty() || imgRecon.empty() ||
        imgOrig.m_xsize != imgRecon.m_xsize ||
        imgOrig.m_ysize != imgRecon.m_ysize ||
        imgOrig.m_rastercount != imgRecon.m_rastercount)
    {
        AfxMessageBox(_T("RMSE failed: original and reconstructed images have different size or band count."));
        return -1.0;
    }

    int m = imgOrig.m_rastercount;
    int nPixel = imgOrig.m_xsize * imgOrig.m_ysize;
    double *pOrigData = imgOrig.m_data;
    double *pReconData = imgRecon.m_data;

    double dSumSqErr = 0.0;
    for (int band = 0; band < m; band++)
    {
        for (int pix = 0; pix < nPixel; pix++)
        {
            double dErr = pOrigData[band * nPixel + pix] - pReconData[band * nPixel + pix];
            dSumSqErr += dErr * dErr;
        }
    }
    double dMse = dSumSqErr / (m * nPixel);
    double dRmse = sqrt(dMse);
    return dRmse;
}

// =======================
// RGB-IHS 相关实现
// =======================

BOOL CImageProcessingEx::rgb2ihs(CImageDataset &imgIn, CImageDataset &imgIHS)
{
    if (imgIn.empty() || imgIn.m_rastercount != 3)
    {
        AfxMessageBox(_T("RGB-IHS transform needs a 3-band RGB image."));
        return FALSE;
    }

    if (!imgIn.duplicate(imgIHS)) return FALSE;

    int nPixel = imgIn.m_xsize * imgIn.m_ysize;
    double *R = imgIn.m_data;
    double *G = imgIn.m_data + nPixel;
    double *B = imgIn.m_data + 2 * nPixel;

    double *I = imgIHS.m_data;
    double *H = imgIHS.m_data + nPixel;
    double *S = imgIHS.m_data + 2 * nPixel;

    double PI = 3.14159265358979323846;

    for (int i = 0; i < nPixel; i++)
    {
        double r = R[i];
        double g = G[i];
        double b = B[i];

        double i_val = (r + g + b) / 3.0;

        double h_val = 0.0, s_val = 0.0;

        double num = 0.5 * ((r - g) + (r - b));
        double den = sqrt((r - g) * (r - g) + (r - b) * (g - b));
        double theta = (den == 0) ? 0 : acos(num / den);

        if (b > g) theta = 2 * PI - theta;

        h_val = theta;

        double min_rgb = min(r, min(g, b));
        s_val = (i_val == 0) ? 0 : (1.0 - min_rgb / i_val);

        I[i] = i_val;
        H[i] = h_val;
        S[i] = s_val;
    }
    return TRUE;
}

BOOL CImageProcessingEx::ihs2rgb(CImageDataset &imgIHS, CImageDataset &imgOut)
{
    if (imgIHS.empty() || imgIHS.m_rastercount != 3) return FALSE;
    if (!imgIHS.duplicate(imgOut)) return FALSE;

    int nPixel = imgIHS.m_xsize * imgIHS.m_ysize;
    double *I_ptr = imgIHS.m_data;
    double *H_ptr = imgIHS.m_data + nPixel;
    double *S_ptr = imgIHS.m_data + 2 * nPixel;

    double *R = imgOut.m_data;
    double *G = imgOut.m_data + nPixel;
    double *B = imgOut.m_data + 2 * nPixel;

    double PI = 3.14159265358979323846;

    for (int i = 0; i < nPixel; i++)
    {
        double i_val = I_ptr[i];
        double h_val = H_ptr[i];
        double s_val = S_ptr[i];

        double r = 0, g = 0, b = 0;

        if (s_val == 0)
        {
            r = g = b = i_val;
        }
        else
        {
            double h_deg = h_val * 180.0 / PI;
            while (h_deg >= 360) h_deg -= 360;
            while (h_deg < 0) h_deg += 360;

            double h_rad = h_deg * PI / 180.0;

            if (h_rad >= 0 && h_rad < 2 * PI / 3)
            {
                b = i_val * (1 - s_val);
                r = i_val * (1 + (s_val * cos(h_rad)) / cos(PI / 3 - h_rad));
                g = 3 * i_val - (r + b);
            }
            else if (h_rad >= 2 * PI / 3 && h_rad < 4 * PI / 3)
            {
                double h_prime = h_rad - 2 * PI / 3;
                r = i_val * (1 - s_val);
                g = i_val * (1 + (s_val * cos(h_prime)) / cos(PI / 3 - h_prime));
                b = 3 * i_val - (r + g);
            }
            else
            {
                double h_prime = h_rad - 4 * PI / 3;
                g = i_val * (1 - s_val);
                b = i_val * (1 + (s_val * cos(h_prime)) / cos(PI / 3 - h_prime));
                r = 3 * i_val - (g + b);
            }
        }

        R[i] = max(0.0, min(255.0, r));
        G[i] = max(0.0, min(255.0, g));
        B[i] = max(0.0, min(255.0, b));
    }
    return TRUE;
}

BOOL CImageProcessingEx::linearStretch(CImageDataset &imgIn, CImageDataset &imgOut)
{
    if (imgIn.empty()) return FALSE;
    if (!imgIn.duplicate(imgOut)) return FALSE;

    int nBands = imgIn.m_rastercount;
    int nPixel = imgIn.m_xsize * imgIn.m_ysize;

    for (int b = 0; b < nBands; b++)
    {
        double *pIn = imgIn.m_data + b * nPixel;
        double *pOut = imgOut.m_data + b * nPixel;

        double minV = pIn[0], maxV = pIn[0];
        for (int i = 1; i < nPixel; i++)
        {
            if (pIn[i] < minV) minV = pIn[i];
            if (pIn[i] > maxV) maxV = pIn[i];
        }

        if (maxV - minV < 1e-6)
        {
            for (int i = 0; i < nPixel; i++) pOut[i] = 0;
        }
        else
        {
            for (int i = 0; i < nPixel; i++)
            {
                pOut[i] = (pIn[i] - minV) / (maxV - minV) * 255.0;
            }
        }
    }
    return TRUE;
}

// =======================
// Canny 相关实现
// =======================

static void createGaussianKernel(double sigma, std::vector<double> &kernel, int &size)
{
    size = (int)(2.0 * 3.0 * sigma + 1.0);
    if (size % 2 == 0) size++;
    int half = size / 2;
    kernel.resize(size);
    double sum = 0.0;
    for (int i = 0; i < size; i++)
    {
        double x = i - half;
        kernel[i] = exp(-(x * x) / (2 * sigma * sigma));
        sum += kernel[i];
    }
    for (int i = 0; i < size; i++) kernel[i] /= sum;
}

BOOL CImageProcessingEx::cannyEdgeDetection(CImageDataset &imgIn, CImageDataset &imgOut,
                                             double sigma, double lowThresh, double highThresh)
{
    if (imgIn.empty()) return FALSE;

    int width = imgIn.m_xsize;
    int height = imgIn.m_ysize;
    int nPixel = width * height;

    std::vector<double> grayImg(nPixel);

    if (imgIn.m_rastercount >= 3)
    {
        double *pR = imgIn.m_data;
        double *pG = imgIn.m_data + nPixel;
        double *pB = imgIn.m_data + 2 * nPixel;
        for (int i = 0; i < nPixel; i++)
        {
            grayImg[i] = 0.30 * pR[i] + 0.59 * pG[i] + 0.11 * pB[i];
        }
    }
    else
    {
        double *pIn = imgIn.m_data;
        for (int i = 0; i < nPixel; i++)
            grayImg[i] = pIn[i];
    }

    std::vector<double> smoothImg(nPixel);
    std::vector<double> gradX(nPixel);
    std::vector<double> gradY(nPixel);
    std::vector<double> mag(nPixel);
    std::vector<double> dir(nPixel);

    std::vector<double> kernel;
    int kSize;
    createGaussianKernel(sigma, kernel, kSize);
    int halfK = kSize / 2;

    std::vector<double> tempImg(nPixel);
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            double sum = 0.0;
            for (int k = -halfK; k <= halfK; k++)
            {
                int nx = min(max(x + k, 0), width - 1);
                sum += grayImg[y * width + nx] * kernel[k + halfK];
            }
            tempImg[y * width + x] = sum;
        }
    }

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            double sum = 0.0;
            for (int k = -halfK; k <= halfK; k++)
            {
                int ny = min(max(y + k, 0), height - 1);
                sum += tempImg[ny * width + x] * kernel[k + halfK];
            }
            smoothImg[y * width + x] = sum;
        }
    }

    for (int y = 1; y < height - 1; y++)
    {
        for (int x = 1; x < width - 1; x++)
        {
            double gx = -smoothImg[(y - 1) * width + (x - 1)] + smoothImg[(y - 1) * width + (x + 1)]
                - 2 * smoothImg[y * width + (x - 1)] + 2 * smoothImg[y * width + (x + 1)]
                - smoothImg[(y + 1) * width + (x - 1)] + smoothImg[(y + 1) * width + (x + 1)];

            double gy = smoothImg[(y - 1) * width + (x - 1)] + 2 * smoothImg[(y - 1) * width + x] + smoothImg[(y - 1) * width + (x + 1)]
                - smoothImg[(y + 1) * width + (x - 1)] - 2 * smoothImg[(y + 1) * width + x] - smoothImg[(y + 1) * width + (x + 1)];

            gradX[y * width + x] = gx;
            gradY[y * width + x] = gy;
            mag[y * width + x] = sqrt(gx * gx + gy * gy);

            double angle = atan2(gy, gx) * 180.0 / 3.14159265;
            if (angle < 0) angle += 180.0;

            if (angle < 22.5 || angle >= 157.5) dir[y * width + x] = 0;
            else if (angle >= 22.5 && angle < 67.5) dir[y * width + x] = 1;
            else if (angle >= 67.5 && angle < 112.5) dir[y * width + x] = 2;
            else dir[y * width + x] = 3;
        }
    }

    std::vector<double> nms(width * height, 0.0);
    for (int y = 1; y < height - 1; y++)
    {
        for (int x = 1; x < width - 1; x++)
        {
            int d = (int)dir[y * width + x];
            double m = mag[y * width + x];
            double m1 = 0, m2 = 0;

            if (d == 0)
            {
                m1 = mag[y * width + (x - 1)];
                m2 = mag[y * width + (x + 1)];
            }
            else if (d == 1)
            {
                m1 = mag[(y - 1) * width + (x + 1)];
                m2 = mag[(y + 1) * width + (x - 1)];
            }
            else if (d == 2)
            {
                m1 = mag[(y - 1) * width + x];
                m2 = mag[(y + 1) * width + x];
            }
            else
            {
                m1 = mag[(y - 1) * width + (x - 1)];
                m2 = mag[(y + 1) * width + (x + 1)];
            }

            if (m >= m1 && m >= m2)
                nms[y * width + x] = m;
        }
    }

    imgOut.create(width, height, 1, 0.0);
    double *pOut = imgOut.m_data;
    std::vector<int> stack;

    for (int i = 0; i < width * height; i++)
    {
        if (nms[i] >= highThresh)
        {
            pOut[i] = 255.0;
            stack.push_back(i);
        }
        else if (nms[i] >= lowThresh)
        {
            pOut[i] = 128.0;
        }
        else
        {
            pOut[i] = 0.0;
        }
    }

    while (!stack.empty())
    {
        int idx = stack.back();
        stack.pop_back();

        int cx = idx % width;
        int cy = idx / width;

        for (int dy = -1; dy <= 1; dy++)
        {
            for (int dx = -1; dx <= 1; dx++)
            {
                if (dx == 0 && dy == 0) continue;
                int nx = cx + dx;
                int ny = cy + dy;
                if (nx >= 0 && nx < width && ny >= 0 && ny < height)
                {
                    int nidx = ny * width + nx;
                    if (pOut[nidx] == 128.0)
                    {
                        pOut[nidx] = 255.0;
                        stack.push_back(nidx);
                    }
                }
            }
        }
    }

    for (int i = 0; i < width * height; i++)
    {
        if (pOut[i] == 128.0) pOut[i] = 0.0;
    }

    return TRUE;
}

void CImageProcessingEx::getAutoCannyThresholds(CImageDataset &imgIn,
                                                  double &outLow, double &outHigh)
{
    if (imgIn.empty())
    {
        outLow = 50.0;
        outHigh = 150.0;
        return;
    }

    int nPixel = imgIn.m_xsize * imgIn.m_ysize;
    std::vector<double> grayImg(nPixel);

    if (imgIn.m_rastercount >= 3)
    {
        double *pR = imgIn.m_data;
        double *pG = imgIn.m_data + nPixel;
        double *pB = imgIn.m_data + 2 * nPixel;
        for (int i = 0; i < nPixel; i++)
            grayImg[i] = 0.30 * pR[i] + 0.59 * pG[i] + 0.11 * pB[i];
    }
    else
    {
        double *pIn = imgIn.m_data;
        for (int i = 0; i < nPixel; i++)
            grayImg[i] = pIn[i];
    }

    int hist[256] = { 0 };
    for (int i = 0; i < nPixel; i++)
    {
        int val = (int)grayImg[i];
        if (val < 0) val = 0;
        if (val > 255) val = 255;
        hist[val]++;
    }

    int median = 128;
    int count = 0;
    for (int i = 0; i < 256; i++)
    {
        count += hist[i];
        if (count >= nPixel / 2)
        {
            median = i;
            break;
        }
    }

    double v = (double)median;
    if (v < 50) v = 50;
    outHigh = v * 1.5;
    outLow = outHigh * 0.4;
}



