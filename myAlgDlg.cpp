
// myAlgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myAlg.h"
#include "myAlgDlg.h"
#include "afxdialogex.h"
#include "FourierInputDlg.h"
#include "FourierSpectrumDlg.h"
#include "WedgeFilterDlg.h"
#include "ImageProcessingEx.h"
#include "PcaPParamDlg.h"
#include "RgbIhsParamDlg.h"
#include "CannyParamDlg.h"
#include "PcaChartDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CmyAlgDlg dialog




CmyAlgDlg::CmyAlgDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CmyAlgDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_strImgInput = _T("");
	m_strImgOutput = _T("");
}

void CmyAlgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SRCFILE, m_strImgInput);
	DDX_Text(pDX, IDC_EDIT_DESFILE, m_strImgOutput);
}

BEGIN_MESSAGE_MAP(CmyAlgDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_BUTTON_READ, &CmyAlgDlg::OnClickedButtonRead)
	ON_BN_CLICKED(IDC_BUTTON_WRITE, &CmyAlgDlg::OnClickedButtonWrite)
	ON_BN_CLICKED(IDC_BUTTON_HIST, &CmyAlgDlg::OnClickedButtonHist)
	ON_BN_CLICKED(IDC_BUTTON_hismatch, &CmyAlgDlg::OnClickedButtonHismatch)
	ON_BN_CLICKED(IDC_BUTTON_median, &CmyAlgDlg::OnClickedButtonMedian)
	ON_BN_CLICKED(IDC_BUTTON_BilateralFilter, &CmyAlgDlg::OnClickedButtonBilateralfilter)
	ON_BN_CLICKED(IDC_BUTTON_Laplacian, &CmyAlgDlg::OnClickedButtonLaplacian)
	ON_BN_CLICKED(IDC_BUTTON_Fourier, &CmyAlgDlg::OnClickedButtonFourier)
	ON_BN_CLICKED(IDC_BUTTON_IDEALHIGHPASS, &CmyAlgDlg::OnClickedButtonIdealhighpass)
    ON_BN_CLICKED(IDC_BUTTON1, &CmyAlgDlg::OnClickedButtonWedge)
    ON_BN_CLICKED(IDC_BUTTON_OSTU, &CmyAlgDlg::OnClickedButtonOtsu)
    ON_BN_CLICKED(IDC_BUTTON_PCA, &CmyAlgDlg::OnClickedButtonPca)
    ON_BN_CLICKED(IDC_BUTTON_RGBIHS, &CmyAlgDlg::OnClickedButtonRgbihs)
    ON_BN_CLICKED(IDC_BUTTON_CANNY, &CmyAlgDlg::OnClickedButtonCanny)
END_MESSAGE_MAP()


// CmyAlgDlg message handlers

BOOL CmyAlgDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CmyAlgDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CmyAlgDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CmyAlgDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CmyAlgDlg::OnClickedButtonRead()
{
	CFileDialog dlg(TRUE);//TRUE为OPEN对话框，FALSE为SAVE AS对话框
	if(dlg.DoModal()==IDOK)
	{
		if(FALSE == CImageIO::read(imgIn,dlg.GetPathName()))
		{
			AfxMessageBox("读取图像失败！");
		}
		else
		{
			m_strImgInput=dlg.GetPathName();
			UpdateData(FALSE);
			
			if(imgIn.m_rastercount>=3)
				CImageDisplay::show(imgIn,this,dlg.GetFileName(),1,2,3,0);
			else
				CImageDisplay::show(imgIn,this,dlg.GetFileName(),1,1,1,0);
		}
	}
}


void CmyAlgDlg::OnClickedButtonWrite()
{
	CFileDialog dlg(FALSE);
	if(dlg.DoModal()==IDOK)
	{
		if(FALSE == CImageIO::write(imgOut,dlg.GetPathName()))
		{
			AfxMessageBox("保存图像失败！");
		}
		else
		{
			m_strImgOutput=dlg.GetPathName();
			UpdateData(FALSE);
			
			if(imgOut.m_rastercount>=3)
				CImageDisplay::show(imgOut,this,dlg.GetFileName(),1,2,3,0);
			else
				CImageDisplay::show(imgOut,this,dlg.GetFileName(),1,1,1,0);
		}
	}
}

