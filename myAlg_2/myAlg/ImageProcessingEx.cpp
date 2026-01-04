#include "StdAfx.h"
#include "ImageProcessingEx.h"
#include <cmath>
#include <algorithm>
#include "afxdialogex.h"
#include <vector>

CImageProcessingEx::CImageProcessingEx(void)
{
}


CImageProcessingEx::~CImageProcessingEx(void)
{
}

BOOL CImageProcessingEx::histeq(CImageDataset &imgIn,CImageDataset &imgOut)
{
	const int LEVEL = 256;
    int k, row, col;
    double hist[LEVEL], sk[LEVEL];
    /* checking image validation */
    if(imgIn.empty())
    {
       return FALSE;
    }
    /* creating the output image */
    if(FALSE == imgIn.duplicate(imgOut))//get new image from input image
    {
       return FALSE;
    }
    double *data = imgOut.m_data;
	/* 步骤 1：计算输入图像的直方图 */
    for(k=0;k< LEVEL;k++) //初始化直方图数据为 0
    {
       hist[k]=0; 
    }
    for(row=0; row<imgIn.m_ysize; row++) //对图像逐行逐列进行扫描，计算完成直方图
    {
       for(col=0; col<imgIn.m_xsize; col++)
       {
            hist[UINT8(data[row*imgIn.m_xsize+col])]++;
       }
    }
    /* 步骤 2：计算累积直方图 */
    sk[0]= hist[0]/ (imgIn.m_ysize*imgIn.m_xsize);
	for(k=1;k< LEVEL-1;k++) //对直方图索引进行扫描，计算完成累积直方图
    {
       sk[k]=sk[k-1]+ hist[k]/ (imgIn.m_ysize*imgIn.m_xsize);
    }
    sk[255]=1;
    /* 步骤 3：根据用累积直方图建立灰度变换函数，并逐行逐列进行像素的灰度变换 */
    for(row=0; row<imgOut.m_ysize; row++)
    {
       for(col=0; col<imgOut.m_xsize; col++)
       {
          for(k=0;k<LEVEL;k++)
          {
            if(data[row*imgOut.m_xsize+col] == k )
            {
                 data[row*imgOut.m_xsize+col] = int((LEVEL-1)*sk[k]+0.5);//灰度变换
                 k=LEVEL;
            }
          }
       }
     }
     return TRUE;
}

// 直方图匹配函数实现
BOOL CImageProcessingEx::histmatch(CImageDataset &imgIn, CImageDataset &imgRef, CImageDataset &imgOut)
{
    const int LEVEL = 256; // 8位图像灰度级
    double histIn[LEVEL] = {0}, histRef[LEVEL] = {0};
    double skF[LEVEL] = {0}, smR[LEVEL] = {0};
    int grayMap[LEVEL] = {0};
    int totalPixel = imgIn.m_xsize * imgIn.m_ysize;

    // 1. 校验图像有效性
    if (imgIn.empty() || imgRef.empty())
    {
        AfxMessageBox(_T("图像为空！"));
        return FALSE;
    }

    // 2. 创建输出图像
    if (!imgIn.duplicate(imgOut))
    {
        AfxMessageBox(_T("创建输出图像失败！"));
        return FALSE;
    }

    // 3. 计算待匹配图像的直方图+累积直方图
    for (int i = 0; i < totalPixel; i++)
    {
        int gray = (int)UINT8(imgIn.m_data[i]);
        histIn[gray]++;
    }
    for (int k = 0; k < LEVEL; k++) histIn[k] /= totalPixel;
    skF[0] = histIn[0];
    for (int k = 1; k < LEVEL; k++) skF[k] = skF[k-1] + histIn[k];

    // 4. 计算参考图像的直方图+累积直方图
    for (int i = 0; i < totalPixel; i++)
    {
        int gray = (int)UINT8(imgRef.m_data[i]);
        histRef[gray]++;
    }
    for (int m = 0; m < LEVEL; m++) histRef[m] /= totalPixel;
    smR[0] = histRef[0];
    for (int m = 1; m < LEVEL; m++) smR[m] = smR[m-1] + histRef[m];

    // 5. 建立灰度映射表
    for (int k = 0; k < LEVEL; k++)
    {
        double minDiff = 1.0;
        int bestM = 0;
        for (int m = 0; m < LEVEL; m++)
        {
            double diff = fabs(skF[k] - smR[m]);
            if (diff < minDiff) { minDiff = diff; bestM = m; }
        }
        grayMap[k] = bestM;
    }

    // 6. 像素变换生成结果
    for (int i = 0; i < totalPixel; i++)
    {
        int gray = (int)UINT8(imgIn.m_data[i]);
        imgOut.m_data[i] = (double)grayMap[gray];
    }

    return TRUE;
}

