// OtsuDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "myAlg.h"
#include "OtsuDlg.h"
#include "afxdialogex.h"
#include "ImageProcessingEx.h"

#include <vector>
#include <algorithm>
#include <cmath>


// COtsuDlg 对话框

IMPLEMENT_DYNAMIC(COtsuDlg, CDialogEx)

COtsuDlg::COtsuDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(COtsuDlg::IDD, pParent)
	, m_pInputImage(NULL)
	, m_pOutputImage(NULL)
{
}

COtsuDlg::~COtsuDlg()
{
}

void COtsuDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(COtsuDlg, CDialogEx)
	ON_BN_CLICKED(IDC_RADIO_GRASCALE, &COtsuDlg::OnBnClickedRadioGrayscale)
	ON_BN_CLICKED(IDC_RADIO_RGB, &COtsuDlg::OnBnClickedRadioRgb)
	ON_CBN_SELCHANGE(IDC_COMBO_SMOOTH, &COtsuDlg::OnCbnSelchangeComboSmooth)
	ON_BN_CLICKED(IDC_RADIO_SINGLE_THRESHOLD, &COtsuDlg::OnBnClickedRadioSingleThreshold)
	ON_BN_CLICKED(IDC_RADIO_MULTI_THRESHOLD, &COtsuDlg::OnBnClickedRadioMultiThreshold)
    ON_BN_CLICKED(IDC_BUTTON_PREVIEW, &COtsuDlg::OnBnClickedButtonPreview)
END_MESSAGE_MAP()


BOOL COtsuDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 初始化平滑选项下拉框
	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_SMOOTH);
	if (pCombo)
	{
		pCombo->ResetContent();
		pCombo->AddString(_T("不平滑处理"));
		pCombo->AddString(_T("高斯平滑"));
		pCombo->AddString(_T("中值平滑"));
		pCombo->SetCurSel(0); // 默认不平滑
	}

	// 默认阈值类型为单阈值
	CheckDlgButton(IDC_RADIO_SINGLE_THRESHOLD, BST_CHECKED);
	CheckDlgButton(IDC_RADIO_MULTI_THRESHOLD, BST_UNCHECKED);
	SetDlgItemInt(IDC_EDIT_THRESHOLD_COUNT, 2, FALSE);

	// 默认使用自动灰度
	CheckDlgButton(IDC_RADIO_GRASCALE, BST_CHECKED);
	CheckDlgButton(IDC_RADIO_RGB, BST_UNCHECKED);
	SetDlgItemInt(IDC_EDIT_RGB, 1, FALSE);

	// 将四个单选按钮分成两个互不影响的组：
	// 1) 通道选择组：IDC_RADIO_GRASCALE, IDC_RADIO_RGB
	// 2) 阈值类型组：IDC_RADIO_SINGLE_THRESHOLD, IDC_RADIO_MULTI_THRESHOLD
	CButton* pRadioGray = (CButton*)GetDlgItem(IDC_RADIO_GRASCALE);
	CButton* pRadioSingle = (CButton*)GetDlgItem(IDC_RADIO_SINGLE_THRESHOLD);
	if (pRadioGray)
	{
		pRadioGray->ModifyStyle(0, WS_GROUP);
	}
	if (pRadioSingle)
	{
		pRadioSingle->ModifyStyle(0, WS_GROUP);
	}

	UpdateSmoothControls();
	UpdateThresholdControls();
	UpdateChannelControls();

	return TRUE;
}

