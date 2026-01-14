// IdealFilterDlg.cpp
#include "stdafx.h"
#include "myAlg.h"
#include "IdealFilterDlg.h"
#include "afxdialogex.h"
#include "FourierTransform.h"
#include "ImageProcessingEx.h"

IMPLEMENT_DYNAMIC(CIdealFilterDlg, CDialogEx)

CIdealFilterDlg::CIdealFilterDlg(CFourierSpectrumDlg* pSpectrumDlg, CWnd* pParent)
    : CDialogEx(IDD_IDEALFILTERDLG, pParent)
    , m_pSpectrumDlg(pSpectrumDlg)
    , m_dCutoffFrequency(0.3)
    , m_nOrder(2)
    , m_filterType(FILTER_IDEAL_HP)
    , m_boostBase(1.2)
{
    TRACE(_T("CIdealFilterDlg 构造函数已调用, pSpectrumDlg = %p\n"), pSpectrumDlg);
    
    if (pSpectrumDlg)
    {
        TRACE(_T("  DFT 数据大小: %d\n"), (int)pSpectrumDlg->m_dftResult.size());
        TRACE(_T("  DFT 尺寸: %dx%d\n"), pSpectrumDlg->m_dftWidth, pSpectrumDlg->m_dftHeight);
    }
}

CIdealFilterDlg::~CIdealFilterDlg()
{
}

void CIdealFilterDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CIdealFilterDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_PREVIEW, &CIdealFilterDlg::OnBnClickedButtonPreview)
    ON_BN_CLICKED(IDOK, &CIdealFilterDlg::OnBnClickedOK)
    ON_WM_HSCROLL()
    ON_BN_CLICKED(IDC_RADIO_IDEAL, &CIdealFilterDlg::OnBnClickedRadioIdeal)
    ON_BN_CLICKED(IDC_RADIO_BUTTERWORTH, &CIdealFilterDlg::OnBnClickedRadioButterworth)
    ON_BN_CLICKED(IDC_RADIO_GAUSSIAN, &CIdealFilterDlg::OnBnClickedRadioGaussian)
END_MESSAGE_MAP()

BOOL CIdealFilterDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    
    SetWindowText(_T("频域高通滤波器"));
    
    // 设置截止频率编辑框的初始值
    CString strValue;
    strValue.Format(_T("%.3f"), m_dCutoffFrequency);
    SetDlgItemText(IDC_EDIT_CUTOFF, strValue);
    SetDlgItemText(IDC_EDIT_CUTOFF2, strValue);
    SetDlgItemText(IDC_EDIT_CUTOFF3, strValue);
    SetDlgItemInt(IDC_EDIT_ORDER, m_nOrder, FALSE);
    
    // 初始化滑块控件
    CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CUTOFF);
    if (pSlider)
    {
        pSlider->SetRange(0, 100);  // 0-1.0 映射到 0-100
        pSlider->SetPos((int)(m_dCutoffFrequency * 100));
        TRACE(_T("滑块已初始化，位置: %d\n"), (int)(m_dCutoffFrequency * 100));
    }
    else
    {
        TRACE(_T("警告：找不到滑块控件 IDC_SLIDER_CUTOFF\n"));
    }
    // 初始化第二个和第三个截止频率滑块
    CSliderCtrl* pSlider2 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CUTOFF2);
    if (pSlider2)
    {
        pSlider2->SetRange(0, 100);
        pSlider2->SetPos((int)(m_dCutoffFrequency * 100));
    }
    else
    {
        TRACE(_T("警告：找不到滑块控件 IDC_SLIDER_CUTOFF2\n"));
    }
    CSliderCtrl* pSlider3 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CUTOFF3);
    if (pSlider3)
    {
        pSlider3->SetRange(0, 100);
        pSlider3->SetPos((int)(m_dCutoffFrequency * 100));
    }
    else
    {
        TRACE(_T("警告：找不到滑块控件 IDC_SLIDER_CUTOFF3\n"));
    }
    // 默认选择理想高通
    CheckRadioButton(IDC_RADIO_IDEAL, IDC_RADIO_GAUSSIAN, IDC_RADIO_IDEAL);
    m_filterType = FILTER_IDEAL_HP;
    UpdateControlEnableState();
    
    TRACE(_T("频域高通滤波器对话框已打开\n"));
    
    return TRUE;
}

