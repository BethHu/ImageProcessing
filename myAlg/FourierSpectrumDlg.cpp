// FourierSpectrumDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "myAlg.h"
#include "FourierSpectrumDlg.h"
#include "afxdialogex.h"
#include "FourierTransform.h"
#include "ImageProcessingEx.h"
#include "InverseDFTDlg.h"  
#include <cmath>
#include <algorithm>

IMPLEMENT_DYNAMIC(CFourierSpectrumDlg, CDialogEx)

CFourierSpectrumDlg::CFourierSpectrumDlg(CImageDataset* pInputImage, CWnd* pParent)
	: CDialogEx(IDD_FOURIERSPECTRUMDLG, pParent),
	m_pInputImage(pInputImage),
	m_bShowAmplitude(FALSE),
	m_bShowPhase(FALSE),
	m_bShowAngular(FALSE),
	m_bShowRadial(FALSE),
	m_dDisplayMin(0.0),
	m_dDisplayMax(255.0),
	m_dLogBase(10.0),
	m_dLogOffset(1.0),
	m_bNormalizeAmp(FALSE),
	m_bLogTransform(FALSE),
	m_bCenter(FALSE),  // FALSE=中心化，TRUE=非中心化（只影响显示）
	m_nPhaseRange(0),
	m_nAngularBins(360),
	m_dAngularRange(360.0),
	m_nRadialBins(100),
	m_dMaxRadius(0.0),
	m_dftWidth(0),
	m_dftHeight(0),
	m_bDFTComputed(FALSE),  // 添加这个
	m_mse(0.0),            // 添加误差初始值
	m_psnr(0.0),
	m_maxError(0.0)
{

}

CFourierSpectrumDlg::~CFourierSpectrumDlg()
{
}

void CFourierSpectrumDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	// 将成员变量与控件关联
	DDX_Text(pDX, IDC_EDIT_DISPLAY_MIN, m_strDisplayMin);
	DDX_Text(pDX, IDC_EDIT_DISPLAY_MAX, m_strDisplayMax);
	DDX_Text(pDX, IDC_EDIT_LOG_BASE, m_strLogBase);
	DDX_Text(pDX, IDC_EDIT_LOG_OFFSET, m_strLogOffset);
	DDX_Check(pDX, IDC_CHECK_CENTER, m_bCenter);
}

