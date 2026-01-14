#include "stdafx.h"
#include "myAlg.h"
#include "FourierInputDlg.h"
#include "afxdialogex.h"
#include "ImageProcessingEx.h"

IMPLEMENT_DYNAMIC(CFourierInputDlg, CDialogEx)

CFourierInputDlg::CFourierInputDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_FOURIERINPUTDLG, pParent)
    , m_selectionMode(0)      // 默认生成图像
    , m_genImageType(0)       // 默认矩形图像
    , m_genWidth(512)         // 默认图像尺寸
    , m_genHeight(512)
    , m_rectWidth(20)         // 默认矩形参数
    , m_rectHeight(40)
    , m_bgColor(0)            // 黑色背景
    , m_rectColor(255)        // 白色矩形
    , m_sineFrequency(8.0)    // 默认正弦波频率
    , m_sinePhase(0.0)        // 默认相位
    , m_checkerSize(16)       // 默认棋盘格大小
{
}

CFourierInputDlg::~CFourierInputDlg()
{
    m_inputImage.clear();
}

void CFourierInputDlg::DoDataExchange(CDataExchange* pDX)
{
	 #ifdef _DEBUG
    if (pDX->m_bSaveAndValidate == FALSE)  // 从控件到变量
    {
        HWND hWnd;
        hWnd = ::GetDlgItem(pDX->m_pDlgWnd->m_hWnd, IDC_RADIO_GENERATE);
        ASSERT(hWnd != NULL);
        hWnd = ::GetDlgItem(pDX->m_pDlgWnd->m_hWnd, IDC_COMBO_GEN_TYPE);
        ASSERT(hWnd != NULL);
        // 检查所有ID...
    }
    #endif
    CDialogEx::DoDataExchange(pDX);
    
    // 选择模式
    DDX_Radio(pDX, IDC_RADIO_GENERATE, m_selectionMode);
    
    // 图像类型
    DDX_CBIndex(pDX, IDC_COMBO_GEN_TYPE, m_genImageType);
    
    // 通用图像参数
    DDX_Text(pDX, IDC_EDIT_GEN_WIDTH, m_genWidth);
    DDX_Text(pDX, IDC_EDIT_GEN_HEIGHT, m_genHeight);
    
    // 矩形图像参数
    DDX_Text(pDX, IDC_EDIT_RECT_WIDTH, m_rectWidth);
    DDX_Text(pDX, IDC_EDIT_RECT_HEIGHT, m_rectHeight);
    DDX_Text(pDX, IDC_EDIT_BG_COLOR, m_bgColor);
    DDX_Text(pDX, IDC_EDIT_RECT_COLOR, m_rectColor);
    
    // 正弦波图像参数
    DDX_Text(pDX, IDC_EDIT_SINE_FREQ, m_sineFrequency);
    DDX_Text(pDX, IDC_EDIT_SINE_PHASE, m_sinePhase);
    
    // 棋盘格图像参数
    DDX_Text(pDX, IDC_EDIT_CHECKER_SIZE, m_checkerSize);
    
    // 读取图像参数
    DDX_Text(pDX, IDC_EDIT_READ_PATH, m_readImagePath);
    
    // 数据验证
    if (pDX->m_bSaveAndValidate)
    {
        // 验证通用图像尺寸
        if (m_genWidth < 32 || m_genWidth > 2048)
        {
            AfxMessageBox(_T("图像宽度必须在32-2048之间！"));
            pDX->Fail();
            return;
        }
        if (m_genHeight < 32 || m_genHeight > 2048)
        {
            AfxMessageBox(_T("图像高度必须在32-2048之间！"));
            pDX->Fail();
            return;
        }
        
        // 根据图像类型验证特定参数
        if (m_selectionMode == 0)  // 生成图像模式
        {
            switch (m_genImageType)
            {
            case 0: // 矩形图像
                if (m_rectWidth <= 0 || m_rectHeight <= 0)
                {
                    AfxMessageBox(_T("矩形宽度和高度必须大于0！"));
                    pDX->Fail();
                    return;
                }
                if (m_bgColor < 0 || m_bgColor > 255 || 
                    m_rectColor < 0 || m_rectColor > 255)
                {
                    AfxMessageBox(_T("颜色值必须在0-255之间！"));
                    pDX->Fail();
                    return;
                }
                break;
                
            case 1: // 正弦波图像
                if (m_sineFrequency <= 0)
                {
                    AfxMessageBox(_T("频率必须大于0！"));
                    pDX->Fail();
                    return;
                }
                break;
                
            case 2: // 棋盘格图像
                if (m_checkerSize <= 0)
                {
                    AfxMessageBox(_T("方格大小必须大于0！"));
                    pDX->Fail();
                    return;
                }
                break;
            }
        }
    }
}