# include"ImageProcessingEx.h"

void CmyAlgDlg::OnClickedButtonHist()
{
	int bSuccess=CImageProcessingEx::histeq(imgIn,imgOut);
	if(!bSuccess)
	{
		AfxMessageBox("处理图像失败！");
		return;
	}
	if(imgOut.m_rastercount>=3)
		CImageDisplay::show(imgOut,this,"直方图均衡化",1,2,3,0);
	else
		CImageDisplay::show(imgOut,this,"直方图均衡化",1,1,1,0);
}


void CmyAlgDlg::OnClickedButtonHismatch()
{
    // 1. 检查是否有待匹配图像
    if (imgIn.empty())
    {
        AfxMessageBox(_T("请先使用\"Read\"按钮读取待匹配图像！"));
        return;
    }
    
    // 2. 直接弹出文件选择对话框
    CFileDialog dlg(TRUE,  // TRUE表示打开文件对话框
                   _T("*.bmp"), 
                   NULL, 
                   OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
                   _T("Image Files (*.bmp;*.jpg;*.png)|*.bmp;*.jpg;*.png|All Files (*.*)|*.*||"), 
                   this);
    
    dlg.m_ofn.lpstrTitle = _T("选择参考图像");  // 设置对话框标题
    
    if (dlg.DoModal() == IDOK)
    {
        CString refImagePath = dlg.GetPathName();
        CString refFileName = dlg.GetFileName();  // 获取文件名（不含路径）
        
        // 3. 加载参考图像
        CImageDataset refImage;
        if (FALSE == CImageIO::read(refImage, refImagePath))
        {
            AfxMessageBox(_T("读取参考图像失败！"));
            return;
        }
        
        // 4. 检查参考图像是否有效
        if (refImage.empty())
        {
            AfxMessageBox(_T("参考图像无效！"));
            return;
        }
        
        // ================ 新增：显示参考图像 ================
        AfxMessageBox(_T("正在显示参考图像，点击确定后开始匹配..."));
        
        // 显示参考图像（和你的显示方式一致）
        if (refImage.m_rastercount >= 3)
            CImageDisplay::show(refImage, this, _T("参考图像 - ") + refFileName, 1, 2, 3, 0);
        else
            CImageDisplay::show(refImage, this, _T("参考图像 - ") + refFileName, 1, 1, 1, 0);
        // ==================================================
        
        // 5. 进行直方图匹配
        BOOL bSuccess = CImageProcessingEx::histMatch(imgIn, refImage, imgOut);
        
        if (!bSuccess)
        {
            AfxMessageBox(_T("直方图匹配失败！"));
            return;
        }
        
        // 6. 显示匹配结果
        if (imgOut.m_rastercount >= 3)
            CImageDisplay::show(imgOut, this, _T("直方图匹配结果"), 1, 2, 3, 0);
        else
            CImageDisplay::show(imgOut, this, _T("直方图匹配结果"), 1, 1, 1, 0);
        
        // 7. 提示用户保存结果
        AfxMessageBox(_T("直方图匹配完成！请使用\"Write\"按钮保存结果。"));
    }
}

void CmyAlgDlg::OnClickedButtonMedian()
{
    // TODO: 在此添加控件通知处理程序代码
    
    // 1. 检查是否有输入图像
    if (imgIn.empty())
    {
        AfxMessageBox(_T("请先使用\"Read\"按钮读取图像！"));
        return;
    }
    
    // 2. 弹出中值滤波参数对话框
    CMedianFilterDlg dlg;
    if (dlg.DoModal() != IDOK)
    {
        return;  // 用户取消
    }
    
    int windowSize = dlg.m_windowSize;
    
    // 3. 验证窗口大小（必须是奇数）
    if (windowSize % 2 == 0)
    {
        AfxMessageBox(_T("窗口大小必须是奇数！"));
        return;
    }
    
    // 4. 执行中值滤波
    BOOL bSuccess = CImageProcessingEx::medianFilter(imgIn, imgOut, windowSize);
    
    if (!bSuccess)
    {
        AfxMessageBox(_T("中值滤波失败！"));
        return;
    }
    
    // 5. 显示结果
    if (imgOut.m_rastercount >= 3)
        CImageDisplay::show(imgOut, this, _T("中值滤波结果"), 1, 2, 3, 0);
    else
        CImageDisplay::show(imgOut, this, _T("中值滤波结果"), 1, 1, 1, 0);
}