BEGIN_MESSAGE_MAP(CFourierSpectrumDlg, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK_AMPLTITUDE, &CFourierSpectrumDlg::OnBnClickedCheckAmpltitude)
	ON_BN_CLICKED(IDC_CHECK_PHASE, &CFourierSpectrumDlg::OnBnClickedCheckPhase)
	ON_BN_CLICKED(IDC_CHECK_ANGULAR, &CFourierSpectrumDlg::OnBnClickedCheckAngular)
	ON_BN_CLICKED(IDC_CHECK_RADIAL, &CFourierSpectrumDlg::OnBnClickedCheckRadial)
	ON_BN_CLICKED(IDC_CHECK_NORMALIZE_AMP, &CFourierSpectrumDlg::OnBnClickedCheckNormalizeAmp)
	ON_BN_CLICKED(IDC_CHECK_LOG_TRANSFORM, &CFourierSpectrumDlg::OnBnClickedCheckLogTransform)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CFourierSpectrumDlg::OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_RADIO_PHASE_RANGE1, &CFourierSpectrumDlg::OnBnClickedRadioPhaseRange1)
	ON_BN_CLICKED(IDC_RADIO_PHASE_RANGE2, &CFourierSpectrumDlg::OnBnClickedRadioPhaseRange2)
	ON_BN_CLICKED(IDC_RADIO_PHASE_RANGE3, &CFourierSpectrumDlg::OnBnClickedRadioPhaseRange3)
	ON_WM_PAINT()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CFourierSpectrumDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置对话框标题
	SetWindowText(_T("傅里叶谱显示设置"));

	// 初始化字符串成员变量
	m_strDisplayMin = _T("0.0");
	m_strDisplayMax = _T("255.0");
	m_strLogBase = _T("10.0");
	m_strLogOffset = _T("1.0");

	// 更新数据交换
	UpdateData(FALSE);

	// Initialize spectrum type checkboxes - 全部不选
	CheckDlgButton(IDC_CHECK_AMPLTITUDE, FALSE);
	CheckDlgButton(IDC_CHECK_PHASE, FALSE);
	CheckDlgButton(IDC_CHECK_ANGULAR, FALSE);
	CheckDlgButton(IDC_CHECK_RADIAL, FALSE);

	// 设置默认显示选项（中心化显示）
	CheckDlgButton(IDC_CHECK_CENTER, FALSE);
	m_bCenter = FALSE;

	// Set default values for amplitude spectrum parameters
	SetDlgItemText(IDC_EDIT_DISPLAY_MIN, _T("0.0"));
	SetDlgItemText(IDC_EDIT_DISPLAY_MAX, _T("255.0"));
	SetDlgItemText(IDC_EDIT_LOG_BASE, _T("10.0"));
	SetDlgItemText(IDC_EDIT_LOG_OFFSET, _T("1.0"));

	// Set default values for angular spectrum parameters
	SetDlgItemInt(IDC_EDIT_ANGULAR_BINS, 360, FALSE);
	SetDlgItemText(IDC_EDIT_ANGULAR_RANGE, _T("360.0"));

	// Set default values for radial spectrum parameters
	SetDlgItemInt(IDC_EDIT_RADIAL_BINS, 100, FALSE);
	SetDlgItemText(IDC_EDIT_MAX_RADIUS, _T("0.0"));

	// Initialize amplitude spectrum checkboxes
	CheckDlgButton(IDC_CHECK_NORMALIZE_AMP, FALSE);
	CheckDlgButton(IDC_CHECK_LOG_TRANSFORM, FALSE);

	// Initialize phase range radio buttons
	CheckRadioButton(IDC_RADIO_PHASE_RANGE1, IDC_RADIO_PHASE_RANGE3, IDC_RADIO_PHASE_RANGE1);

	// 禁用所有参数控件，直到选择相应的谱类型
	UpdateParameterControls();

	// 检查输入图像
	if (m_pInputImage && !m_pInputImage->empty())
	{
		TRACE(_T("输入图像尺寸：%dx%d，通道数：%d\n"), 
			m_pInputImage->m_xsize, 
			m_pInputImage->m_ysize,
			m_pInputImage->m_rastercount);

		// 清空显示图像
		m_displayImage.clear();

		// 禁用应用按钮，直到选择谱类型
		GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(FALSE);
	}
	else
	{
		AfxMessageBox(_T("输入图像为空！"), MB_OK | MB_ICONERROR);
		EndDialog(IDCANCEL);
		return FALSE;
	}

	return TRUE;
}

void CFourierSpectrumDlg::OnDestroy()
{
	// 清理资源
	m_displayImage.clear();
	m_dftResult.clear();
	m_dftResultCentered.clear(); // 添加中心化版本的清理
	CDialogEx::OnDestroy();
}

// 频谱中心化函数（fftshift）
void CFourierSpectrumDlg::fftshift(std::vector<std::complex<double>>& data, int width, int height)
{
	if (data.empty() || width <= 0 || height <= 0)
		return;

	TRACE(_T("执行fftshift，尺寸：%dx%d\n"), width, height);

	// 临时存储
	std::vector<std::complex<double>> temp(data.size());

	// 计算偏移量
	int halfWidth = width / 2;
	int halfHeight = height / 2;

	// 执行fftshift
	for (int y = 0; y < height; y++)
	{
		int targetY = (y + halfHeight) % height;
		for (int x = 0; x < width; x++)
		{
			int targetX = (x + halfWidth) % width;
			int srcIndex = y * width + x;
			int dstIndex = targetY * width + targetX;
			temp[dstIndex] = data[srcIndex];
		}
	}

	// 复制回原数组
	data = temp;
}