BEGIN_MESSAGE_MAP(CFourierInputDlg, CDialogEx)
    ON_BN_CLICKED(IDC_RADIO_GENERATE, &CFourierInputDlg::OnBnClickedRadioGenerate)
    ON_BN_CLICKED(IDC_RADIO_READ, &CFourierInputDlg::OnBnClickedRadioRead)
    ON_CBN_SELCHANGE(IDC_COMBO_GEN_TYPE, &CFourierInputDlg::OnCbnSelchangeComboGenType)
    ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CFourierInputDlg::OnBnClickedButtonBrowse)
    ON_BN_CLICKED(IDC_BUTTON_PREVIEW, &CFourierInputDlg::OnBnClickedButtonPreview)
END_MESSAGE_MAP()

BOOL CFourierInputDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    
    // 设置对话框标题
    SetWindowText(_T("傅里叶变换 - 输入选择"));
    
    // 初始化下拉框
    CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_GEN_TYPE);
    if (pCombo)
    {
        pCombo->AddString(_T("矩形图像"));
        pCombo->AddString(_T("正弦波图像"));
        pCombo->AddString(_T("棋盘格图像"));
        pCombo->SetCurSel(m_genImageType);
    }
    
    // 设置默认值
    SetDlgItemInt(IDC_EDIT_GEN_WIDTH, m_genWidth);
    SetDlgItemInt(IDC_EDIT_GEN_HEIGHT, m_genHeight);
    SetDlgItemInt(IDC_EDIT_RECT_WIDTH, m_rectWidth);
    SetDlgItemInt(IDC_EDIT_RECT_HEIGHT, m_rectHeight);
    SetDlgItemInt(IDC_EDIT_BG_COLOR, m_bgColor);
    SetDlgItemInt(IDC_EDIT_RECT_COLOR, m_rectColor);
    
    CString strValue;
    strValue.Format(_T("%.2f"), m_sineFrequency);
    SetDlgItemText(IDC_EDIT_SINE_FREQ, strValue);
    
    strValue.Format(_T("%.2f"), m_sinePhase);
    SetDlgItemText(IDC_EDIT_SINE_PHASE, strValue);
    
    SetDlgItemInt(IDC_EDIT_CHECKER_SIZE, m_checkerSize);
    
    // 重要：先更新数据到控件
    UpdateData(FALSE);
    
    // 然后更新UI状态
    UpdateUIState();
    
    return TRUE;
}
void CFourierInputDlg::OnBnClickedRadioGenerate()
{
    TRACE(_T("=== 选择了'生成测试图像' ===\n"));
    
    // 获取当前选择状态
    UpdateData(TRUE);
    
    // 确保选择的是生成图像模式
    m_selectionMode = 0;
    
    // 更新数据到控件
    UpdateData(FALSE);
    
    // 更新UI状态
    UpdateUIState();
}

void CFourierInputDlg::OnBnClickedRadioRead()
{
    TRACE(_T("=== 选择了'读取已有图像' ===\n"));
    
    // 获取当前选择状态
    UpdateData(TRUE);
    
    // 确保选择的是读取图像模式
    m_selectionMode = 1;
    
    // 更新数据到控件
    UpdateData(FALSE);
    
    // 更新UI状态
    UpdateUIState();
}

void CFourierInputDlg::OnCbnSelchangeComboGenType()
{
    CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_GEN_TYPE);
    if (pCombo)
    {
        m_genImageType = pCombo->GetCurSel();
        UpdateParametersVisibility();
    }
}

void CFourierInputDlg::OnBnClickedButtonBrowse()
{
    CFileDialog dlg(TRUE,  // TRUE表示打开文件对话框
                   _T("*.bmp"), 
                   NULL, 
                   OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
                   _T("图像文件 (*.bmp;*.jpg;*.png)|*.bmp;*.jpg;*.png|所有文件 (*.*)|*.*||"), 
                   this);
    
    if (dlg.DoModal() == IDOK)
    {
        m_readImagePath = dlg.GetPathName();
        UpdateData(FALSE);  // 更新控件显示
        
        // 立即尝试读取并显示选择的图像
        CImageDataset tempImage;
        if (CImageIO::read(tempImage, m_readImagePath))
        {
            // 显示图像预览窗口
            CImageDisplay::show(tempImage, this, _T("选择的图像"), 1, 1, 1, 0);
            
            // 同时显示图像信息确认
            CString strMsg;
            strMsg.Format(_T("图像已加载\n\n宽度：%d\n高度：%d\n通道数：%d\n\n点击'下一步'继续分析此图像"),
                         tempImage.m_xsize, tempImage.m_ysize, tempImage.m_rastercount);
            AfxMessageBox(strMsg, MB_ICONINFORMATION);
        }
        else
        {
            AfxMessageBox(_T("无法读取选择的图像文件！"));
            m_readImagePath.Empty();
            UpdateData(FALSE);
        }
    }
}