void CmyAlgDlg::OnClickedButtonBilateralfilter()
{
	// 1. 检查是否有输入图像
    if (imgIn.empty())
    {
        AfxMessageBox(_T("请先使用\"读取\"按钮输入图像！"));
        return;
    }
    
    // 2. 弹出参数对话框
    BilateralFilter dlg;
    if (dlg.DoModal() != IDOK)
    {
        return;  // 用户取消
    }
    
    // 3. 执行双边滤波
    BOOL bSuccess = CImageProcessingEx::bilateralFilter(imgIn, imgOut, 
                                                       dlg.m_sigmaS, 
                                                       dlg.m_sigmaR, 
                                                       dlg.m_windowSize);
    
    if (!bSuccess)
    {
        AfxMessageBox(_T("双边滤波失败！"));
        return;
    }
    
    // 4. 显示结果
    CString title;
    title.Format(_T("双边滤波 [σs=%.1f, σr=%.2f, 窗口=%d]"), 
                 dlg.m_sigmaS, dlg.m_sigmaR, dlg.m_windowSize);
    
    if (imgOut.m_rastercount >= 3)
        CImageDisplay::show(imgOut, this, title, 1, 2, 3, 0);
    else
        CImageDisplay::show(imgOut, this, title, 1, 1, 1, 0);
    
    // 5. 提示保存
    AfxMessageBox(_T("双边滤波完成！请使用\"保存\"按钮保存结果。"));
}


void CmyAlgDlg::OnClickedButtonLaplacian()
{
    // 1. 检查是否有输入图像
    if (imgIn.empty())
    {
        AfxMessageBox(_T("请先使用\"读取\"按钮输入图像！"));
        return;
    }
    
    // 2. 可以选择是否弹出参数对话框（可选）
    // 这里用简单版本，不需要参数
    
    // 3. 执行拉普拉斯锐化
    BOOL bSuccess = CImageProcessingEx::laplacianSharpen(imgIn, imgOut);
    // 或者使用增强版：CImageProcessingEx::laplacianSharpenEx(imgIn, imgOut, 0);
    
    if (!bSuccess)
    {
        AfxMessageBox(_T("拉普拉斯锐化失败！"));
        return;
    }
    
    // 4. 显示结果
    CString title = _T("拉普拉斯锐化结果");
    
    if (imgOut.m_rastercount >= 3)
        CImageDisplay::show(imgOut, this, title, 1, 2, 3, 0);
    else
        CImageDisplay::show(imgOut, this, title, 1, 1, 1, 0);
    
    // 5. 提示保存
    AfxMessageBox(_T("拉普拉斯锐化完成！边缘和细节已增强。"));
}