void CFourierSpectrumDlg::ComputeDFT()
{
	TRACE(_T("=== ComputeDFT 开始 ===\n"));

	if (!m_pInputImage)
	{
		TRACE(_T("m_pInputImage为空指针\n"));
		return;
	}

	if (m_pInputImage->empty())
	{
		TRACE(_T("输入图像为空\n"));
		return;
	}

	int width = m_pInputImage->m_xsize;
	int height = m_pInputImage->m_ysize;

	TRACE(_T("图像尺寸：%dx%d\n"), width, height);

	// 清空之前的DFT结果
	m_dftResult.clear();
	m_dftResultCentered.clear();
	m_dftWidth = 0;
	m_dftHeight = 0;

	// 使用局部变量
	int tempWidth = width;
	int tempHeight = height;

	TRACE(_T("调用CImageProcessingEx::DFT2D\n"));

	// 调用傅里叶变换
	BOOL result = FALSE;
	try
	{
		result = CImageProcessingEx::DFT2D(*m_pInputImage, m_dftResult, tempWidth, tempHeight);
	}
	catch (...)
	{
		TRACE(_T("调用DFT2D时发生异常\n"));
		MessageBox(_T("计算傅里叶变换时发生异常！"), _T("错误"), MB_OK | MB_ICONERROR);
		return;
	}

	if (!result)
	{
		TRACE(_T("DFT2D返回失败\n"));
		MessageBox(_T("傅里叶变换计算失败！"), _T("错误"), MB_OK | MB_ICONERROR);
		return;
	}

	// 保存尺寸
	m_dftWidth = tempWidth;
	m_dftHeight = tempHeight;

	// 创建中心化版本的DFT结果（用于所有后续计算）
	m_dftResultCentered = m_dftResult; // 复制原始结果
	fftshift(m_dftResultCentered, m_dftWidth, m_dftHeight); // 中心化
	m_bDFTComputed = TRUE; 
	TRACE(_T("DFT完成，尺寸：%dx%d，元素数：%d\n"), 
		m_dftWidth, m_dftHeight, m_dftResult.size());
	
	TRACE(_T("=== ComputeDFT 结束 ===\n"));
}

// 添加拉伸角谱图像的函数