void CFourierInputDlg::OnBnClickedButtonPreview()
{
    UpdateData(TRUE);  // 从控件获取数据
    
    if (GeneratePreviewImage())
    {
        // 显示生成的预览图像的实际样子
        CImageDisplay::show(m_inputImage, this, _T("生成的图像预览"), 1, 1, 1, 0);
        
        // 同时显示图像参数确认
        CString strMsg;
        strMsg.Format(_T("图像生成成功\n\n宽度：%d\n高度：%d\n通道数：%d\n\n点击'下一步'继续分析此图像"),
                     m_genWidth, m_genHeight, m_inputImage.m_rastercount);
        AfxMessageBox(strMsg, MB_ICONINFORMATION);
    }
    else
    {
        AfxMessageBox(_T("预览图像生成失败！请检查参数。"));
    }
}

void CFourierInputDlg::UpdateUIState()
{
    BOOL enableGenerateControls = (m_selectionMode == 0);  // 0=生成图像（启用）
    BOOL enableReadControls = (m_selectionMode == 1);      // 1=读取图像（启用）
    
    TRACE(_T("UpdateUIState: 选择模式=%d\n"), m_selectionMode);
    
    // === 生成图像相关控件 ===
    // 所有控件都显示，但根据模式启用/禁用
    GetDlgItem(IDC_COMBO_GEN_TYPE)->EnableWindow(enableGenerateControls);
    GetDlgItem(IDC_EDIT_GEN_WIDTH)->EnableWindow(enableGenerateControls);
    GetDlgItem(IDC_EDIT_GEN_HEIGHT)->EnableWindow(enableGenerateControls);
    GetDlgItem(IDC_BUTTON_PREVIEW)->EnableWindow(enableGenerateControls);
    
    // === 读取图像相关控件 ===
    GetDlgItem(IDC_EDIT_READ_PATH)->EnableWindow(enableReadControls);
    GetDlgItem(IDC_BUTTON_BROWSE)->EnableWindow(enableReadControls);
    
    // === 处理参数组 ===
    if (enableGenerateControls)
    {
        // 生成图像模式启用，参数组正常显示/隐藏
        UpdateParametersVisibility();
    }
    else
    {
        // 生成图像模式禁用，隐藏所有参数组
        // 矩形参数组 - 隐藏
        GetDlgItem(IDC_GROUP_RECT)->ShowWindow(FALSE);
        GetDlgItem(IDC_EDIT_RECT_WIDTH)->ShowWindow(FALSE);
        GetDlgItem(IDC_EDIT_RECT_HEIGHT)->ShowWindow(FALSE);
        GetDlgItem(IDC_EDIT_BG_COLOR)->ShowWindow(FALSE);
        GetDlgItem(IDC_EDIT_RECT_COLOR)->ShowWindow(FALSE);
        GetDlgItem(IDC_STATIC_4)->ShowWindow(FALSE);
        GetDlgItem(IDC_STATIC_5)->ShowWindow(FALSE);
        GetDlgItem(IDC_STATIC_6)->ShowWindow(FALSE);
        GetDlgItem(IDC_STATIC_7)->ShowWindow(FALSE);
        
        // 正弦波参数组 - 隐藏
        GetDlgItem(IDC_GROUP_SINE)->ShowWindow(FALSE);
        GetDlgItem(IDC_EDIT_SINE_FREQ)->ShowWindow(FALSE);
        GetDlgItem(IDC_EDIT_SINE_PHASE)->ShowWindow(FALSE);
        GetDlgItem(IDC_STATIC_2)->ShowWindow(FALSE);
        GetDlgItem(IDC_STATIC_3)->ShowWindow(FALSE);
        
        // 棋盘格参数组 - 隐藏
        GetDlgItem(IDC_GROUP_CHECKER)->ShowWindow(FALSE);
        GetDlgItem(IDC_EDIT_CHECKER_SIZE)->ShowWindow(FALSE);
        GetDlgItem(IDC_STATIC_1)->ShowWindow(FALSE);
    }
}