// ===== 新增：主成分分析（PCA）入口 =====
void CmyAlgDlg::OnClickedButtonPca()
{

    if (imgIn.empty() || imgIn.m_rastercount < 2)
    {
        AfxMessageBox(_T("请先使用 \"Read\" 按钮读取多波段实验数据（如 lanier.img，波段数≥2）！"));
        return;
    }
    int nBandCount = imgIn.m_rastercount;

    CPcaPParamDlg dlgP(this);
    dlgP.SetBandCount(nBandCount);
    int nPcaDlgRet = dlgP.DoModal();
    if (nPcaDlgRet != IDOK)
        return;

    CImageDataset imgPCA;
    CMatrix matT;
    CMatrix matEigValues;
    CMatrix matMean;
    if (!CImageProcessingEx::pcaForward(imgIn, imgPCA, matT, matEigValues, matMean))
    {
        AfxMessageBox(_T("PCA 正变换失败！"));
        return;
    }

    // 模式0：仅正变换（保存/显示主成分影像）
    if (dlgP.m_bIsSingleMode == 0)
    {
        CImageDataset imgPCADisplay;
        if (CImageProcessingEx::linearStretch(imgPCA, imgPCADisplay))
        {
            if (imgPCADisplay.m_rastercount >= 3)
                CImageDisplay::show(imgPCADisplay, this, _T("主成分影像（前3分量）"), 1, 2, 3, 0);
            else
                CImageDisplay::show(imgPCADisplay, this, _T("主成分影像"), 1, 1, 1, 0);
        }

        if (IDYES == AfxMessageBox(_T("PCA 正变换成功！是否保存主成分影像？"), MB_YESNO))
        {
            CFileDialog dlgSave(FALSE, _T("tif"), _T("PCA_Result.tif"), OFN_OVERWRITEPROMPT,
                _T("GeoTIFF文件 (*.tif)|*.tif|所有文件 (*.*)|*.*|"));
            if (dlgSave.DoModal() == IDOK)
            {
                if (CImageIO::write(imgPCA, dlgSave.GetPathName()))
                    AfxMessageBox(_T("主成分影像保存成功！"));
                else
                    AfxMessageBox(_T("保存失败！"));
            }
        }
    }
    // 模式1：单 p 值重建
    else if (dlgP.m_bIsSingleMode == 1)
    {
        int nCustomP = dlgP.m_nSingleP;
        CImageDataset imgRecon;
        if (!CImageProcessingEx::pcaBackward(imgPCA, matT, matMean, nCustomP, imgRecon))
        {
            AfxMessageBox(_T("PCA 反变换失败！"));
            return;
        }

        CString strDefaultFileName;
        strDefaultFileName.Format(_T("PCA_Recon_p%d"), nCustomP);
        CFileDialog dlgSave(FALSE, _T("tif"), strDefaultFileName, OFN_OVERWRITEPROMPT,
            _T("GeoTIFF文件 (*.tif)|*.tif|所有文件 (*.*)|*.*|"));
        if (dlgSave.DoModal() == IDOK)
        {
            if (CImageIO::write(imgRecon, dlgSave.GetPathName()))
                AfxMessageBox(_T("PCA 重建影像保存成功！"));
            else
                AfxMessageBox(_T("保存重建影像失败！"));
        }

        double dRmse = CImageProcessingEx::calcPcaRmse(imgIn, imgRecon);
        CString strRmseInfo;
        strRmseInfo.Format(_T("PCA 重建完成（p=%d）！\n原始影像 vs 重建影像\nRMSE=%.6f"), nCustomP, dRmse);
        AfxMessageBox(strRmseInfo);

        CString strReconTitle;
        strReconTitle.Format(_T("PCA 重建影像（p=%d）"), nCustomP);
        CImageDisplay::show(imgRecon, this, strReconTitle, 1, 2, 3, 0);
        CImageDisplay::show(imgIn, this, _T("原始影像"), 1, 2, 3, 0);
    }
    // 模式2：多 p 值误差分析
    else
    {
        std::vector<int> vecMultiP = dlgP.m_vecMultiP;
        std::vector<double> vecRmseResults;
        std::vector<int> vecPResults;
        CString strErrorResult = _T("多 p 值 PCA 重建误差（RMSE）结果：\n");

        for (size_t i = 0; i < vecMultiP.size(); i++)
        {
            int nP = vecMultiP[i];
            CImageDataset imgRecon;
            if (CImageProcessingEx::pcaBackward(imgPCA, matT, matMean, nP, imgRecon))
            {
                double dRmse = CImageProcessingEx::calcPcaRmse(imgIn, imgRecon);
                CString strLine;
                strLine.Format(_T("p=%d → RMSE=%.6f\n"), nP, dRmse);
                strErrorResult += strLine;
                vecPResults.push_back(nP);
                vecRmseResults.push_back(dRmse);
            }
            else
            {
                CString strP;
                strP.Format(_T("p=%d"), nP);
                strErrorResult += strP + _T(" → 计算失败\n");
            }
        }

        AfxMessageBox(strErrorResult);

        if (!vecPResults.empty())
        {
            CPcaChartDlg dlgChart(this);
            dlgChart.SetData(vecPResults, vecRmseResults);
            dlgChart.DoModal();
        }

        CFileDialog dlgSaveTxt(FALSE, _T("txt"), _T("PCA_MultiP_Error.txt"), OFN_OVERWRITEPROMPT,
            _T("文本文件 (*.txt)|*.txt|所有文件 (*.*)|*.*|"));
        if (dlgSaveTxt.DoModal() == IDOK)
        {
            CStdioFile file;
            if (file.Open(dlgSaveTxt.GetPathName(), CFile::modeCreate | CFile::modeWrite | CFile::typeText))
            {
                file.WriteString(strErrorResult);
                file.Close();
                AfxMessageBox(_T("多 p 值误差结果保存成功！"));
            }
        }
    }
}