void COtsuDlg::OnOK()
{
	if (m_pInputImage == NULL || m_pInputImage->empty())
	{
		AfxMessageBox(_T("请先在主对话框中读取输入图像！"));
		return;
	}

	CImageDataset preImg;
	CImageDataset segImg;
	std::vector<int> thresholds;
	BOOL bMulti = FALSE;
	if (!ComputeOtsuSegmentation(preImg, segImg, thresholds, bMulti))
	{
		// 具体错误在 ComputeOtsuSegmentation 中已弹框提示
		return;
	}

	// 构造灰度直方图图像，并标注阈值
	CImageDataset histImg;
	if (!BuildHistogramImage(preImg, thresholds, histImg))
	{
		AfxMessageBox(_T("构建直方图图像失败！"));
		return;
	}

	// 显示：原图 + 直方图 + 分割结果
	CString titleOrig = _T("原始图像");
	if (m_pInputImage->m_rastercount >= 3)
		CImageDisplay::show(*m_pInputImage, this, titleOrig, 1, 2, 3, 0);
	else
		CImageDisplay::show(*m_pInputImage, this, titleOrig, 1, 1, 1, 0);

	CString titleHist;
	if (!bMulti)
	{
		titleHist.Format(_T("灰度直方图 (Otsu T=%d)"), thresholds[0]);
	}
	else
	{
		CString thrStr;
		for (size_t i = 0; i < thresholds.size(); ++i)
		{
			CString tmp;
			tmp.Format(_T("%d"), thresholds[i]);
			if (!thrStr.IsEmpty()) thrStr += _T(",");
			thrStr += tmp;
		}
		titleHist.Format(_T("灰度直方图 (多阈值: %s)"), thrStr);
	}
	CImageDisplay::show(histImg, this, titleHist, 1, 1, 1, 0);

	CString titleSeg = bMulti ? _T("Otsu 多阈值分割结果") : _T("Otsu 单阈值分割结果");
	CImageDisplay::show(segImg, this, titleSeg, 1, 1, 1, 0);

	// 将分割结果复制到主对话框的输出图像，便于保存
	if (m_pOutputImage)
	{
		if (!segImg.duplicate(*m_pOutputImage))
		{
			TRACE(_T("复制分割结果到输出图像失败\n"));
		}
		else
		{
			AfxMessageBox(_T("分割结果已复制到主窗口输出图像，点击 \"保存\" 可写盘。"));
		}
	}
	// 注意：这里不关闭对话框，保持参数窗口继续打开
}

	BOOL COtsuDlg::ComputeOtsuSegmentation(CImageDataset& preImg,
		CImageDataset& segImg,
		std::vector<int>& thresholds,
		BOOL& bMulti)
	{
		// 读取图像通道选择
		BOOL bAutoGray = (IsDlgButtonChecked(IDC_RADIO_GRASCALE) == BST_CHECKED);
		int nRgbChannel = GetDlgItemInt(IDC_EDIT_RGB, NULL, TRUE);
		if (nRgbChannel < 1) nRgbChannel = 1;
		if (nRgbChannel > 3) nRgbChannel = 3;

		// 读取平滑方式
		int nSmoothMode = 0; // 0: 不平滑, 1: 高斯, 2: 中值
		CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_SMOOTH);
		if (pCombo)
		{
			int sel = pCombo->GetCurSel();
			if (sel == 1) nSmoothMode = 1;
			else if (sel == 2) nSmoothMode = 2;
		}

		int nKernelSize = GetDlgItemInt(IDC_EDIT_WINDOW_SIZE, NULL, TRUE);
		if (nKernelSize <= 0) nKernelSize = 3;
		if (nKernelSize % 2 == 0) nKernelSize += 1; // 卷积核尺寸必须为奇数

		CString strSigma;
		GetDlgItemText(IDC_EDIT_SIGMA, strSigma);
		double dSigma = _tstof(strSigma);
		if (dSigma <= 0.0) dSigma = 1.0;

		// 读取阈值类型
		bMulti = (IsDlgButtonChecked(IDC_RADIO_MULTI_THRESHOLD) == BST_CHECKED);
		int nThreshCount = GetDlgItemInt(IDC_EDIT_THRESHOLD_COUNT, NULL, TRUE);
		if (nThreshCount < 2) nThreshCount = 2;
		if (nThreshCount > 5) nThreshCount = 5;

		// 构造灰度图
		CImageDataset grayImg;
		if (!BuildGrayImage(grayImg, bAutoGray, nRgbChannel))
		{
			AfxMessageBox(_T("灰度图构建失败！"));
			return FALSE;
		}

		// 预处理
		if (!ApplyPreprocess(grayImg, preImg, nSmoothMode, nKernelSize, dSigma))
		{
			AfxMessageBox(_T("图像预处理失败！"));
			return FALSE;
		}

		// 计算阈值
		int singleThresh = 0;
		thresholds.clear();
		BOOL bOk = FALSE;
		if (!bMulti)
		{
			bOk = OtsuSingle(preImg, singleThresh);
			if (bOk) thresholds.push_back(singleThresh);
		}
		else
		{
			bOk = OtsuMulti(preImg, nThreshCount, thresholds);
		}

		if (!bOk || thresholds.empty())
		{
			AfxMessageBox(_T("Otsu 阈值计算失败！"));
			return FALSE;
		}

		// 根据阈值进行分割
		int w = preImg.m_xsize;
		int h = preImg.m_ysize;
		if (!segImg.create(w, h, 1, 0.0))
		{
			AfxMessageBox(_T("创建分割结果图失败！"));
			return FALSE;
		}

		double* pSrc = preImg.m_data;
		double* pDst = segImg.m_data;
		int nPixels = w * h;
		std::sort(thresholds.begin(), thresholds.end());
		int levelCount = (int)thresholds.size() + 1;

		for (int i = 0; i < nPixels; ++i)
		{
			int v = (int)pSrc[i];
			if (v < 0) v = 0;
			if (v > 255) v = 255;

			int cls = 0;
			while (cls < (int)thresholds.size() && v > thresholds[cls])
				++cls;

			double outVal = 0.0;
			if (levelCount > 1)
				outVal = 255.0 * cls / (levelCount - 1);

			pDst[i] = outVal;
		}

		return TRUE;
	}