void CFourierInputDlg::UpdateParametersVisibility()
{
    // 只有生成图像模式启用时才更新参数组显示
    if (m_selectionMode != 0)  // 不是生成图像模式
    {
        TRACE(_T("UpdateParametersVisibility: 不是生成图像模式，跳过\n"));
        return;
    }
    
    BOOL showRectParams = (m_genImageType == 0);
    BOOL showSineParams = (m_genImageType == 1);
    BOOL showCheckerParams = (m_genImageType == 2);
    
    TRACE(_T("UpdateParametersVisibility: 图像类型=%d, 显示矩形=%d, 显示正弦=%d, 显示棋盘=%d\n"),
          m_genImageType, showRectParams, showSineParams, showCheckerParams);
    
    // === 矩形参数组 ===
    // 显示/隐藏
    GetDlgItem(IDC_GROUP_RECT)->ShowWindow(showRectParams);
    GetDlgItem(IDC_EDIT_RECT_WIDTH)->ShowWindow(showRectParams);
    GetDlgItem(IDC_EDIT_RECT_HEIGHT)->ShowWindow(showRectParams);
    GetDlgItem(IDC_EDIT_BG_COLOR)->ShowWindow(showRectParams);
    GetDlgItem(IDC_EDIT_RECT_COLOR)->ShowWindow(showRectParams);
    GetDlgItem(IDC_STATIC_4)->ShowWindow(showRectParams);
    GetDlgItem(IDC_STATIC_5)->ShowWindow(showRectParams);
    GetDlgItem(IDC_STATIC_6)->ShowWindow(showRectParams);
    GetDlgItem(IDC_STATIC_7)->ShowWindow(showRectParams);
    
    // 启用（生成图像模式下控件都是启用的）
    GetDlgItem(IDC_EDIT_RECT_WIDTH)->EnableWindow(showRectParams);
    GetDlgItem(IDC_EDIT_RECT_HEIGHT)->EnableWindow(showRectParams);
    GetDlgItem(IDC_EDIT_BG_COLOR)->EnableWindow(showRectParams);
    GetDlgItem(IDC_EDIT_RECT_COLOR)->EnableWindow(showRectParams);
    
    // === 正弦波参数组 ===
    GetDlgItem(IDC_GROUP_SINE)->ShowWindow(showSineParams);
    GetDlgItem(IDC_EDIT_SINE_FREQ)->ShowWindow(showSineParams);
    GetDlgItem(IDC_EDIT_SINE_PHASE)->ShowWindow(showSineParams);
    GetDlgItem(IDC_STATIC_2)->ShowWindow(showSineParams);
    GetDlgItem(IDC_STATIC_3)->ShowWindow(showSineParams);
    
    GetDlgItem(IDC_EDIT_SINE_FREQ)->EnableWindow(showSineParams);
    GetDlgItem(IDC_EDIT_SINE_PHASE)->EnableWindow(showSineParams);
    
    // === 棋盘格参数组 ===
    GetDlgItem(IDC_GROUP_CHECKER)->ShowWindow(showCheckerParams);
    GetDlgItem(IDC_EDIT_CHECKER_SIZE)->ShowWindow(showCheckerParams);
    GetDlgItem(IDC_STATIC_1)->ShowWindow(showCheckerParams);
    
    GetDlgItem(IDC_EDIT_CHECKER_SIZE)->EnableWindow(showCheckerParams);
    
    // 强制重绘，确保显示效果
    Invalidate();
    UpdateWindow();
}
BOOL CFourierInputDlg::GeneratePreviewImage()
{
    // 清理之前的预览图像
    CImageDataset previewImage;
    
    BOOL bSuccess = FALSE;
    
    // 根据图像类型生成相应的图像
    switch (m_genImageType)
    {
    case 0: // 矩形图像
        {
            // 计算矩形位置使其居中
            int rectX = (m_genWidth - m_rectWidth) / 2;
            int rectY = (m_genHeight - m_rectHeight) / 2;
            
            // 使用新实现的矩形图像生成函数
            bSuccess = CImageProcessingEx::createRectangleImage(
                previewImage, 
                m_genWidth, m_genHeight,
                rectX, rectY,
                m_rectWidth, m_rectHeight,
                m_bgColor, m_rectColor);
        }
        break;
        
    case 1: // 正弦波图像
        {
            // 使用新实现的正弦波图像生成函数
            bSuccess = CImageProcessingEx::createSineImage(
                previewImage,
                m_genWidth, m_genHeight,
                m_sineFrequency,
                m_sinePhase);
        }
        break;
        
    case 2: // 棋盘格图像
        {
            // 使用新实现的棋盘格图像生成函数
            bSuccess = CImageProcessingEx::createCheckerboardImage(
                previewImage,
                m_genWidth, m_genHeight,
                m_checkerSize);
        }
        break;
    }
    
    if (bSuccess && !previewImage.empty())
    {
        // 显示预览图像
        CString title;
        switch (m_genImageType)
        {
        case 0:
            title.Format(_T("矩形图像预览 [%dx%d, 矩形:%dx%d]"), 
                        m_genWidth, m_genHeight, m_rectWidth, m_rectHeight);
            break;
        case 1:
            title.Format(_T("正弦波图像预览 [%dx%d, 频率:%.2f, 相位:%.2f]"), 
                        m_genWidth, m_genHeight, m_sineFrequency, m_sinePhase);
            break;
        case 2:
            title.Format(_T("棋盘格图像预览 [%dx%d, 方格:%d]"), 
                        m_genWidth, m_genHeight, m_checkerSize);
            break;
        }
        
        CImageDisplay::show(previewImage, this, title, 1, 1, 1, 0);
        
        // 保存到输入图像供后续使用
        previewImage.duplicate(m_inputImage);
        
        return TRUE;
    }
    
    return FALSE;
}