void CIdealFilterDlg::OnBnClickedButtonPreview()
{
    // 获取用户输入的截止频率
    CString strValue;
    switch (m_filterType)
    {
    case FILTER_IDEAL_HP:
        GetDlgItemText(IDC_EDIT_CUTOFF, strValue);
        break;
    case FILTER_BUTTERWORTH_HP:
        GetDlgItemText(IDC_EDIT_CUTOFF2, strValue);
        break;
    case FILTER_GAUSSIAN_HP:
        GetDlgItemText(IDC_EDIT_CUTOFF3, strValue);
        break;
    }
    m_dCutoffFrequency = _ttof(strValue);
    // 约束截止频率到合理范围 [0.01, 0.49]
    if (m_dCutoffFrequency < 0.01) m_dCutoffFrequency = 0.01;
    if (m_dCutoffFrequency > 0.49) m_dCutoffFrequency = 0.49;
    m_nOrder = GetDlgItemInt(IDC_EDIT_ORDER, NULL, FALSE);
    // 约束阶数范围 [1, 10]
    if (m_nOrder < 1) m_nOrder = 1;
    if (m_nOrder > 10) m_nOrder = 10;
    
    TRACE(_T("=== OnBnClickedButtonPreview ===\n"));
    TRACE(_T("预览：类型=%d，截止频率 = %.3f，阶数=%d\n"), (int)m_filterType, m_dCutoffFrequency, m_nOrder);
    
    // 应用滤波器
    switch (m_filterType)
    {
    case FILTER_IDEAL_HP:
        ApplyIdealHighPassFilter();
        break;
    case FILTER_BUTTERWORTH_HP:
        ApplyButterworthHighPassFilter();
        break;
    case FILTER_GAUSSIAN_HP:
        ApplyGaussianHighPassFilter();
        break;
    }
    
    // 显示预览结果
    if (!m_filteredDFT.empty() && m_pSpectrumDlg)
    {
        // 为预览显示幅度谱（使用中心化显示更直观）
        std::vector<std::complex<double>> tmp = m_filteredDFT;
        CFourierTransform::ShiftDFTCenter(tmp, m_pSpectrumDlg->m_dftWidth, m_pSpectrumDlg->m_dftHeight);

        CImageDataset spectrumImage;
        if (CFourierTransform::ComputeMagnitudeSpectrum(tmp, spectrumImage,
            m_pSpectrumDlg->m_dftWidth, m_pSpectrumDlg->m_dftHeight, true))
        {
            CString title;
            title.Format(_T("滤波预览：类型=%d, 截止=%.3f"), (int)m_filterType, m_dCutoffFrequency);
            CImageDisplay::show(spectrumImage, this, title, 1, 1, 1, 0);
        }
        else
        {
            AfxMessageBox(_T("预览幅度谱生成失败"));
        }
    }
    else
    {
        AfxMessageBox(_T("预览失败：滤波数据为空"));
    }
}

void CIdealFilterDlg::OnBnClickedOK()
{
    // 获取用户输入的截止频率
    CString strValue;
    switch (m_filterType)
    {
    case FILTER_IDEAL_HP:
        GetDlgItemText(IDC_EDIT_CUTOFF, strValue);
        break;
    case FILTER_BUTTERWORTH_HP:
        GetDlgItemText(IDC_EDIT_CUTOFF2, strValue);
        break;
    case FILTER_GAUSSIAN_HP:
        GetDlgItemText(IDC_EDIT_CUTOFF3, strValue);
        break;
    }
    m_dCutoffFrequency = _ttof(strValue);
    if (m_dCutoffFrequency < 0.01) m_dCutoffFrequency = 0.01;
    if (m_dCutoffFrequency > 0.49) m_dCutoffFrequency = 0.49;
    m_nOrder = GetDlgItemInt(IDC_EDIT_ORDER, NULL, FALSE);
    if (m_nOrder < 1) m_nOrder = 1;
    if (m_nOrder > 10) m_nOrder = 10;
    
    // 应用滤波器
    switch (m_filterType)
    {
    case FILTER_IDEAL_HP:
        ApplyIdealHighPassFilter();
        break;
    case FILTER_BUTTERWORTH_HP:
        ApplyButterworthHighPassFilter();
        break;
    case FILTER_GAUSSIAN_HP:
        ApplyGaussianHighPassFilter();
        break;
    }
    
    // 检查滤波器是否已应用
    if (m_filteredDFT.empty())
    {
        AfxMessageBox(_T("错误：滤波器未正确应用！请调整截止频率或滤波类型。"));
        return;
    }
    
    // 关闭对话框
    CDialogEx::OnOK();
}

void CIdealFilterDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    CSliderCtrl* pSlider1 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CUTOFF);
    CSliderCtrl* pSlider2 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CUTOFF2);
    CSliderCtrl* pSlider3 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CUTOFF3);

    if (pSlider1 && pScrollBar == (CScrollBar*)pSlider1)
    {
        int pos = pSlider1->GetPos();
        m_dCutoffFrequency = pos / 100.0;
        CString str;
        str.Format(_T("%.3f"), m_dCutoffFrequency);
        SetDlgItemText(IDC_EDIT_CUTOFF, str);
        TRACE(_T("滑块1移动：位置=%d，截止频率=%.3f\n"), pos, m_dCutoffFrequency);
    }
    else if (pSlider2 && pScrollBar == (CScrollBar*)pSlider2)
    {
        int pos = pSlider2->GetPos();
        m_dCutoffFrequency = pos / 100.0;
        CString str;
        str.Format(_T("%.3f"), m_dCutoffFrequency);
        SetDlgItemText(IDC_EDIT_CUTOFF2, str);
        TRACE(_T("滑块2移动：位置=%d，截止频率=%.3f\n"), pos, m_dCutoffFrequency);
    }
    else if (pSlider3 && pScrollBar == (CScrollBar*)pSlider3)
    {
        int pos = pSlider3->GetPos();
        m_dCutoffFrequency = pos / 100.0;
        CString str;
        str.Format(_T("%.3f"), m_dCutoffFrequency);
        SetDlgItemText(IDC_EDIT_CUTOFF3, str);
        TRACE(_T("滑块3移动：位置=%d，截止频率=%.3f\n"), pos, m_dCutoffFrequency);
    }
    
    CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CIdealFilterDlg::OnBnClickedRadioIdeal()
{
    m_filterType = FILTER_IDEAL_HP;
    UpdateControlEnableState();
    TRACE(_T("选择 理想高通\n"));
}

void CIdealFilterDlg::OnBnClickedRadioButterworth()
{
    m_filterType = FILTER_BUTTERWORTH_HP;
    UpdateControlEnableState();
    TRACE(_T("选择 Butterworth 高通\n"));
}

void CIdealFilterDlg::OnBnClickedRadioGaussian()
{
    m_filterType = FILTER_GAUSSIAN_HP;
    UpdateControlEnableState();
    TRACE(_T("选择 高斯高通\n"));
}

void CIdealFilterDlg::ApplyIdealHighPassFilter()
{
    TRACE(_T("=== ApplyIdealHighPassFilter 开始 ===\n"));
    TRACE(_T("m_pSpectrumDlg = %p\n"), m_pSpectrumDlg);
    
    if (!m_pSpectrumDlg)
    {
        TRACE(_T("错误：m_pSpectrumDlg 为空指针\n"));
        return;
    }
    
    TRACE(_T("m_pSpectrumDlg->m_dftResult.size() = %d\n"), (int)m_pSpectrumDlg->m_dftResult.size());
    TRACE(_T("m_pSpectrumDlg->m_dftWidth = %d\n"), m_pSpectrumDlg->m_dftWidth);
    TRACE(_T("m_pSpectrumDlg->m_dftHeight = %d\n"), m_pSpectrumDlg->m_dftHeight);
    
    if (m_pSpectrumDlg->m_dftResult.empty())
    {
        TRACE(_T("错误：频谱数据为空\n"));
        return;
    }
    
    int width = m_pSpectrumDlg->m_dftWidth;
    int height = m_pSpectrumDlg->m_dftHeight;
    
    if (width <= 0 || height <= 0)
    {
        TRACE(_T("错误：图像尺寸无效 width=%d, height=%d\n"), width, height);
        return;
    }
    
    // 确保有足够的空间
    int size = width * height;
    m_filteredDFT.resize(size);
    
    TRACE(_T("分配后 m_filteredDFT.size() = %d\n"), (int)m_filteredDFT.size());
    
    int centerX = width / 2;
    int centerY = height / 2;
    double cutoff = m_dCutoffFrequency * min(width, height) / 2.0;
    
    TRACE(_T("应用高通滤波器: 截止频率=%.3f, 尺寸=%dx%d, cutoff=%f\n"), 
          m_dCutoffFrequency, width, height, cutoff);
    
    // 应用理想高通滤波器（在中心化频谱上构造掩模，再反中心化用于IDFT）
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int idx = y * width + x;
            double dx = x - centerX;
            double dy = y - centerY;
            double dist = sqrt(dx * dx + dy * dy);
            
            // 高通滤波：距离中心小的频率被衰减
            // 注意：掩模构造应在中心化频谱上
            // 先从中心化频谱取值
            const auto& dftCentered = m_pSpectrumDlg->GetDFTCenteredResult();
            double H = (dist < cutoff) ? 0.0 : 1.0;
            m_filteredDFT[idx] = dftCentered[idx] * (m_boostBase + H);
        }
    }
    // 反中心化以便后续IDFT使用非中心化数据
    CFourierTransform::ShiftDFTCenter(m_filteredDFT, width, height);
    
    TRACE(_T("高通滤波器已应用成功，过滤后数据大小=%d\n"), (int)m_filteredDFT.size());
    TRACE(_T("=== ApplyIdealHighPassFilter 结束 ===\n"));
}