void COtsuDlg::UpdateChannelControls()
{
	BOOL bIsRgb = FALSE;
	if (m_pInputImage && !m_pInputImage->empty())
		bIsRgb = (m_pInputImage->m_rastercount >= 3);

	CWnd* pGray = GetDlgItem(IDC_RADIO_GRASCALE);
	CWnd* pRgb = GetDlgItem(IDC_RADIO_RGB);
	CWnd* pEdit = GetDlgItem(IDC_EDIT_RGB);
	if (!pGray || !pRgb || !pEdit)
		return;

	if (!bIsRgb)
	{
		pGray->EnableWindow(FALSE);
		pRgb->EnableWindow(FALSE);
		pEdit->EnableWindow(FALSE);
		return;
	}

	// RGB 图像：根据选择自动灰度/手动RGB，控制 EDIT_RGB 是否可编辑
	BOOL bAutoGray = (IsDlgButtonChecked(IDC_RADIO_GRASCALE) == BST_CHECKED);
	if (bAutoGray)
	{
		pGray->EnableWindow(TRUE);
		pRgb->EnableWindow(TRUE);
		pEdit->EnableWindow(FALSE);
	}
	else
	{
		pGray->EnableWindow(TRUE);
		pRgb->EnableWindow(TRUE);
		pEdit->EnableWindow(TRUE);
	}
}

void COtsuDlg::UpdateSmoothControls()
{
	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_SMOOTH);
	if (!pCombo) return;
	int sel = pCombo->GetCurSel();

	CWnd* pKernel = GetDlgItem(IDC_EDIT_WINDOW_SIZE);
	CWnd* pSigma = GetDlgItem(IDC_EDIT_SIGMA);
	if (!pKernel || !pSigma) return;

	if (sel == 0) // 不平滑
	{
		pKernel->EnableWindow(FALSE);
		pSigma->EnableWindow(FALSE);
	}
	else if (sel == 1) // 高斯平滑
	{
		pKernel->EnableWindow(TRUE);
		pSigma->EnableWindow(TRUE);
	}
	else if (sel == 2) // 中值平滑
	{
		pKernel->EnableWindow(TRUE);
		pSigma->EnableWindow(FALSE);
	}
}