void CFourierSpectrumDlg::UpdateSpectrumDisplay()
{
	TRACE(_T("=== UpdateSpectrumDisplay 开始 ===\n"));

	// 设置等待光标
	CWaitCursor wait;

	// 检查1：DFT结果
	if (m_dftResult.empty())
	{
		TRACE(_T("DFT结果为空，请先计算DFT\n"));
		AfxMessageBox(_T("请先计算傅里叶变换！"), MB_OK | MB_ICONERROR);
		return;
	}

	TRACE(_T("DFT结果有效，尺寸：%dx%d\n"), m_dftWidth, m_dftHeight);

	// 检查2：谱类型选择
	if (!m_bShowAmplitude && !m_bShowPhase && !m_bShowAngular && !m_bShowRadial)
	{
		AfxMessageBox(_T("请至少选择一种谱类型！"));
		TRACE(_T("没有选择任何谱类型\n"));
		return;
	}

	// 遍历所有选择的谱类型，分别显示
	BOOL hasDisplayedAny = FALSE;

	// 1. 显示幅度谱
	if (m_bShowAmplitude)
	{
		TRACE(_T("开始计算幅度谱，中心化选项：%s\n"), 
			m_bCenter ? _T("非中心化") : _T("中心化"));

		CImageDataset spectrumImage;

		// 根据用户选择决定是否归一化
		UpdateData(TRUE);  // 从控件获取最新状态

		if (!spectrumImage.create(m_dftWidth, m_dftHeight, 1, 0.0))
		{
			AfxMessageBox(_T("无法创建幅度谱图像"));
			TRACE(_T("幅度谱图像创建失败\n"));
		}
		else
		{
			// 根据m_bCenter选择使用哪个DFT结果进行显示
			std::vector<std::complex<double>>* pDFTForDisplay = 
				m_bCenter ? &m_dftResult : &m_dftResultCentered;
			
			TRACE(_T("使用%s的DFT结果进行幅度谱显示\n"), 
				m_bCenter ? _T("非中心化") : _T("中心化"));

			// 1. 计算原始幅度
			int totalPixels = m_dftWidth * m_dftHeight;
			for (int i = 0; i < totalPixels; i++)
			{
				spectrumImage.m_data[i] = std::abs((*pDFTForDisplay)[i]);
			}

			TRACE(_T("原始幅度计算完成，总像素数：%d\n"), totalPixels);

			// 2. 应用对数变换（如果需要）
			if (m_bLogTransform)
			{
				m_dLogBase = _ttof(m_strLogBase);
				m_dLogOffset = _ttof(m_strLogOffset);

				for (int i = 0; i < totalPixels; i++)
				{
					double value = spectrumImage.m_data[i];
					if (value + m_dLogOffset > 0)
					{
						if (m_dLogBase == 10.0)
							spectrumImage.m_data[i] = log10(value + m_dLogOffset);
						else if (fabs(m_dLogBase - 2.718281828) < 0.000001)
							spectrumImage.m_data[i] = log(value + m_dLogOffset);
						else
							spectrumImage.m_data[i] = log(value + m_dLogOffset) / log(m_dLogBase);
					}
					else
					{
						spectrumImage.m_data[i] = 0.0;
					}
				}
				TRACE(_T("应用对数变换完成，base=%f, offset=%f\n"), m_dLogBase, m_dLogOffset);
			}

			// 3. 应用归一化（如果需要）
			if (m_bNormalizeAmp)
			{
				m_dDisplayMin = _ttof(m_strDisplayMin);
				m_dDisplayMax = _ttof(m_strDisplayMax);

				// 找到当前最小值和最大值
				double currentMin = spectrumImage.m_data[0];
				double currentMax = spectrumImage.m_data[0];

				for (int i = 1; i < totalPixels; i++)
				{
					if (spectrumImage.m_data[i] < currentMin) currentMin = spectrumImage.m_data[i];
					if (spectrumImage.m_data[i] > currentMax) currentMax = spectrumImage.m_data[i];
				}

				TRACE(_T("归一化前：min=%f, max=%f\n"), currentMin, currentMax);
				TRACE(_T("目标范围：[%f, %f]\n"), m_dDisplayMin, m_dDisplayMax);

				if (fabs(currentMax - currentMin) > 1e-10)
				{
					double scale = (m_dDisplayMax - m_dDisplayMin) / (currentMax - currentMin);
					for (int i = 0; i < totalPixels; i++)
					{
						spectrumImage.m_data[i] = m_dDisplayMin + 
							(spectrumImage.m_data[i] - currentMin) * scale;
					}
				}
				TRACE(_T("归一化完成\n"));
			}
			else
			{
				// 不归一化：自动拉伸到可显示范围
				double currentMin = spectrumImage.m_data[0];
				double currentMax = spectrumImage.m_data[0];

				for (int i = 1; i < totalPixels; i++)
				{
					if (spectrumImage.m_data[i] < currentMin) currentMin = spectrumImage.m_data[i];
					if (spectrumImage.m_data[i] > currentMax) currentMax = spectrumImage.m_data[i];
				}

				if (currentMax > currentMin)
				{
					for (int i = 0; i < totalPixels; i++)
					{
						spectrumImage.m_data[i] =
							(spectrumImage.m_data[i] - currentMin) * 255.0 / (currentMax - currentMin);
					}
				}
			}

			// 显示幅度谱
			CString title;
			title.Format(_T("幅度谱 (%s)"), m_bCenter ? _T("非中心化") : _T("中心化"));
			CImageDisplay::show(spectrumImage, this, title, 1, 1, 1, 0);
			hasDisplayedAny = TRUE;
			TRACE(_T("幅度谱显示成功\n"));
		}
	}

	// 2. 显示相位谱
	if (m_bShowPhase)
	{
		TRACE(_T("开始计算相位谱，范围选项：%d\n"), m_nPhaseRange);

		CImageDataset spectrumImage;

		// 注意：相位谱计算总是使用中心化后的DFT结果
		BOOL success = CFourierTransform::ComputePhaseSpectrum(
			m_dftResultCentered, spectrumImage, m_dftWidth, m_dftHeight, m_nPhaseRange);

		if (success && !spectrumImage.empty())
		{
			// 获取相位范围信息用于显示
			CString rangeText;
			switch (m_nPhaseRange)
			{
			case 0: rangeText = _T("[-π, π]"); break;
			case 1: rangeText = _T("[0, 2π]"); break;
			case 2: rangeText = _T("[-180°, 180°]"); break;
			default: rangeText = _T("[-π, π]"); break;
			}

			CString title;
			title.Format(_T("相位谱 (%s)"), rangeText);

			TRACE(_T("相位谱图像尺寸：%dx%d，标题：%s\n"), 
				spectrumImage.m_xsize, spectrumImage.m_ysize, title);

			// 显示相位谱
			CImageDisplay::show(spectrumImage, this, title, 1, 1, 1, 0);
			hasDisplayedAny = TRUE;
			TRACE(_T("相位谱显示成功\n"));
		}
		else
		{
			AfxMessageBox(_T("相位谱计算失败"));
			TRACE(_T("相位谱计算失败\n"));
		}
	}

	// 3. 显示角谱
	if (m_bShowAngular)
	{
		TRACE(_T("开始计算角谱\n"));

		int bins = GetDlgItemInt(IDC_EDIT_ANGULAR_BINS, NULL, FALSE);
		if (bins <= 0) bins = 360;

		CImageDataset spectrumImage;

		// 注意：角谱计算总是使用中心化后的DFT结果
		CFourierTransform fourier;
		BOOL success = fourier.ComputeAngularSpectrum(
			m_dftResultCentered, spectrumImage, m_dftWidth, m_dftHeight, bins, true);

		if (success && !spectrumImage.empty())
		{
			TRACE(_T("原始角谱尺寸：%dx%d\n"), 
				spectrumImage.m_xsize, spectrumImage.m_ysize);

			// 如果角谱只有1像素高，拉伸它
			

			// 检查角谱数据
			double maxVal = 0;
			double minVal = spectrumImage.m_data[0];
			for (int i = 0; i < min(10, spectrumImage.m_xsize); i++)
			{
				if (spectrumImage.m_data[i] > maxVal) maxVal = spectrumImage.m_data[i];
				if (spectrumImage.m_data[i] < minVal) minVal = spectrumImage.m_data[i];
			}
			TRACE(_T("角谱前10个值范围：[%f, %f]\n"), minVal, maxVal);

			CString title;
			title.Format(_T("角谱 (分bin数: %d)"), bins);

			// 显示角谱
			CImageDisplay::show(spectrumImage, this, title, 1, 1, 1, 0);
			hasDisplayedAny = TRUE;
			TRACE(_T("角谱显示成功\n"));
		}
		else
		{
			AfxMessageBox(_T("角谱计算失败"));
			TRACE(_T("角谱计算失败或结果为空\n"));
		}
	}

	// 4. 显示径向谱
	if (m_bShowRadial)
	{
		TRACE(_T("开始计算径向谱\n"));

		int bins = GetDlgItemInt(IDC_EDIT_RADIAL_BINS, NULL, FALSE);
		if (bins <= 0) bins = 100;

		CImageDataset spectrumImage;

		// 注意：径向谱计算总是使用中心化后的DFT结果
		CFourierTransform fourier;
		BOOL success = fourier.ComputeRadialSpectrum(
			m_dftResultCentered, spectrumImage, m_dftWidth, m_dftHeight, bins, true);

		if (success && !spectrumImage.empty())
		{
			TRACE(_T("径向谱图像尺寸：%dx%d\n"), 
				spectrumImage.m_xsize, spectrumImage.m_ysize);

			// 如果径向谱只有1像素高，拉伸它
			

			// 检查径向谱数据
			double maxVal = 0;
			for (int i = 0; i < min(10, spectrumImage.m_xsize); i++)
			{
				if (spectrumImage.m_data[i] > maxVal)
					maxVal = spectrumImage.m_data[i];
			}
			TRACE(_T("径向谱前10个值最大幅值：%f\n"), maxVal);

			CString title;
			title.Format(_T("径向谱 (分bin数: %d)"), bins);

			// 显示径向谱
			CImageDisplay::show(spectrumImage, this, title, 1, 1, 1, 0);
			hasDisplayedAny = TRUE;
			TRACE(_T("径向谱显示成功\n"));
		}
		else
		{
			AfxMessageBox(_T("径向谱计算失败"));
			TRACE(_T("径向谱计算失败或结果为空\n"));
		}
	}

	if (!hasDisplayedAny)
	{
		AfxMessageBox(_T("所有选择的谱类型计算都失败"));
	}

	TRACE(_T("=== UpdateSpectrumDisplay 完成 ===\n"));
}