// ===== 新增：RGB-IHS 变换入口 =====
void CmyAlgDlg::OnClickedButtonRgbihs()
{
    if (imgIn.empty() || imgIn.m_rastercount != 3)
    {
        AfxMessageBox(_T("请先读取 3 波段 RGB 图像！"));
        return;
    }

    CRgbIhsParamDlg dlg(this);
    int nRgbIhsRet = dlg.DoModal();
    if (nRgbIhsRet != IDOK)
        return;

    CImageDataset imgIHS;
    if (!CImageProcessingEx::rgb2ihs(imgIn, imgIHS))
    {
        AfxMessageBox(_T("RGB-IHS 变换失败！"));
        return;
    }

    CString strFileName = m_strImgInput;
    int nPos = strFileName.ReverseFind('\\');
    if (nPos != -1) strFileName = strFileName.Mid(nPos + 1);

    if (dlg.m_bShowI || dlg.m_bShowH || dlg.m_bShowS)
    {
        CImageDataset imgIhsDisplay;
        CImageProcessingEx::linearStretch(imgIHS, imgIhsDisplay);

        if (dlg.m_bShowI)
        {
            CString strTitleI;
            strTitleI.Format(_T("IHS 分量 - I (%s)"), strFileName);
            CImageDisplay::show(imgIhsDisplay, this, strTitleI, 1, 1, 1, 0);
        }
        if (dlg.m_bShowH)
        {
            CString strTitleH;
            strTitleH.Format(_T("IHS 分量 - H (%s)"), strFileName);
            CImageDisplay::show(imgIhsDisplay, this, strTitleH, 2, 2, 2, 0);
        }
        if (dlg.m_bShowS)
        {
            CString strTitleS;
            strTitleS.Format(_T("IHS 分量 - S (%s)"), strFileName);
            CImageDisplay::show(imgIhsDisplay, this, strTitleS, 3, 3, 3, 0);
        }
    }

    if (dlg.m_bShowRecon)
    {
        CImageDataset imgRecon;
        if (!CImageProcessingEx::ihs2rgb(imgIHS, imgRecon))
        {
            AfxMessageBox(_T("IHS-RGB 反变换失败！"));
            return;
        }

        double dRmse = CImageProcessingEx::calcPcaRmse(imgIn, imgRecon);
        CString strTitleRecon;
        strTitleRecon.Format(_T("IHS 反变换结果 (RMSE=%.6f)"), dRmse);
        CImageDisplay::show(imgRecon, this, strTitleRecon, 1, 2, 3, 0);

        CString strOriginTitle;
        strOriginTitle.Format(_T("原始影像 (%s)"), strFileName);
        CImageDisplay::show(imgIn, this, strOriginTitle, 1, 2, 3, 0);

        imgRecon.duplicate(imgOut);
        m_strImgOutput = _T("IHS_Recon.tif");
        UpdateData(FALSE);
    }
}

