
// myAlgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "myAlg.h"
#include "myAlgDlg.h"
#include "afxdialogex.h"
#include "FourierInputDlg.h"
#include "FourierSpectrumDlg.h"

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