void CFourierSpectrumDlg::UpdateParameterControls()
{
	// 幅度谱参数
	BOOL enableAmpParams = m_bShowAmplitude;
	GetDlgItem(IDC_CHECK_NORMALIZE_AMP)->EnableWindow(enableAmpParams);
	GetDlgItem(IDC_CHECK_LOG_TRANSFORM)->EnableWindow(enableAmpParams);
	GetDlgItem(IDC_EDIT_DISPLAY_MIN)->EnableWindow(enableAmpParams);
	GetDlgItem(IDC_EDIT_DISPLAY_MAX)->EnableWindow(enableAmpParams);
	GetDlgItem(IDC_EDIT_LOG_BASE)->EnableWindow(enableAmpParams && m_bLogTransform);
	GetDlgItem(IDC_EDIT_LOG_OFFSET)->EnableWindow(enableAmpParams && m_bLogTransform);

	// 相位谱参数
	BOOL enablePhaseParams = m_bShowPhase;
	GetDlgItem(IDC_RADIO_PHASE_RANGE1)->EnableWindow(enablePhaseParams);
	GetDlgItem(IDC_RADIO_PHASE_RANGE2)->EnableWindow(enablePhaseParams);
	GetDlgItem(IDC_RADIO_PHASE_RANGE3)->EnableWindow(enablePhaseParams);

	// 角谱参数
	BOOL enableAngularParams = m_bShowAngular;
	GetDlgItem(IDC_EDIT_ANGULAR_BINS)->EnableWindow(enableAngularParams);
	GetDlgItem(IDC_EDIT_ANGULAR_RANGE)->EnableWindow(enableAngularParams);

	// 径向谱参数
	BOOL enableRadialParams = m_bShowRadial;
	GetDlgItem(IDC_EDIT_RADIAL_BINS)->EnableWindow(enableRadialParams);
	GetDlgItem(IDC_EDIT_MAX_RADIUS)->EnableWindow(enableRadialParams);

	// 中心化选项（只对幅度谱有效）
	GetDlgItem(IDC_CHECK_CENTER)->EnableWindow(enableAmpParams);

	// 更新应用按钮状态（至少选择一个谱时才启用）
	BOOL enableApplyButton = m_bShowAmplitude || m_bShowPhase || 
		m_bShowAngular || m_bShowRadial;
	GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(enableApplyButton);
}