// 中值滤波实现
BOOL CImageProcessingEx::medianFilter(CImageDataset &imgIn, CImageDataset &imgOut, int winSize)
{
    // 1. 输入校验
    if (imgIn.empty() || winSize % 2 == 0 || winSize < 3 || winSize > 11)
    {
        AfxMessageBox(_T("中值滤波失败：图像为空或窗口大小非法！"));
        return FALSE;
    }

    // 2. 创建输出图像（与输入同尺寸/波段）
    if (!imgIn.duplicate(imgOut))
    {
        AfxMessageBox(_T("中值滤波失败：创建输出图像失败！"));
        return FALSE;
    }

    // 3. 初始化参数
    int bandCount = imgIn.m_rastercount;
    int width = imgIn.m_xsize;
    int height = imgIn.m_ysize;
    int halfWin = winSize / 2;
    int totalPixelPerBand = width * height;

    // 4. 多波段循环处理
    for (int band = 0; band < bandCount; band++)
    {
        double *pInBand = imgIn.m_data + band * totalPixelPerBand;
        double *pOutBand = imgOut.m_data + band * totalPixelPerBand;

        // 5. 滑动窗口遍历图像
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                // 6. 收集窗口内像素（边界处理：取最近像素）
                std::vector<unsigned char> winPixels;
                for (int dy = -halfWin; dy <= halfWin; dy++)
                {
                    for (int dx = -halfWin; dx <= halfWin; dx++)
                    {
                        int ny = max(0, min(y + dy, height - 1));
                        int nx = max(0, min(x + dx, width - 1));
                        winPixels.push_back(UINT8(pInBand[ny * width + nx]));
                    }
                }

                // 7. 排序取中值
                sort(winPixels.begin(), winPixels.end());
                int medianIdx = winPixels.size() / 2;
                pOutBand[y * width + x] = (double)winPixels[medianIdx];
            }
        }
    }

    return TRUE;
}

// 双边滤波函数实现：输入图像imgIn，输出图像imgOut，参数d（窗口直径）、sigmaS（空间域σ）、sigmaR（灰度域σ）
BOOL CImageProcessingEx::bilateralFilter(CImageDataset &imgIn, CImageDataset &imgOut, 
                                         int d, double sigmaS, double sigmaR)
{
    // 1. 输入参数合法性校验
    if (imgIn.empty())  // 校验输入图像是否为空
    {
        AfxMessageBox(_T("双边滤波失败：未读取输入图像！"));
        return FALSE;
    }
    if (d % 2 == 0 || d < 3 || d > 15)  // 窗口d必须为≥3的奇数
    {
        CString strErr;
        strErr.Format(_T("双边滤波失败：窗口大小d需为3~15的奇数（当前d=%d）！"), d);
        AfxMessageBox(strErr);
        return FALSE;
    }
    if (sigmaS <= 0 || sigmaR <= 0)  // σₛ、σᵣ需为正数
    {
        AfxMessageBox(_T("双边滤波失败：空间域sigmaS、灰度域sigmaR需大于0！"));
        return FALSE;
    }

    // 2. 创建输出图像（复制输入图像的行列数、波段数、数据类型，避免重新分配内存）
    if (!imgIn.duplicate(imgOut))
    {
        AfxMessageBox(_T("双边滤波失败：创建输出图像结构失败！"));
        return FALSE;
    }

    // 3. 初始化核心参数（窗口半宽=直径/2，减少重复计算）
    int halfD = d / 2;                  // 窗口半宽（如d=5时，halfD=2，窗口范围[-2,2]）
    int bandCount = imgIn.m_rastercount;// 图像波段数（如RGB图像为3波段）
    int height = imgIn.m_ysize;         // 图像行数（y方向）
    int width = imgIn.m_xsize;          // 图像列数（x方向）
    int totalPixelPerBand = width * height;  // 单波段总像素数

    // 4. 多波段循环处理（BSQ格式：每个波段独立滤波，避免彩色图像偏色）
    for (int band = 0; band < bandCount; band++)
    {
        // 获取当前波段的输入/输出数据指针（BSQ格式：波段0→波段1→波段2...）
        double *pInBand = imgIn.m_data + band * totalPixelPerBand;
        double *pOutBand = imgOut.m_data + band * totalPixelPerBand;

        // 5. 滑动窗口遍历图像（逐行逐列处理每个像素）
        for (int y = 0; y < height; y++)  // y：当前像素行坐标
        {
            for (int x = 0; x < width; x++)  // x：当前像素列坐标
            {
                double weightedSum = 0.0;  // 窗口内像素的加权和
                double totalWeight = 0.0;   // 窗口内总权重（用于归一化）
                double centerGray = pInBand[y * width + x];  // 当前中心像素灰度值

                // 6. 遍历窗口内所有像素（计算每个像素的权重）
                for (int dy = -halfD; dy <= halfD; dy++)  // 窗口行偏移（从-halfD到halfD）
                {
                    for (int dx = -halfD; dx <= halfD; dx++)  // 窗口列偏移
                    {
                        // 边界处理：超出图像范围时取最近边界像素
                        int ny = max(0, min(y + dy, height - 1));  // 窗口像素行坐标（校正后）
                        int nx = max(0, min(x + dx, width - 1));   // 窗口像素列坐标（校正后）
                        double winGray = pInBand[ny * width + nx];  // 窗口内当前像素灰度值

                        // 7. 计算空间域权重Wₛ（基于欧式距离）
                        double spaceDist = sqrt(static_cast<double>(dx * dx + dy * dy));
                        double spaceWeight = exp(-(spaceDist * spaceDist) / (2 * sigmaS * sigmaS));

                        // 8. 计算灰度域权重Wᵣ（基于灰度差）
                        double grayDiff = fabs(winGray - centerGray);  // 灰度差绝对值
                        double grayWeight = exp(-(grayDiff * grayDiff) / (2 * sigmaR * sigmaR));

                        // 9. 总权重=空间权重×灰度权重（双边滤波核心）
                        double weight = spaceWeight * grayWeight;

                        // 10. 累积加权像素和与总权重
                        weightedSum += weight * winGray;
                        totalWeight += weight;
                    }
                }

                // 11. 归一化：加权平均作为输出像素（避免总权重为0的极端情况）
                if (totalWeight > 1e-6)  // 防止除零（浮点数精度判断）
                    pOutBand[y * width + x] = weightedSum / totalWeight;
                else
                    pOutBand[y * width + x] = centerGray;  // 权重为0时保留原像素（无滤波）
            }
        }
    }

    return TRUE;  // 滤波成功
}