void COtsuDlg::UpdateThresholdControls()
{
	BOOL bMulti = (IsDlgButtonChecked(IDC_RADIO_MULTI_THRESHOLD) == BST_CHECKED);
	CWnd* pEditCount = GetDlgItem(IDC_EDIT_THRESHOLD_COUNT);
	if (!pEditCount) return;
	pEditCount->EnableWindow(bMulti);
}

void COtsuDlg::OnBnClickedRadioGrayscale()
{
	UpdateChannelControls();
}

void COtsuDlg::OnBnClickedRadioRgb()
{
	UpdateChannelControls();
}

void COtsuDlg::OnCbnSelchangeComboSmooth()
{
	UpdateSmoothControls();
}

void COtsuDlg::OnBnClickedRadioSingleThreshold()
{
	// 选中单阈值时，只需要禁用数量输入框
	CheckDlgButton(IDC_RADIO_MULTI_THRESHOLD, BST_UNCHECKED);
	UpdateThresholdControls();
}

void COtsuDlg::OnBnClickedRadioMultiThreshold()
{
	CheckDlgButton(IDC_RADIO_MULTI_THRESHOLD, BST_CHECKED);
	UpdateThresholdControls();
}

void COtsuDlg::OnBnClickedButtonPreview()
{
	if (m_pInputImage == NULL || m_pInputImage->empty())
	{
		AfxMessageBox(_T("请先在主对话框中读取输入图像！"));
		return;
	}

	CImageDataset preImg;
	CImageDataset segImg;
	std::vector<int> thresholds;
	BOOL bMulti = FALSE;
	if (!ComputeOtsuSegmentation(preImg, segImg, thresholds, bMulti))
	{
		// 具体错误在 ComputeOtsuSegmentation 中已弹框提示
		return;
	}

	// 仅预览分割结果，窗口挂到当前 Otsu 对话框上
	CString titleSeg = bMulti ? _T("Otsu 多阈值分割预览") : _T("Otsu 单阈值分割预览");
	CImageDisplay::show(segImg, this, titleSeg, 1, 1, 1, 0);
}

BOOL COtsuDlg::BuildGrayImage(CImageDataset& grayImg, bool bAutoGray, int nRgbChannel)
{
	if (!m_pInputImage || m_pInputImage->empty())
		return FALSE;

	int w = m_pInputImage->m_xsize;
	int h = m_pInputImage->m_ysize;
	int bands = m_pInputImage->m_rastercount;

	if (bands <= 1)
	{
		return m_pInputImage->duplicate(grayImg);
	}

	if (!grayImg.create(w, h, 1, 0.0))
		return FALSE;

	int bandSize = w * h;
	double* pSrc = m_pInputImage->m_data;
	double* pDst = grayImg.m_data;

	if (bAutoGray)
	{
		// Y = 0.299R + 0.587G + 0.114B
		for (int i = 0; i < bandSize; ++i)
		{
			double r = pSrc[i];
			double g = pSrc[bandSize + i];
			double b = pSrc[2 * bandSize + i];
			pDst[i] = 0.299 * r + 0.587 * g + 0.114 * b;
		}
	}
	else
	{
		if (nRgbChannel < 1) nRgbChannel = 1;
		if (nRgbChannel > 3) nRgbChannel = 3;
		int offset = (nRgbChannel - 1) * bandSize;
		for (int i = 0; i < bandSize; ++i)
		{
			pDst[i] = pSrc[offset + i];
		}
	}

	return TRUE;
}

BOOL COtsuDlg::ApplyPreprocess(CImageDataset& src, CImageDataset& dst,
	int nSmoothMode, int nKernelSize, double dSigma)
{
	if (src.empty()) return FALSE;

	if (nSmoothMode == 0)
	{
		return src.duplicate(dst);
	}
	else if (nSmoothMode == 1)
	{
		return GaussianSmooth(src, dst, nKernelSize, dSigma);
	}
	else // 中值平滑
	{
		return CImageProcessingEx::medianFilter(src, dst, nKernelSize);
	}
}