void CFourierSpectrumDlg::OnBnClickedCheckAmpltitude()
{
	m_bShowAmplitude = IsDlgButtonChecked(IDC_CHECK_AMPLTITUDE);
	UpdateParameterControls();
}

void CFourierSpectrumDlg::OnBnClickedCheckPhase()
{
	m_bShowPhase = IsDlgButtonChecked(IDC_CHECK_PHASE);
	UpdateParameterControls();
}

void CFourierSpectrumDlg::OnBnClickedCheckAngular()
{
	m_bShowAngular = IsDlgButtonChecked(IDC_CHECK_ANGULAR);
	UpdateParameterControls();
}

void CFourierSpectrumDlg::OnBnClickedCheckRadial()
{
	m_bShowRadial = IsDlgButtonChecked(IDC_CHECK_RADIAL);
	UpdateParameterControls();
}

void CFourierSpectrumDlg::OnBnClickedCheckNormalizeAmp()
{
	m_bNormalizeAmp = IsDlgButtonChecked(IDC_CHECK_NORMALIZE_AMP);
	UpdateParameterControls();
}

void CFourierSpectrumDlg::OnBnClickedCheckLogTransform()
{
	m_bLogTransform = IsDlgButtonChecked(IDC_CHECK_LOG_TRANSFORM);
	UpdateParameterControls();
}