void CFourierInputDlg::OnOK()
{
    // 从控件获取数据
    UpdateData(TRUE);
    
    // 验证输入
    if (m_selectionMode == 0)  // 生成图像模式
    {
        // 生成图像并保存到 m_inputImage
        CImageDataset tempImage;
        
        BOOL bSuccess = FALSE;
        switch (m_genImageType)
        {
        case 0: // 矩形图像
        {
            int rectX = (m_genWidth - m_rectWidth) / 2;
            int rectY = (m_genHeight - m_rectHeight) / 2;
            
            bSuccess = CImageProcessingEx::createRectangleImage(
                tempImage, 
                m_genWidth, m_genHeight,
                rectX, rectY,
                m_rectWidth, m_rectHeight,
                m_bgColor, m_rectColor);
        }
        break;
        
        case 1: // 正弦波图像
        {
            bSuccess = CImageProcessingEx::createSineImage(
                tempImage,
                m_genWidth, m_genHeight,
                m_sineFrequency,
                m_sinePhase);
        }
        break;
        
        case 2: // 棋盘格图像
        {
            bSuccess = CImageProcessingEx::createCheckerboardImage(
                tempImage,
                m_genWidth, m_genHeight,
                m_checkerSize);
        }
        break;
        }
        
        if (!bSuccess || tempImage.empty())
        {
            AfxMessageBox(_T("生成图像失败！请检查参数。"));
            return;  // 不关闭对话框
        }
        
        // 保存到 m_inputImage
        tempImage.duplicate(m_inputImage);
        
        // 重要：不要在这里显示图像！只是生成图像
        TRACE(_T("生成图像成功，尺寸：%dx%d\n"), 
              m_inputImage.m_xsize, m_inputImage.m_ysize);
        
        // 显示一个详细的提示，显示生成的图像信息
        CString strMsg;
        strMsg.Format(_T("图像生成成功！\n\n图像尺寸：%d × %d\n通道数：%d\n\n点击确定继续到频谱分析"),
                     m_inputImage.m_xsize, m_inputImage.m_ysize, m_inputImage.m_rastercount);
        AfxMessageBox(strMsg, MB_ICONINFORMATION);
    }
    else  // 读取图像模式
    {
        if (m_readImagePath.IsEmpty())
        {
            AfxMessageBox(_T("请选择图像文件！"));
            GetDlgItem(IDC_BUTTON_BROWSE)->SetFocus();
            return;
        }
        
        // 读取图像
        if (FALSE == CImageIO::read(m_inputImage, m_readImagePath))
        {
            AfxMessageBox(_T("读取图像失败！"));
            return;
        }
        
        TRACE(_T("读取图像成功，尺寸：%dx%d，通道数：%d\n"), 
              m_inputImage.m_xsize, m_inputImage.m_ysize, m_inputImage.m_rastercount);
        
        // 显示一个详细的提示，显示读取的图像信息
        CString strMsg;
        strMsg.Format(_T("图像读取成功！\n\n文件名：%s\n图像尺寸：%d × %d\n通道数：%d\n\n点击确定继续到频谱分析"),
                     (LPCTSTR)m_readImagePath, m_inputImage.m_xsize, m_inputImage.m_ysize, m_inputImage.m_rastercount);
        AfxMessageBox(strMsg, MB_ICONINFORMATION);
    }
    
    // 所有验证通过，关闭对话框
    CDialogEx::OnOK();
}