// ===== 新增：Canny 边缘检测入口 =====
void CmyAlgDlg::OnClickedButtonCanny()
{
    if (imgIn.empty())
    {
        AfxMessageBox(_T("请先使用 \"Read\" 按钮读取输入图像！"));
        return;
    }

    double recLow = 0.0, recHigh = 0.0;
    CImageProcessingEx::getAutoCannyThresholds(imgIn, recLow, recHigh);

    CCannyParamDlg dlg(this);
    dlg.m_dLowThresh = recLow;
    dlg.m_dHighThresh = recHigh;
    dlg.m_pImgIn = &imgIn;
    int nCannyRet = dlg.DoModal();
    if (nCannyRet != IDOK)
        return;

    if (!CImageProcessingEx::cannyEdgeDetection(imgIn, imgOut, dlg.m_dSigma, dlg.m_dLowThresh, dlg.m_dHighThresh))
    {
        AfxMessageBox(_T("Canny 边缘检测失败！"));
        return;
    }

    CString strTitle;
    strTitle.Format(_T("Canny 边缘检测 (Sigma=%.1f, Low=%.1f, High=%.1f)"),
                    dlg.m_dSigma, dlg.m_dLowThresh, dlg.m_dHighThresh);
    CImageDisplay::show(imgOut, this, strTitle, 1, 1, 1, 0);
    m_strImgOutput = _T("Canny_Edge.tif");
    UpdateData(FALSE);
}

void CmyAlgDlg::OnClickedButtonFourier()
{
    TRACE(_T("开始傅里叶变换功能...\n"));
    
    // 1. 显示输入图像对话框
    CFourierInputDlg inputDlg;
    
    if (inputDlg.DoModal() != IDOK)
    {
        TRACE(_T("用户取消了输入对话框\n"));
        return;
    }
    
    TRACE(_T("输入对话框返回OK\n"));
    
    // 2. 检查是否成功获取图像
    if (inputDlg.m_inputImage.empty())
    {
        AfxMessageBox(_T("获取图像失败！"));
        return;
    }
    
    // 3. 显示获取的图像（使用非阻塞方式）
    CString title;
    if (inputDlg.m_selectionMode == 0)  // 生成图像模式
    {
        title.Format(_T("生成的图像 [%dx%d]"), 
                    inputDlg.m_inputImage.m_xsize,
                    inputDlg.m_inputImage.m_ysize);
        
        // 显示生成的图像（使用独立的窗口）
        CImageDisplay::show(inputDlg.m_inputImage, this, title, 1, 1, 1, 0);
    }
    else  // 读取图像模式
    {
        title = _T("读取的图像: ") + inputDlg.m_readImagePath;
        
        // 显示读取的图像
        CImageDisplay::show(inputDlg.m_inputImage, this, title, 1, 1, 1, 0);
    }
    
    // 4. 询问用户是否继续到频谱分析
    if (AfxMessageBox(_T("是否对该图像进行傅里叶频谱分析？"), 
                      MB_YESNO | MB_ICONQUESTION) == IDYES)
    {
        // 5. 显示频谱分析对话框
        TRACE(_T("显示频谱分析对话框...\n"));
        
        CFourierSpectrumDlg spectrumDlg(&inputDlg.m_inputImage, this);
        spectrumDlg.DoModal();
        
        TRACE(_T("频谱分析对话框关闭\n"));
    }
    else
    {
        TRACE(_T("用户选择不进行频谱分析\n"));
    }
}