void CFourierSpectrumDlg::OnBnClickedButtonApply()
{
	TRACE(_T("=== OnBnClickedButtonApply 开始 ===\n"));

	try
	{
		// 第一步：计算DFT（包含中心化和非中心化两个版本）
		ComputeDFT();

		if (m_dftResult.empty() || m_dftResultCentered.empty())
		{
			AfxMessageBox(_T("傅里叶变换计算结果为空！"));
			TRACE(_T("DFT结果为空\n"));
			return;
		}

		TRACE(_T("DFT计算完成，准备显示频谱\n"));

		// 第二步：显示选择的频谱
		UpdateSpectrumDisplay();
	}
	catch (const std::exception& e)
	{
		CString msg;
		msg.Format(_T("发生C++异常：%s"), CA2T(e.what()));
		AfxMessageBox(msg);
		TRACE(_T("异常：%s\n"), CA2T(e.what()));
	}
	catch (...)
	{
		AfxMessageBox(_T("发生未知异常！"));
		TRACE(_T("未知异常\n"));
	}

	TRACE(_T("=== OnBnClickedButtonApply 结束 ===\n"));
}

void CFourierSpectrumDlg::OnOK()
{
	CInverseDFTDlg dlg(this);
	dlg.DoModal();
}


void CFourierSpectrumDlg::OnBnClickedRadioPhaseRange1()
{
	m_nPhaseRange = 0;  // [-pi, pi]
	TRACE(_T("相位范围设置为：[-π, π]\n"));
}

void CFourierSpectrumDlg::OnBnClickedRadioPhaseRange2()
{
	m_nPhaseRange = 1;  // [0, 2pi]
	TRACE(_T("相位范围设置为：[0, 2π]\n"));
}

void CFourierSpectrumDlg::OnBnClickedRadioPhaseRange3()
{
	m_nPhaseRange = 2;  // [-180, 180] degrees
	TRACE(_T("相位范围设置为：[-180°, 180°]\n"));
}

void CFourierSpectrumDlg::OnPaint()
{
	CPaintDC dc(this);

	CRect clientRect;
	GetClientRect(&clientRect);

	// 清除背景
	dc.FillSolidRect(clientRect, RGB(240, 240, 240));

	
}

// FourierSpectrumDlg.cpp - 修改ComputeInverseDFT函数
int CFourierSpectrumDlg::ComputeInverseDFT()
{
    TRACE(_T("[CFourierSpectrumDlg::ComputeInverseDFT] 开始\n"));
    
    try {
        // 1. 检查数据 - 使用非中心化数据
        if (!m_bDFTComputed || m_dftResult.empty())
        {
            AfxMessageBox(_T("错误：未找到DFT频谱数据。请先计算DFT。"));
            return -1;
        }
        
        // 2. 设置等待光标
        CWaitCursor wait;
        
        // 3. 使用CFourierTransform类进行计算
        CFourierTransform fourierTransform;
        
        // 4. 直接使用非中心化数据 m_dftResult
        // 因为 m_dftResult 是非中心化的，所以不需要反中心化
        BOOL bSuccess = fourierTransform.ComputeInverseDFT(
            m_dftResult,           // 非中心化数据
            m_reconstructedImage,  // 输出图像
            m_dftWidth,            // 宽度
            m_dftHeight,           // 高度
            FALSE);                // FALSE: 数据是非中心化的，不需要反中心化
        
        if (!bSuccess)
        {
            AfxMessageBox(_T("错误：逆DFT计算失败。"));
            return -1;
        }
        
        // 5. 显示成功消息
        CString strMsg;
        strMsg.Format(_T("逆DFT计算成功！\n重建图像尺寸: %d x %d\n使用的数据: 非中心化频谱"),
                     m_dftWidth, m_dftHeight);
        AfxMessageBox(strMsg);
        
        TRACE(_T("[CFourierSpectrumDlg::ComputeInverseDFT] 成功完成\n"));
        return 1;
        
    } 
    catch (const std::exception& e) 
    {
        CString strError;
        strError.Format(_T("计算逆DFT时发生异常：%s"), CString(e.what()));
        AfxMessageBox(strError);
        return -1;
    } 
    catch (...) 
    {
        AfxMessageBox(_T("计算逆DFT时发生未知错误！"));
        return -1;
    }
}