void CIdealFilterDlg::ApplyButterworthHighPassFilter()
{
    TRACE(_T("=== ApplyButterworthHighPassFilter 开始 ===\n"));
    if (!m_pSpectrumDlg || m_pSpectrumDlg->m_dftResult.empty())
    {
        TRACE(_T("错误：频谱数据不可用\n"));
        return;
    }

    int width = m_pSpectrumDlg->m_dftWidth;
    int height = m_pSpectrumDlg->m_dftHeight;
    int size = width * height;
    m_filteredDFT.resize(size);

    int centerX = width / 2;
    int centerY = height / 2;
    double D0 = m_dCutoffFrequency * min(width, height) / 2.0;
    int n = max(1, m_nOrder);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int idx = y * width + x;
            double dx = x - centerX;
            double dy = y - centerY;
            double D = sqrt(dx * dx + dy * dy);

            double H = 0.0;
            if (D <= 0.0)
            {
                H = 0.0; // 直流分量完全抑制
            }
            else
            {
                double ratio = D0 / D;
                H = 1.0 / (1.0 + pow(ratio, 2 * n));
            }
            // 使用中心化频谱构造掩模并进行高提升混合
            const auto& dftCentered = m_pSpectrumDlg->GetDFTCenteredResult();
            m_filteredDFT[idx] = dftCentered[idx] * (m_boostBase + H);
        }
    }
    // 反中心化供IDFT使用
    CFourierTransform::ShiftDFTCenter(m_filteredDFT, width, height);
    TRACE(_T("Butterworth 高通滤波完成 (D0=%.3f, n=%d)\n"), D0, n);
}

void CIdealFilterDlg::ApplyGaussianHighPassFilter()
{
    TRACE(_T("=== ApplyGaussianHighPassFilter 开始 ===\n"));
    if (!m_pSpectrumDlg || m_pSpectrumDlg->m_dftResult.empty())
    {
        TRACE(_T("错误：频谱数据不可用\n"));
        return;
    }

    int width = m_pSpectrumDlg->m_dftWidth;
    int height = m_pSpectrumDlg->m_dftHeight;
    int size = width * height;
    m_filteredDFT.resize(size);

    int centerX = width / 2;
    int centerY = height / 2;
    double D0 = m_dCutoffFrequency * min(width, height) / 2.0;
    double twoSigma2 = 2.0 * D0 * D0;

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int idx = y * width + x;
            double dx = x - centerX;
            double dy = y - centerY;
            double D2 = dx * dx + dy * dy;

            double H = 1.0 - exp(-(D2) / (twoSigma2));
            const auto& dftCentered = m_pSpectrumDlg->GetDFTCenteredResult();
            m_filteredDFT[idx] = dftCentered[idx] * (m_boostBase + H);
        }
    }
    // 反中心化供IDFT使用
    CFourierTransform::ShiftDFTCenter(m_filteredDFT, width, height);
    TRACE(_T("高斯高通滤波完成 (D0=%.3f)\n"), D0);
}

void CIdealFilterDlg::UpdateControlEnableState()
{
    // 互斥：根据滤波类型启用对应的截止频率控件
    BOOL enableIdeal = (m_filterType == FILTER_IDEAL_HP);
    BOOL enableButter = (m_filterType == FILTER_BUTTERWORTH_HP);
    BOOL enableGaussian = (m_filterType == FILTER_GAUSSIAN_HP);

    // 理想
    GetDlgItem(IDC_EDIT_CUTOFF)->EnableWindow(enableIdeal);
    GetDlgItem(IDC_SLIDER_CUTOFF)->EnableWindow(enableIdeal);
    // Butterworth
    GetDlgItem(IDC_EDIT_CUTOFF2)->EnableWindow(enableButter);
    GetDlgItem(IDC_SLIDER_CUTOFF2)->EnableWindow(enableButter);
    GetDlgItem(IDC_EDIT_ORDER)->EnableWindow(enableButter);
    // Gaussian
    GetDlgItem(IDC_EDIT_CUTOFF3)->EnableWindow(enableGaussian);
    GetDlgItem(IDC_SLIDER_CUTOFF3)->EnableWindow(enableGaussian);
}