BOOL COtsuDlg::GaussianSmooth(CImageDataset& src, CImageDataset& dst,
	int kernelSize, double sigma)
{
	if (src.empty()) return FALSE;
	if (kernelSize <= 0 || sigma <= 0.0) return FALSE;
	if (kernelSize % 2 == 0) kernelSize += 1;

	int w = src.m_xsize;
	int h = src.m_ysize;
	if (!dst.create(w, h, 1, 0.0)) return FALSE;

	int half = kernelSize / 2;
	std::vector<double> kernel(kernelSize * kernelSize);
	double sigma2 = 2.0 * sigma * sigma;
	double sum = 0.0;
	int idx = 0;
	for (int y = -half; y <= half; ++y)
	{
		for (int x = -half; x <= half; ++x)
		{
			double r2 = (double)(x * x + y * y);
			double v = std::exp(-r2 / sigma2);
			kernel[idx++] = v;
			sum += v;
		}
	}
	for (size_t i = 0; i < kernel.size(); ++i)
	{
		kernel[i] /= sum;
	}

	double* pSrc = src.m_data;
	double* pDst = dst.m_data;

	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			double acc = 0.0;
			idx = 0;
			for (int ky = -half; ky <= half; ++ky)
			{
				int yy = y + ky;
				if (yy < 0) yy = 0;
				if (yy >= h) yy = h - 1;
				for (int kx = -half; kx <= half; ++kx)
				{
					int xx = x + kx;
					if (xx < 0) xx = 0;
					if (xx >= w) xx = w - 1;
					acc += pSrc[yy * w + xx] * kernel[idx++];
				}
			}
			pDst[y * w + x] = acc;
		}
	}

	return TRUE;
}

BOOL COtsuDlg::OtsuSingle(CImageDataset& img, int& threshold)
{
	if (img.empty()) return FALSE;

	const int LEVEL = 256;
	std::vector<double> hist(LEVEL, 0.0);
	int w = img.m_xsize;
	int h = img.m_ysize;
	int n = w * h;
	double* p = img.m_data;
	for (int i = 0; i < n; ++i)
	{
		int v = (int)p[i];
		if (v < 0) v = 0;
		if (v > 255) v = 255;
		hist[v] += 1.0;
	}

	double sum = 0.0;
	for (int t = 0; t < LEVEL; ++t)
	{
		sum += t * hist[t];
	}

	double sumB = 0.0;
	double wB = 0.0;
	double wF = 0.0;
	double varMax = 0.0;
	int bestT = 0;
	for (int t = 0; t < LEVEL; ++t)
	{
		wB += hist[t];
		if (wB == 0) continue;
		wF = n - wB;
		if (wF == 0) break;

		sumB += t * hist[t];
		double mB = sumB / wB;
		double mF = (sum - sumB) / wF;
		double varBetween = wB * wF * (mB - mF) * (mB - mF);
		if (varBetween > varMax)
		{
			varMax = varBetween;
			bestT = t;
		}
	}

	threshold = bestT;
	return TRUE;
}

