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
    // 验证输入
    if (imgIn.empty())
    {
        return FALSE;
    }

    if (FALSE == imgIn.duplicate(imgOut))
    {
        return FALSE;
    }
    
    // 使用如下拉普拉斯核（4邻域）
    //  0  -1   0
    // -1   4  -1
    //  0  -1   0
    double laplacianKernel[9] = {
        0, -1,  0,
        -1,  4, -1,
        0, -1,  0
    };
    
    // 也可使用8邻域的拉普拉斯核：
    // -1  -1  -1
    // -1   8  -1
    // -1  -1  -1
    
    int kernelSize = 3;
    int halfSize = kernelSize / 2;
    
    // 对每个像素应用卷积，跳过边界
    for (int y = halfSize; y < imgIn.m_ysize - halfSize; y++)
    {
        for (int x = halfSize; x < imgIn.m_xsize - halfSize; x++)
        {
            double laplacianValue = 0.0;
            
            // 应用卷积核
            int kernelIndex = 0;
            for (int dy = -halfSize; dy <= halfSize; dy++)
            {
                for (int dx = -halfSize; dx <= halfSize; dx++)
                {
                    double pixelValue = imgIn.m_data[(y + dy) * imgIn.m_xsize + (x + dx)];
                    laplacianValue += pixelValue * laplacianKernel[kernelIndex++];
                }
            }
            
            // 锐化：原图 - 拉普拉斯
            double sharpenedValue = imgIn.m_data[y * imgIn.m_xsize + x] - laplacianValue;
            
            // 截断到 0-255
            if (sharpenedValue < 0) sharpenedValue = 0;
            if (sharpenedValue > 255) sharpenedValue = 255;
            
            imgOut.m_data[y * imgOut.m_xsize + x] = sharpenedValue;
        }
    }
    
    // 边界像素保持原值（暂不处理边缘）
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