void CmyAlgDlg::OnClickedButtonIdealhighpass()
{
    try {
        // 步骤1: 显示傅里叶输入对话框
        CFourierInputDlg inputDlg;
        if (inputDlg.DoModal() != IDOK)
            return;
        
        // 步骤2: 显示频谱对话框（计算DFT）
        // 注意：将 spectrumDlg 放在堆上，以便在 filterDlg 使用时不被销毁
        // 同时也保存原始图像的副本，以确保它在整个工作流中有效
        CFourierSpectrumDlg* pSpectrumDlg = new CFourierSpectrumDlg(&inputDlg.m_inputImage);
        pSpectrumDlg->SetAutoShowInverseDFT(FALSE);  // 不自动显示反变换，让工作流继续
        if (pSpectrumDlg->DoModal() != IDOK)
        {
            delete pSpectrumDlg;
            return;
        }
        
        AfxMessageBox(_T("即将进入高通滤波器对话框，请调整参数"));
        
        // 步骤3: 显示理想高通滤波器对话框
        CIdealFilterDlg filterDlg(pSpectrumDlg);
        int nResult = filterDlg.DoModal();
        
        if (nResult != IDOK)
        {
            AfxMessageBox(_T("用户取消了高通滤波器对话框"));
            delete pSpectrumDlg;
            return;
        }
        
        AfxMessageBox(_T("高通滤波器已应用，即将进行反变换"));
        
        // 验证过滤器是否正确应用
        if (filterDlg.m_filteredDFT.empty())
        {
            AfxMessageBox(_T("高通滤波器未正确应用！"));
            delete pSpectrumDlg;
            return;
        }
        
        // 步骤4: 显示滤波后的逆变换对话框
        CInverseDFTDlg inverseDlg(pSpectrumDlg);
        inverseDlg.SetFilteredDFT(&filterDlg.m_filteredDFT);
        inverseDlg.DoModal();

        // 将重建结果复制到主对话框的输出图像，便于使用“保存”按钮写盘
        CImageDataset& reconstructed = pSpectrumDlg->GetReconstructedImage();
        if (!reconstructed.empty())
        {
            if (FALSE == reconstructed.duplicate(imgOut))
            {
                TRACE(_T("复制重建图像到输出缓冲失败\n"));
            }
            else
            {
                TRACE(_T("已将重建图像复制到主窗口的输出图像，可直接保存\n"));
                AfxMessageBox(_T("重建图像已复制到主窗口的输出图像，点击“Write”可保存。"));
            }
        }
        
        // 清理堆内存
        delete pSpectrumDlg;
        
    }
    catch (CException* e) {
        e->ReportError();
        e->Delete();
    }
    catch (...) {
        AfxMessageBox(_T("处理过程中发生未知错误！"));
    }
}

// Wedge 去条带工作流：输入→频谱→Wedge→反变换→复制到输出
void CmyAlgDlg::OnClickedButtonWedge()
{
    try {
        // 1. 输入图像选择或生成
        CFourierInputDlg inputDlg;
        if (inputDlg.DoModal() != IDOK) return;

        // 2. 频谱显示(计算DFT)，不自动弹反变换
        CFourierSpectrumDlg* pSpectrumDlg = new CFourierSpectrumDlg(&inputDlg.m_inputImage);
        pSpectrumDlg->SetAutoShowInverseDFT(FALSE);
        if (pSpectrumDlg->DoModal() != IDOK) { delete pSpectrumDlg; return; }

        AfxMessageBox(_T("即将进入 Wedge 滤波对话框，请设置角度参数"));

        // 3. Wedge 滤波
        CWedgeFilterDlg wedgeDlg(pSpectrumDlg);
        if (wedgeDlg.DoModal() != IDOK) { delete pSpectrumDlg; return; }
        if (wedgeDlg.m_filteredDFT.empty()) {
            AfxMessageBox(_T("Wedge 滤波未成功应用！"));
            delete pSpectrumDlg; return;
        }

        AfxMessageBox(_T("Wedge 滤波已应用，即将进行反变换"));

        // 4. 反变换
        CInverseDFTDlg inverseDlg(pSpectrumDlg);
        inverseDlg.SetFilteredDFT(&wedgeDlg.m_filteredDFT);
        inverseDlg.DoModal();

        // 5. 将重建结果复制到输出图像，便于保存
        CImageDataset& reconstructed = pSpectrumDlg->GetReconstructedImage();
        if (!reconstructed.empty()) {
            if (FALSE == reconstructed.duplicate(imgOut)) {
                TRACE(_T("复制重建图像到输出缓冲失败\n"));
            } else {
                AfxMessageBox(_T("结果已复制到主窗口输出图像，点击 \"保存\" 可写盘。"));
            }
        }

        delete pSpectrumDlg;
    }
    catch (CException* e) { e->ReportError(); e->Delete(); }
    catch (...) { AfxMessageBox(_T("Wedge 处理过程中发生未知错误！")); }
}

// Otsu 图像分割：弹出参数对话框并基于当前输入图像执行
void CmyAlgDlg::OnClickedButtonOtsu()
{
    if (imgIn.empty())
    {
        AfxMessageBox(_T("请先使用\"读取\"按钮输入图像！"));
        return;
    }

    COtsuDlg dlg(this);
    dlg.SetInputImage(&imgIn);
    dlg.SetOutputImage(&imgOut);
    dlg.DoModal();
}