// 拉普拉斯锐化实现（4邻域模板）
BOOL CImageProcessingEx::laplacianSharpen(CImageDataset &imgIn, CImageDataset &imgOut)
{
    // 1. 输入有效性校验
    if (imgIn.empty())
    {
        AfxMessageBox(_T("拉普拉斯锐化失败：输入图像为空！"));
        return FALSE;
    }

    // 2. 创建输出图像（与输入同尺寸、同波段数）
    if (!imgIn.duplicate(imgOut))
    {
        AfxMessageBox(_T("拉普拉斯锐化失败：创建输出图像失败！"));
        return FALSE;
    }

    // 3. 多波段循环处理（BSQ格式，每个波段单独锐化）
    int bandCount = imgIn.m_rastercount;  // 波段数（如3波段RGB）
    int width = imgIn.m_xsize;           // 图像列数（x方向）
    int height = imgIn.m_ysize;          // 图像行数（y方向）
    int totalPixelPerBand = width * height;

    for (int band = 0; band < bandCount; band++)
    {
        // 3.1 获取当前波段的像素指针（BSQ格式：band×总像素数=波段起始地址）
        double *pInBand = imgIn.m_data + band * totalPixelPerBand;
        double *pOutBand = imgOut.m_data + band * totalPixelPerBand;

        // 3.2 逐像素计算拉普拉斯锐化（跳过边缘？不，用复制边界处理边缘）
        for (int y = 0; y < height; y++)  // 行（y方向）
        {
            for (int x = 0; x < width; x++)  // 列（x方向）
            {
                // 边界处理：超出图像范围时，取最近的边界像素（避免越界）
                int x1 = max(0, min(x - 1, width - 1));  // 左邻像素列坐标
                int x2 = max(0, min(x + 1, width - 1));  // 右邻像素列坐标
                int y1 = max(0, min(y - 1, height - 1)); // 上邻像素行坐标
                int y2 = max(0, min(y + 1, height - 1)); // 下邻像素行坐标

                // 3.3 读取邻域像素（转UINT8避免浮点数误差，课程指导书2.4.2节同款用法）
                unsigned char f = UINT8(pInBand[y * width + x]);       // 中心像素f(x,y)
                unsigned char fL = UINT8(pInBand[y * width + x1]);    // 左邻f(x-1,y)
                unsigned char fR = UINT8(pInBand[y * width + x2]);    // 右邻f(x+1,y)
                unsigned char fU = UINT8(pInBand[y1 * width + x]);    // 上邻f(x,y-1)
                unsigned char fD = UINT8(pInBand[y2 * width + x]);    // 下邻f(x,y+1)

                // 3.4 计算拉普拉斯二阶微分（课程指导书公式3.3.1）
                int laplacian = fL + fR + fU + fD - 4 * f;

                // 3.5 锐化结果：g(x,y) = f(x,y) - ∇²f（避免灰度溢出，转UINT8）
                int sharpened = f - laplacian;
                // 裁剪到0~255（防止负数值或超255）
                sharpened = max(0, min(sharpened, 255));
                pOutBand[y * width + x] = (double)sharpened;
            }
        }
    }

    return TRUE;
}
// PCA正变换
BOOL CImageProcessingEx::pcaForward(CImageDataset &imgIn, CImageDataset &imgPCA, CMatrix &matT, CMatrix &matEigValues, CMatrix &matMean)
{
    // 1. 输入校验
    if (imgIn.empty() || imgIn.m_rastercount < 2)
    {
        AfxMessageBox(_T("PCA正变换失败：需多波段图像（如lanier.img）！"));
        return FALSE;
    }
    int m = imgIn.m_rastercount;          // 波段数
    int n = imgIn.m_xsize * imgIn.m_ysize;// 单波段像素数


    // 2. 构建数据矩阵X
    float **matX = new float*[m];
    for (int i = 0; i < m; i++)
        matX[i] = new float[n]();
    // 从框架的double转float
    float *pInDataFloat = new float[m * n]();
    double *pInData = imgIn.m_data;
    for (int i = 0; i < m * n; i++)
        pInDataFloat[i] = static_cast<float>(pInData[i]);
    // 填充matX
    for (int band = 0; band < m; band++)
        for (int col = 0; col < n; col++)
            matX[band][col] = pInDataFloat[band * n + col];
    delete[] pInDataFloat;


    // 3. 数据中心化
    float *pBandMean = new float[m]();
    matMean.create(1, m, 0.0); // 创建均值矩阵
    for (int band = 0; band < m; band++)
    {
        float fSum = 0.0f;
        for (int col = 0; col < n; col++)
            fSum += matX[band][col];
        pBandMean[band] = fSum / n;
        matMean.m_data[band] = static_cast<double>(pBandMean[band]); // 保存均值
        for (int col = 0; col < n; col++)
            matX[band][col] -= pBandMean[band];
    }
    delete[] pBandMean;


    // 4. 计算协方差矩阵S
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


    // 5. 特征分解
    float *pEigVals = new float[m]();    // 特征值
    float *pEigVecs = new float[m*m]();  // 特征向量
    
    // 将协方差矩阵转成框架的CMatrix
    CMatrix matSCtemp;
    matSCtemp.create(m, m, 0.0);
    for (int i = 0; i < m; i++)
        for (int j = 0; j < m; j++)
            matSCtemp.m_data[i * m + j] = static_cast<double>(matS[i][j]);
    
    // 调用eig
    if (!matSCtemp.eig(pEigVals, pEigVecs))
    {
        AfxMessageBox(_T("PCA正变换失败：特征分解失败！"));
        // 释放内存
        for (int i = 0; i < m; i++) { delete[] matX[i]; delete[] matS[i]; }
        delete[] matX; delete[] matS; delete[] pEigVals; delete[] pEigVecs;
        return FALSE;
    }


    // 6. 特征值降序排序
    int *pIndex = new int[m]();
    for (int i = 0; i < m; i++) pIndex[i] = i;
    for (int i = 0; i < m-1; i++)
        for (int j = i+1; j < m; j++)
            if (pEigVals[pIndex[i]] < pEigVals[pIndex[j]])
                std::swap(pIndex[i], pIndex[j]);


    // 7. 构建变换矩阵T
    float **matT_float = new float*[m];
    for (int i = 0; i < m; i++)
        matT_float[i] = new float[m]();
    for (int col = 0; col < m; col++)
    {
        int nOrigCol = pIndex[col];
        for (int row = 0; row < m; row++)
            matT_float[row][col] = pEigVecs[row * m + nOrigCol];
    }


    // 8. 计算主成分Y
    float **matY = new float*[m];
    for (int i = 0; i < m; i++)
        matY[i] = new float[n]();
    for (int rowT = 0; rowT < m; rowT++)  // T^T的行 = T的列
    {
        for (int colX = 0; colX < n; colX++)
        {
            float fSum = 0.0f;
            for (int k = 0; k < m; k++)
                fSum += matT_float[k][rowT] * matX[k][colX];
            matY[rowT][colX] = fSum;
        }
    }


    // 9. 适配框架：转成double存CMatrix/CImageDataset 
    // 9.1 变换矩阵matT（float→double）
    matT.create(m, m, 0.0);
    for (int i = 0; i < m; i++)
        for (int j = 0; j < m; j++)
            matT.m_data[i * m + j] = static_cast<double>(matT_float[i][j]);
    
    // 9.2 主成分影像imgPCA（float→double）
    imgIn.duplicate(imgPCA);
    float *pPCDataFloat = new float[m * n]();
    for (int bandPC = 0; bandPC < m; bandPC++)
        for (int col = 0; col < n; col++)
            pPCDataFloat[bandPC * n + col] = matY[bandPC][col];
    // 转成框架的double
    for (int i = 0; i < m * n; i++)
        imgPCA.m_data[i] = static_cast<double>(pPCDataFloat[i]);
    delete[] pPCDataFloat;
    
    // 9.3 特征值matEigValues（float→double）
    matEigValues.create(1, m, 0.0);
    for (int i = 0; i < m; i++)
        matEigValues.m_data[i] = static_cast<double>(pEigVals[pIndex[i]]);


    // 释放内存
    for (int i = 0; i < m; i++) { delete[] matX[i]; delete[] matS[i]; delete[] matT_float[i]; delete[] matY[i]; }
    delete[] matX; delete[] matS; delete[] matT_float; delete[] matY;
    delete[] pIndex; delete[] pEigVals; delete[] pEigVecs;
    return TRUE;
}
//PCA反变换
BOOL CImageProcessingEx::pcaBackward(CImageDataset &imgPCA, CMatrix &matT, CMatrix &matMean, int nCustomP, CImageDataset &imgRecon)
{
    // 1. 输入校验
    int m = imgPCA.m_rastercount;
    int nPixel = imgPCA.m_xsize * imgPCA.m_ysize;
    if (imgPCA.empty() || matT.empty() || nCustomP < 1 || nCustomP > m)
    {
        CString strErr;
        strErr.Format(_T("PCA反变换失败：p值需满足1≤p≤%d（当前p=%d）！"), m, nCustomP);
        AfxMessageBox(strErr);
        return FALSE;
    }
    if (matT.m_rows != m || matT.m_cols != m)
    {
        AfxMessageBox(_T("PCA反变换失败：变换矩阵尺寸需为m×m（m=波段数）！"));
        return FALSE;
    }

    // 2. 提取数据
    double **pPCData = new double*[m];
    double **pTransformMat = new double*[m];
    double *pPCAData = imgPCA.m_data;
    // 读取主成分影像数据
    for (int band = 0; band < m; band++)
    {
        pPCData[band] = new double[nPixel]();
        for (int pix = 0; pix < nPixel; pix++)
            pPCData[band][pix] = pPCAData[band * nPixel + pix];
    }
    // 读取变换矩阵数据
    float *pTData = matT.m_data;
    for (int row = 0; row < m; row++)
    {
        pTransformMat[row] = new double[m]();
        for (int col = 0; col < m; col++)
            pTransformMat[row][col] = pTData[row * m + col];
    }

    // 3. 重构中心化影像
    double **pReconCentered = new double*[m];
    for (int band = 0; band < m; band++)
    {
        pReconCentered[band] = new double[nPixel]();
        double dMean = matMean.m_data[band]; // 获取该波段均值
        // 仅用前nCustomP个主成分
        for (int p = 0; p < nCustomP; p++)
        {
            for (int pix = 0; pix < nPixel; pix++)
                pReconCentered[band][pix] += pTransformMat[band][p] * pPCData[p][pix];
        }
        // 加上均值（反中心化）
        for (int pix = 0; pix < nPixel; pix++)
            pReconCentered[band][pix] += dMean;
    }

    // 4. 生成重建影像
    if (!imgPCA.duplicate(imgRecon))
    {
        AfxMessageBox(_T("PCA反变换失败：创建重建影像失败！"));
        // 释放内存
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

    // 5. 释放内存
    for (int i = 0; i < m; i++) { delete[] pPCData[i]; delete[] pTransformMat[i]; delete[] pReconCentered[i]; }
    delete[] pPCData; delete[] pTransformMat; delete[] pReconCentered;
    return TRUE;
}
//PCA计算MSE
double CImageProcessingEx::calcPcaRmse(CImageDataset &imgOrig, CImageDataset &imgRecon)
{
    // 1. 影像一致性校验
    if (imgOrig.empty() || imgRecon.empty() ||
        imgOrig.m_xsize != imgRecon.m_xsize ||
        imgOrig.m_ysize != imgRecon.m_ysize ||
        imgOrig.m_rastercount != imgRecon.m_rastercount)
    {
        AfxMessageBox(_T("RMSE计算失败：原始影像与重建影像尺寸/波段数不匹配！"));
        return -1.0;
    }

    // 2. 计算均方根误差
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

// RGB-IHS 正变换
BOOL CImageProcessingEx::rgb2ihs(CImageDataset &imgIn, CImageDataset &imgIHS)
{
    if (imgIn.empty() || imgIn.m_rastercount != 3)
    {
        AfxMessageBox(_T("RGB-IHS变换需要3波段RGB图像！"));
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

        // Intensity
        double i_val = (r + g + b) / 3.0;

        // Saturation & Hue
        double h_val = 0.0, s_val = 0.0;
        
        double num = 0.5 * ((r - g) + (r - b));
        double den = sqrt((r - g)*(r - g) + (r - b)*(g - b));
        double theta = (den == 0) ? 0 : acos(num / den); // radians
        
        if (b > g) theta = 2 * PI - theta;
        
        h_val = theta; // 0 ~ 2pi
        
        double min_rgb = min(r, min(g, b));
        s_val = (i_val == 0) ? 0 : (1.0 - min_rgb / i_val); // 0 ~ 1

        I[i] = i_val;
        H[i] = h_val;
        S[i] = s_val;
    }
    return TRUE;
}

// RGB-IHS 反变换
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
        double h_val = H_ptr[i]; // radians
        double s_val = S_ptr[i];
        
        double r = 0, g = 0, b = 0;
        
        if (s_val == 0)
        {
            r = g = b = i_val;
        }
        else
        {
            double h_deg = h_val * 180.0 / PI;
            while(h_deg >= 360) h_deg -= 360;
            while(h_deg < 0) h_deg += 360;
            
            double h_rad = h_deg * PI / 180.0;

            if (h_rad >= 0 && h_rad < 2*PI/3)
            {
                b = i_val * (1 - s_val);
                r = i_val * (1 + (s_val * cos(h_rad)) / cos(PI/3 - h_rad));
                g = 3 * i_val - (r + b);
            }
            else if (h_rad >= 2*PI/3 && h_rad < 4*PI/3)
            {
                double h_prime = h_rad - 2*PI/3;
                r = i_val * (1 - s_val);
                g = i_val * (1 + (s_val * cos(h_prime)) / cos(PI/3 - h_prime));
                b = 3 * i_val - (r + g);
            }
            else
            {
                double h_prime = h_rad - 4*PI/3;
                g = i_val * (1 - s_val);
                b = i_val * (1 + (s_val * cos(h_prime)) / cos(PI/3 - h_prime));
                r = 3 * i_val - (g + b);
            }
        }
        
        R[i] = max(0.0, min(255.0, r));
        G[i] = max(0.0, min(255.0, g));
        B[i] = max(0.0, min(255.0, b));
    }
    return TRUE;
}