// 修改ShowErrorAnalysis函数
void CFourierSpectrumDlg::ShowErrorAnalysis()
{
    TRACE(_T("[CFourierSpectrumDlg::ShowErrorAnalysis] 开始\n"));
    
    try {
        // 1. 检查数据 - 使用 empty() 成员函数
        if (m_reconstructedImage.empty())  // 正确：调用 empty() 成员函数
        {
            AfxMessageBox(_T("错误：重建图像不存在。请先计算逆DFT。"));
            return;
        }
        
        if (m_pInputImage == nullptr || m_pInputImage->empty())  // 正确
        {
            AfxMessageBox(_T("错误：原始图像数据无效。"));
            return;
        }
        
        // 2. 使用CFourierTransform类计算误差
        CFourierTransform fourierTransform;
        
        BOOL bSuccess = fourierTransform.ComputeErrorAnalysis(
            *m_pInputImage,
            m_reconstructedImage,
            m_mse,
            m_psnr,
            m_maxError,
            m_errorImage);
        
        if (!bSuccess)
        {
            AfxMessageBox(_T("错误：误差分析计算失败。"));
            return;
        }
        
        // 3. 显示结果 - 使用公共成员变量
        CString strResults;
        if (m_psnr == std::numeric_limits<double>::infinity())
        {
            strResults.Format(_T("误差分析结果\n")
                             _T("图像尺寸: %d x %d (通道数: %d)\n")
                             _T("均方误差 (MSE): %.6f\n")
                             _T("峰值信噪比 (PSNR): ∞ dB \n")
                             _T("最大绝对误差: %.2f\n"),
                             m_pInputImage->m_xsize,        // 宽度
                             m_pInputImage->m_ysize,        // 高度
                             m_pInputImage->m_rastercount,  // 通道数
                             m_mse,
                             m_maxError);
        }
        else
        {
            strResults.Format(_T("误差分析结果\n")
                             _T("图像尺寸: %d x %d (通道数: %d)\n")
                             _T("均方误差 (MSE): %.6f\n")
                             _T("峰值信噪比 (PSNR): %.2f dB\n")
                             _T("最大绝对误差: %.2f\n"),
                             m_pInputImage->m_xsize,        // 宽度
                             m_pInputImage->m_ysize,        // 高度
                             m_pInputImage->m_rastercount,  // 通道数
                             m_mse,
                             m_psnr,
                             m_maxError);
        }
      
                
        AfxMessageBox(strResults, MB_OK | MB_ICONINFORMATION);
        
        TRACE(_T("[CFourierSpectrumDlg::ShowErrorAnalysis] 完成\n"));
        TRACE(_T("  尺寸: %dx%dx%d, MSE=%.6f, PSNR=%.2f dB, MaxError=%.2f\n"), 
              m_pInputImage->m_xsize, m_pInputImage->m_ysize, m_pInputImage->m_rastercount,
              m_mse, m_psnr, m_maxError);
        
    } 
    catch (const std::exception& e) 
    {
        CString strError;
        strError.Format(_T("计算误差分析时发生异常：%s"), CString(e.what()));
        AfxMessageBox(strError);
    } 
    catch (...) 
    {
        AfxMessageBox(_T("计算误差分析时发生未知错误！"));
    }
}