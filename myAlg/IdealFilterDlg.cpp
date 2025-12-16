// IdealFilterDlg.cpp
#include "stdafx.h"
#include "myAlg.h"
#include "IdealFilterDlg.h"
#include "afxdialogex.h"

IMPLEMENT_DYNAMIC(CIdealFilterDlg, CDialogEx)

CIdealFilterDlg::CIdealFilterDlg(CFourierSpectrumDlg* pSpectrumDlg, CWnd* pParent)
    : CDialogEx(IDD_IDEALFILTERDLG, pParent)
    , m_pSpectrumDlg(pSpectrumDlg)
    , m_dCutoffFrequency(0.3)
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
END_MESSAGE_MAP()

BOOL CIdealFilterDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    
    SetWindowText(_T("理想高通滤波器"));
    
    // 设置截止频率编辑框的初始值
    CString strValue;
    strValue.Format(_T("%.3f"), m_dCutoffFrequency);
    SetDlgItemText(IDC_EDIT_CUTOFF, strValue);
    
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
    
    TRACE(_T("理想高通滤波器对话框已打开\n"));
    
    return TRUE;
}

void CIdealFilterDlg::OnBnClickedButtonPreview()
{
    // 获取用户输入的截止频率
    CString strValue;
    GetDlgItemText(IDC_EDIT_CUTOFF, strValue);
    m_dCutoffFrequency = _ttof(strValue);
    
    TRACE(_T("=== OnBnClickedButtonPreview ===\n"));
    TRACE(_T("预览：截止频率 = %.3f\n"), m_dCutoffFrequency);
    
    // 应用滤波器
    ApplyIdealHighPassFilter();
    
    // 显示预览结果
    CString strMsg;
    if (m_filteredDFT.empty())
    {
        strMsg.Format(_T("预览失败\n\n截止频率: %.3f\n过滤数据大小: 0\n\n请检查调试输出"), 
                      m_dCutoffFrequency);
    }
    else
    {
        strMsg.Format(_T("预览完成\n\n截止频率: %.3f\n过滤数据大小: %d"), 
                      m_dCutoffFrequency, (int)m_filteredDFT.size());
    }
    AfxMessageBox(strMsg, MB_ICONINFORMATION);
}

void CIdealFilterDlg::OnBnClickedOK()
{
    // 获取用户输入的截止频率
    CString strValue;
    GetDlgItemText(IDC_EDIT_CUTOFF, strValue);
    m_dCutoffFrequency = _ttof(strValue);
    
    // 应用滤波器
    ApplyIdealHighPassFilter();
    
    // 检查滤波器是否已应用
    if (m_filteredDFT.empty())
    {
        AfxMessageBox(_T("错误：滤波器未正确应用！"));
        return;
    }
    
    // 关闭对话框
    CDialogEx::OnOK();
}

void CIdealFilterDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CUTOFF);
    if (pSlider && pScrollBar == (CScrollBar*)pSlider)
    {
        int pos = pSlider->GetPos();
        m_dCutoffFrequency = pos / 100.0;
        
        // 更新编辑框显示值
        CString strValue;
        strValue.Format(_T("%.3f"), m_dCutoffFrequency);
        SetDlgItemText(IDC_EDIT_CUTOFF, strValue);
        
        TRACE(_T("滑块移动：位置=%d，截止频率=%.3f\n"), pos, m_dCutoffFrequency);
    }
    
    CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
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
    
    TRACE(_T("应用理想高通滤波器: 截止频率=%.3f, 尺寸=%dx%d, cutoff=%f\n"), 
          m_dCutoffFrequency, width, height, cutoff);
    
    // 应用理想高通滤波器
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int idx = y * width + x;
            double dx = x - centerX;
            double dy = y - centerY;
            double dist = sqrt(dx * dx + dy * dy);
            
            // 高通滤波：距离中心小的频率被衰减
            if (dist < cutoff)
                m_filteredDFT[idx] = std::complex<double>(0, 0);
            else
                m_filteredDFT[idx] = m_pSpectrumDlg->m_dftResult[idx];
        }
    }
    
    TRACE(_T("理想高通滤波器已应用成功，过滤后数据大小=%d\n"), (int)m_filteredDFT.size());
    TRACE(_T("=== ApplyIdealHighPassFilter 结束 ===\n"));
}