// 线性拉伸
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
        for(int i=1; i<nPixel; i++)
        {
            if(pIn[i] < minV) minV = pIn[i];
            if(pIn[i] > maxV) maxV = pIn[i];
        }
        
        if (maxV - minV < 1e-6)
        {
            for(int i=0; i<nPixel; i++) pOut[i] = 0;
        }
        else
        {
            for(int i=0; i<nPixel; i++)
            {
                pOut[i] = (pIn[i] - minV) / (maxV - minV) * 255.0;
            }
        }
    }
    return TRUE;
}

// Helper: Create Gaussian Kernel
void createGaussianKernel(double sigma, std::vector<double>& kernel, int& size)
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

// Canny边缘检测算法实现

BOOL CImageProcessingEx::cannyEdgeDetection(CImageDataset &imgIn, CImageDataset &imgOut, double sigma, double lowThresh, double highThresh)
{
    // 校验输入图像是否为空
    if (imgIn.empty()) return FALSE;

    int width = imgIn.m_xsize;    // 图像宽度
    int height = imgIn.m_ysize;   // 图像高度
    int nPixel = width * height;  // 图像总像素数

    // 1. 转换为灰度图
    std::vector<double> grayImg(nPixel);
    
    // 若为多波段（RGB）图像，转换为灰度图
    if (imgIn.m_rastercount >= 3)
    {
        // RGB转灰度公式：Gray = 0.30*红 + 0.59*绿 + 0.11*蓝
        double* pR = imgIn.m_data;                // 红波段数据指针
        double* pG = imgIn.m_data + nPixel;       // 绿波段数据指针
        double* pB = imgIn.m_data + 2 * nPixel;   // 蓝波段数据指针
        
        for (int i = 0; i < nPixel; i++)
        {
            grayImg[i] = 0.30 * pR[i] + 0.59 * pG[i] + 0.11 * pB[i];
        }
    }
    else
    {
        // 单波段/其他类型图像：直接使用第一个波段作为灰度图
        double* pIn = imgIn.m_data;
        for (int i = 0; i < nPixel; i++)
        {
            grayImg[i] = pIn[i];
        }
    }
    
    // 临时缓冲区定义
    std::vector<double> smoothImg(nPixel);  // 高斯平滑后的图像
    std::vector<double> gradX(nPixel);      // X方向梯度（水平）
    std::vector<double> gradY(nPixel);      // Y方向梯度（垂直）
    std::vector<double> mag(nPixel);        // 梯度幅值（边缘强度）
    std::vector<double> dir(nPixel);        // 梯度方向（量化为0/45/90/135度）
    
    // 2. 高斯平滑（可分离核，分水平+垂直两步）
    std::vector<double> kernel;  // 高斯核数组
    int kSize;                   // 高斯核尺寸
    createGaussianKernel(sigma, kernel, kSize); // 创建对应sigma的高斯核
    int halfK = kSize / 2;       // 核半宽（用于邻域遍历）

    // 第一步：水平方向高斯滤波
    std::vector<double> tempImg(nPixel);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double sum = 0.0;
            // 遍历高斯核邻域，边界像素做越界保护
            for (int k = -halfK; k <= halfK; k++) {
                int nx = min(max(x + k, 0), width - 1); // 防止x坐标越界
                sum += grayImg[y * width + nx] * kernel[k + halfK];
            }
            tempImg[y * width + x] = sum;
        }
    }
    // 第二步：垂直方向高斯滤波
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double sum = 0.0;
            // 遍历高斯核邻域，边界像素做越界保护
            for (int k = -halfK; k <= halfK; k++) {
                int ny = min(max(y + k, 0), height - 1); // 防止y坐标越界
                sum += tempImg[ny * width + x] * kernel[k + halfK];
            }
            smoothImg[y * width + x] = sum;
        }
    }

    // 3. Sobel梯度计算（求边缘强度和方向）
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            // 计算X方向梯度（水平边缘）
            double gx = -smoothImg[(y - 1) * width + (x - 1)] + smoothImg[(y - 1) * width + (x + 1)]
                        - 2 * smoothImg[y * width + (x - 1)] + 2 * smoothImg[y * width + (x + 1)]
                        - smoothImg[(y + 1) * width + (x - 1)] + smoothImg[(y + 1) * width + (x + 1)];
            
            // 计算Y方向梯度（垂直边缘）
            double gy = smoothImg[(y - 1) * width + (x - 1)] + 2 * smoothImg[(y - 1) * width + x] + smoothImg[(y - 1) * width + (x + 1)]
                        - smoothImg[(y + 1) * width + (x - 1)] - 2 * smoothImg[(y + 1) * width + x] - smoothImg[(y + 1) * width + (x + 1)];
            
            // 保存梯度值、计算梯度幅值（边缘强度）
            gradX[y * width + x] = gx;
            gradY[y * width + x] = gy;
            mag[y * width + x] = sqrt(gx * gx + gy * gy);
            
            // 计算梯度方向（弧度转角度），统一到0~180度范围
            double angle = atan2(gy, gx) * 180.0 / 3.14159265;
            if (angle < 0) angle += 180.0;
            
            // 梯度方向量化（简化为4个方向：0/45/90/135度）
            if (angle < 22.5 || angle >= 157.5) dir[y * width + x] = 0;    // 0度（水平）
            else if (angle >= 22.5 && angle < 67.5) dir[y * width + x] = 1;// 45度
            else if (angle >= 67.5 && angle < 112.5) dir[y * width + x] = 2;// 90度（垂直）
            else dir[y * width + x] = 3;                                   // 135度
        }
    }

    // 4. 非极大值抑制（NMS）：细化边缘，只保留梯度方向上的最大值
    std::vector<double> nms(width * height, 0.0); // 非极大值抑制结果
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int d = (int)dir[y * width + x];  // 当前像素梯度方向
            double m = mag[y * width + x];    // 当前像素梯度幅值
            double m1 = 0, m2 = 0;            // 梯度方向上的前后像素幅值
            
            // 根据梯度方向，取对应邻域像素做比较
            if (d == 0) { // 水平方向（0度）：比较左右像素
                m1 = mag[y * width + (x - 1)];
                m2 = mag[y * width + (x + 1)];
            } else if (d == 1) { // 45度方向：比较右上/左下像素
                m1 = mag[(y - 1) * width + (x + 1)];
                m2 = mag[(y + 1) * width + (x - 1)];
            } else if (d == 2) { // 垂直方向（90度）：比较上下像素
                m1 = mag[(y - 1) * width + x];
                m2 = mag[(y + 1) * width + x];
            } else { // 135度方向：比较左上/右下像素
                m1 = mag[(y - 1) * width + (x - 1)];
                m2 = mag[(y + 1) * width + (x + 1)];
            }
            
            // 仅当当前像素幅值是梯度方向上的最大值时保留
            if (m >= m1 && m >= m2)
                nms[y * width + x] = m;
        }
    }

    // 5. 双阈值筛选 + 滞后跟踪（连接弱边缘）
    imgOut.create(width, height, 1, 0.0); // 创建输出图像（单波段）
    double* pOut = imgOut.m_data;         // 输出图像数据指针
    std::vector<int> stack;               // 栈：用于滞后跟踪强边缘连接的弱边缘
    
    // 第一步：初始阈值筛选
    for (int i = 0; i < width * height; i++) {
        if (nms[i] >= highThresh) {
            pOut[i] = 255.0;          // 高于高阈值：强边缘（白色）
            stack.push_back(i);       // 加入栈，用于后续跟踪弱边缘
        } else if (nms[i] >= lowThresh) {
            pOut[i] = 128.0;          // 介于高低阈值之间：弱边缘（标记为128）
        } else {
            pOut[i] = 0.0;            // 低于低阈值：非边缘（黑色）
        }
    }
    
    // 第二步：滞后跟踪（强边缘连接弱边缘）
    while (!stack.empty()) {
        int idx = stack.back();  // 取栈顶强边缘像素索引
        stack.pop_back();
        
        int cx = idx % width;    // 像素X坐标
        int cy = idx / width;    // 像素Y坐标
        
        // 遍历8邻域像素
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                if (dx == 0 && dy == 0) continue; // 跳过自身
                int nx = cx + dx;
                int ny = cy + dy;
                // 邻域像素越界保护
                if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                    int nidx = ny * width + nx;
                    if (pOut[nidx] == 128.0) { // 检测到弱边缘
                        pOut[nidx] = 255.0;    // 升级为强边缘
                        stack.push_back(nidx); // 加入栈，继续跟踪其邻域
                    }
                }
            }
        }
    }
    
    // 第三步：清除未被连接的弱边缘（最终仅保留强边缘）
    for (int i = 0; i < width * height; i++) {
        if (pOut[i] == 128.0) pOut[i] = 0.0;
    }

    return TRUE;
}