BOOL COtsuDlg::OtsuMulti(CImageDataset& img, int levelCount, std::vector<int>& thresholds)
{
	if (img.empty()) return FALSE;
	if (levelCount < 2) levelCount = 2;
	if (levelCount > 5) levelCount = 5;

	const int L = 256;
	std::vector<double> hist(L, 0.0);
	int w = img.m_xsize;
	int h = img.m_ysize;
	int n = w * h;
	double* p = img.m_data;
	for (int i = 0; i < n; ++i)
	{
		int v = (int)p[i];
		if (v < 0) v = 0;
		if (v > 255) v = 255;
		hist[v] += 1.0;
	}

	std::vector<double> prob(L, 0.0);
	for (int i = 0; i < L; ++i)
	{
		prob[i] = hist[i] / (double)n;
	}

	std::vector<double> omega(L, 0.0);
	std::vector<double> mu(L, 0.0);
	omega[0] = prob[0];
	mu[0] = 0.0 * prob[0];
	for (int i = 1; i < L; ++i)
	{
		omega[i] = omega[i - 1] + prob[i];
		mu[i] = mu[i - 1] + i * prob[i];
	}
	double muT = mu[L - 1];

	// 预计算 [a,b] 区间的类间方差项
	std::vector<std::vector<double> > sigma(L, std::vector<double>(L, 0.0));
	for (int a = 0; a < L; ++a)
	{
		for (int b = a; b < L; ++b)
		{
			double w_ab = omega[b] - (a > 0 ? omega[a - 1] : 0.0);
			if (w_ab <= 0.0) continue;
			double mu_ab = (mu[b] - (a > 0 ? mu[a - 1] : 0.0)) / w_ab;
			sigma[a][b] = w_ab * (mu_ab - muT) * (mu_ab - muT);
		}
	}

	int m = levelCount;
	// DP[k][i]: 前 i 级灰度被划分为 k 个类时的最大类间方差
	std::vector<std::vector<double> > DP(m + 1, std::vector<double>(L, 0.0));
	std::vector<std::vector<int> > PT(m + 1, std::vector<int>(L, 0));

	// k=1 时，只是一个类 [0..i]
	for (int i = 0; i < L; ++i)
	{
		DP[1][i] = sigma[0][i];
		PT[1][i] = 0;
	}

	for (int k = 2; k <= m; ++k)
	{
		for (int i = 0; i < L; ++i)
		{
			DP[k][i] = 0.0;
			PT[k][i] = 0;
			for (int j = k - 2; j < i; ++j)
			{
				double val = DP[k - 1][j] + sigma[j + 1][i];
				if (val > DP[k][i])
				{
					DP[k][i] = val;
					PT[k][i] = j + 1;
				}
			}
		}
	}

	thresholds.clear();
	thresholds.resize(m - 1);
	int idx = L - 1;
	for (int k = m; k >= 2; --k)
	{
		int t = PT[k][idx];
		thresholds[k - 2] = t - 1; // 阈值在区间边界-1
		idx = t - 1;
	}

	std::sort(thresholds.begin(), thresholds.end());
	return TRUE;
}

BOOL COtsuDlg::BuildHistogramImage(CImageDataset& img,
	const std::vector<int>& thresholds, CImageDataset& histImg)
{
	if (img.empty()) return FALSE;
	const int LEVEL = 256;
	std::vector<double> hist(LEVEL, 0.0);
	int w = img.m_xsize;
	int h = img.m_ysize;
	int n = w * h;
	double* p = img.m_data;
	for (int i = 0; i < n; ++i)
	{
		int v = (int)p[i];
		if (v < 0) v = 0;
		if (v > 255) v = 255;
		hist[v] += 1.0;
	}

	double maxCount = 0.0;
	for (int i = 0; i < LEVEL; ++i)
	{
		if (hist[i] > maxCount) maxCount = hist[i];
	}
	if (maxCount <= 0.0) maxCount = 1.0;

	int histW = LEVEL;
	int histH = 200;
	if (!histImg.create(histW, histH, 1, 0.0))
		return FALSE;

	double* pHist = histImg.m_data;

	// 绘制柱状图
	for (int x = 0; x < LEVEL; ++x)
	{
		int barHeight = (int)(hist[x] * (histH - 1) / maxCount + 0.5);
		if (barHeight < 0) barHeight = 0;
		if (barHeight >= histH) barHeight = histH - 1;

		for (int y = 0; y < barHeight; ++y)
		{
			int yy = histH - 1 - y;
			pHist[yy * histW + x] = 180.0; // 柱子灰度
		}
	}

	// 在阈值位置画竖线（更亮）
	for (size_t k = 0; k < thresholds.size(); ++k)
	{
		int t = thresholds[k];
		if (t < 0 || t >= LEVEL) continue;
		for (int y = 0; y < histH; ++y)
		{
			pHist[y * histW + t] = 255.0;
		}
	}

	return TRUE;
}


// COtsuDlg 消息处理程序