// 自动计算推荐阈值 (基于中值)
void CImageProcessingEx::getAutoCannyThresholds(CImageDataset &imgIn, double &outLow, double &outHigh)
{
    if (imgIn.empty()) {
        outLow = 50.0; outHigh = 150.0;
        return;
    }

    int nPixel = imgIn.m_xsize * imgIn.m_ysize;
    std::vector<double> grayImg(nPixel);

    // 1. 转灰度 (复用逻辑)
    if (imgIn.m_rastercount >= 3) {
        double* pR = imgIn.m_data;
        double* pG = imgIn.m_data + nPixel;
        double* pB = imgIn.m_data + 2 * nPixel;
        for (int i = 0; i < nPixel; i++)
            grayImg[i] = 0.30 * pR[i] + 0.59 * pG[i] + 0.11 * pB[i];
    } else {
        double* pIn = imgIn.m_data;
        for (int i = 0; i < nPixel; i++)
            grayImg[i] = pIn[i];
    }

    // 2. 计算中值 (使用直方图加速)
    int hist[256] = {0};
    for (int i = 0; i < nPixel; i++) {
        int val = (int)grayImg[i];
        if (val < 0) val = 0;
        if (val > 255) val = 255;
        hist[val]++;
    }

    int median = 128;
    int count = 0;
    for (int i = 0; i < 256; i++) {
        count += hist[i];
        if (count >= nPixel / 2) {
            median = i;
            break;
        }
    }

    // 3. 应用经验公式
    // 经验上，High=Median*1.5, Low=High*0.4 效果尚可。
    // 如果 Median 很小（暗图），至少保证一个最小值。
    
    double v = (double)median;
    if (v < 50) v = 50; // 防止过暗图像导致阈值太低
    
    outHigh = v * 1.5; 
    outLow = outHigh * 0.4;
}