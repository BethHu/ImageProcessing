// myAlgDlg.cpp : implementation file
//
#include "stdafx.h"
#include "myAlg.h"
#include "myAlgDlg.h"
#include "afxdialogex.h"
#include "ImageProcessingEx.h"
#include "MedianParamDlg.h"
#include "BilateralParamDlg.h"  // 包含双边滤波对话框头文件
#include "PcaPParamDlg.h"    // 包含PCA的p值输入对话框头文件
#include "RgbIhsParamDlg.h"  // 包含RGB-IHS选项对话框头文件
#include "CannyParamDlg.h"   // 包含Canny参数对话框头文件
#include "PcaChartDlg.h"     // 包含PCA误差曲线对话框头文件

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
	m_strlmgInput = _T("");  // 保持与.h文件一致
	m_strlmgOutput = _T("");
	m_strImgRef = _T("");
}

void CmyAlgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SRCFILE, m_strlmgInput);  
	DDX_Text(pDX, IDC_EDIT_DESFILE, m_strlmgOutput); 
	DDX_Text(pDX, IDC_EDIT_REF, m_strImgRef);      
}

BEGIN_MESSAGE_MAP(CmyAlgDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	// 按钮消息映射
	ON_BN_CLICKED(IDC_BUTTON_WRITE, &CmyAlgDlg::OnClickedButtonWrite)
	ON_BN_CLICKED(IDC_BUTTON_READ, &CmyAlgDlg::OnClickedButtonRead)
	ON_BN_CLICKED(IDC_BUTTON_HISTEQ, &CmyAlgDlg::OnClickedButtonHisteq)
	ON_BN_CLICKED(IDC_BUTTON_HISTMATCH, &CmyAlgDlg::OnClickedButtonHistmatch)
	ON_BN_CLICKED(IDC_BUTTON_READ_REF, &CmyAlgDlg::OnClickedButtonReadRef)
	ON_BN_CLICKED(IDC_BUTTON_MEDIAN, &CmyAlgDlg::OnClickedButtonMedian)
	ON_BN_CLICKED(IDC_BUTTON_BILATERAL, &CmyAlgDlg::OnClickedButtonBilateral)
	ON_BN_CLICKED(IDC_BUTTON_LAPLACIAN, &CmyAlgDlg::OnClickedButtonLaplacian)
	ON_BN_CLICKED(IDC_BUTTON_PCA, &CmyAlgDlg::OnClickedButtonPca)
	ON_BN_CLICKED(IDC_BUTTON_RGBIHS, &CmyAlgDlg::OnClickedButtonRgbIhs)
	ON_BN_CLICKED(IDC_BUTTON_CANNY, &CmyAlgDlg::OnClickedButtonCanny)
END_MESSAGE_MAP()


// CmyAlgDlg message handlers
BOOL CmyAlgDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 系统菜单添加“关于”项
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

	SetIcon(m_hIcon, TRUE);			// 大图标
	SetIcon(m_hIcon, FALSE);		// 小图标

	return TRUE;  // 除非设置控件焦点，否则返回TRUE
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

void CmyAlgDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 绘图设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 图标居中绘制
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CmyAlgDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 读取按钮实现
void CmyAlgDlg::OnClickedButtonRead()
{
	CFileDialog dlg(TRUE); // TRUE=打开文件对话框，FALSE=保存对话框
	if (dlg.DoModal() == IDOK)
	{
		if (FALSE == CImageIO::read(imgIn, dlg.GetPathName()))
		{
			AfxMessageBox(_T("读取图像失败！"));
		}
		else
		{
			m_strlmgInput = dlg.GetPathName();
			UpdateData(FALSE);  // 同步路径到编辑框显示

			// 按波段数显示图像（RSDIPLib规范：多波段RGB，单波段灰度）
			if (imgIn.m_rastercount >= 3)
				CImageDisplay::show(imgIn, this, dlg.GetFileName(), 1, 2, 3, 0);
			else
				CImageDisplay::show(imgIn, this, dlg.GetFileName(), 1, 1, 1, 0);
		}
	}
}

// 保存按钮实现
void CmyAlgDlg::OnClickedButtonWrite()
{
	CFileDialog dlg(FALSE);
	if (dlg.DoModal() == IDOK)
	{
		if (FALSE == CImageIO::write(imgOut, dlg.GetPathName()))
		{
			AfxMessageBox(_T("保存图像失败！"));
		}
		else
		{
			m_strlmgOutput = dlg.GetPathName();
			UpdateData(FALSE);  // 同步路径到编辑框显示

			if (imgOut.m_rastercount >= 3)
				CImageDisplay::show(imgOut, this, dlg.GetFileName(), 1, 2, 3, 0);
			else
				CImageDisplay::show(imgOut, this, dlg.GetFileName(), 1, 1, 1, 0);
		}
	}
}

// 直方图均衡化
void CmyAlgDlg::OnClickedButtonHisteq()
{
	BOOL bSuccess = CImageProcessingEx::histeq(imgIn, imgOut);
	if (!bSuccess)
    {
         AfxMessageBox(_T("处理图像失败！"));
         return;
    }

    if (imgOut.m_rastercount >= 3)
        CImageDisplay::show(imgOut, this, _T("直方图均衡化结果"), 1, 2, 3, 0);
    else
        CImageDisplay::show(imgOut, this, _T("直方图均衡化结果"), 1, 1, 1, 0);
}

// 直方图匹配
void CmyAlgDlg::OnClickedButtonHistmatch()
{
	// 1. 校验待匹配图像与参考图像是否已读取
    if (imgIn.empty() || imgRef.empty())
    {
        AfxMessageBox(_T("请先通过「读取」选择待匹配图像（quickbirdsub.tif），通过「输入参考图像」选择参考图像（livingroom.tif）！"));
        return;
    }

    // 2. 波段数一致性校验
    if (imgIn.m_rastercount != imgRef.m_rastercount)
    {
        AfxMessageBox(_T("待匹配图像与参考图像波段数不一致，无法进行直方图匹配！"));
        return;
    }

    // 3. 尺寸不一致警告（不阻止处理）
    if (imgIn.m_xsize != imgRef.m_xsize || imgIn.m_ysize != imgRef.m_ysize)
    {
        AfxMessageBox(_T("警告：图像尺寸不一致，匹配结果可能存在灰度分布偏差！"));
    }

    // 4. 调用匹配算法
    BOOL bSuccess = CImageProcessingEx::histmatch(imgIn, imgRef, imgOut);
    if (!bSuccess)
    {
        AfxMessageBox(_T("直方图匹配处理失败！"));
        return;
    }

    // 5. 显示结果
    AfxMessageBox(_T("直方图匹配处理成功！"));
    if (imgOut.m_rastercount >= 3)
        CImageDisplay::show(imgOut, this, _T("直方图匹配结果（待匹配：quickbirdsub）"), 1, 2, 3, 0);
    else
        CImageDisplay::show(imgOut, this, _T("直方图匹配结果（待匹配：quickbirdsub）"), 1, 1, 1, 0);
}

// 读取参考图像
void CmyAlgDlg::OnClickedButtonReadRef()
{
    CFileDialog dlg(
        TRUE,
        _T("tif"),
        NULL,
        OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
        _T("图像文件 (*.tif;*.jpg;*.bmp)|*.tif;*.jpg;*.bmp|所有文件 (*.*)|*.*|")
    );

    if (dlg.DoModal() != IDOK)
        return;

    if (FALSE == CImageIO::read(imgRef, dlg.GetPathName()))
    {
        AfxMessageBox(_T("读取参考图像失败！请检查文件格式（推荐livingroom.tif）。"));
        return;
    }

    m_strImgRef = dlg.GetPathName();
    UpdateData(FALSE);

    // 显示参考图像
    if (imgRef.m_rastercount >= 3)
        CImageDisplay::show(imgRef, this, _T("参考图像（livingroom.tif）"), 1, 2, 3, 0);
    else
        CImageDisplay::show(imgRef, this, _T("参考图像（livingroom.tif）"), 1, 1, 1, 0);
}

// 中值滤波
void CmyAlgDlg::OnClickedButtonMedian()
{
    if (imgIn.empty())
    {
        AfxMessageBox(_T("请先点击「读取」按钮选择输入图像（推荐building.gif）！"));
        return;
    }

    CMedianParamDlg dlgParam;
    dlgParam.m_pImgIn = &imgIn;
    if (dlgParam.DoModal() != IDOK)
        return;

    if (dlgParam.m_nWinSize % 2 == 0)
    {
        AfxMessageBox(_T("窗口大小必须是奇数（如3/5/7）！请重新输入。"));
        return;
    }

    BOOL bSuccess = CImageProcessingEx::medianFilter(imgIn, imgOut, dlgParam.m_nWinSize);
    if (!bSuccess)
    {
        AfxMessageBox(_T("中值滤波处理失败！"));
        return;
    }

    CString strTitle;
    strTitle.Format(_T("中值滤波结果（%d×%d窗口）"), dlgParam.m_nWinSize, dlgParam.m_nWinSize);
    AfxMessageBox(_T("中值滤波处理成功！"));
    
    if (imgOut.m_rastercount >= 3)
        CImageDisplay::show(imgOut, this, strTitle, 1, 2, 3, 0);
    else
        CImageDisplay::show(imgOut, this, strTitle, 1, 1, 1, 0);
}

// 双边滤波按钮响应
void CmyAlgDlg::OnClickedButtonBilateral()
{
    // 1. 校验输入图像已读取
    if (imgIn.empty())
    {
        AfxMessageBox(_T("请先点击「读取」按钮选择输入图像（推荐building.gif）！"));
        return;
    }

    // 2. 弹出自定义参数对话框（获取d、sigmaS、sigmaR）
    CBilateralParamDlg dlgParam;
    dlgParam.m_pImgIn = &imgIn;
    if (dlgParam.DoModal() != IDOK)  // 用户取消输入时返回
        return;

    // 3. 调用双边滤波算法（传递对话框参数，贴合指导书3.2.2节公式）
    BOOL bSuccess = CImageProcessingEx::bilateralFilter(
        imgIn,          // 输入图像（building.gif）
        imgOut,         // 输出图像（滤波结果）
        dlgParam.m_nD,  // 窗口大小d（对话框输入，奇数3~9）
        dlgParam.m_nSigmaS,  // 空间域σₛ（对话框输入，1~10）
        dlgParam.m_dSigmaR   // 灰度域σᵣ（对话框输入，0.05~0.2）
    );

    if (!bSuccess)
    {
        AfxMessageBox(_T("双边滤波处理失败！"));
        return;
    }

    // 4. 显示结果
    CString strTitle;
    strTitle.Format(_T("双边滤波结果（d=%d, 空间域sigmaS=%d, 灰度域simaR=%.2f）"), 
        dlgParam.m_nD, dlgParam.m_nSigmaS, dlgParam.m_dSigmaR);
    AfxMessageBox(_T("双边滤波处理成功！"));

    // 按波段数选择显示模式
    if (imgOut.m_rastercount >= 3)
        CImageDisplay::show(imgOut, this, strTitle, 1, 2, 3, 0);
    else
        CImageDisplay::show(imgOut, this, strTitle, 1, 1, 1, 0);
}

// 拉普拉斯锐化（保持原有逻辑，贴合指导书3.3节实验要求）
void CmyAlgDlg::OnClickedButtonLaplacian()
{
    if (imgIn.empty())
    {
        AfxMessageBox(_T("请先点击「读取」按钮选择输入图像（推荐building.gif）！"));
        return;
    }

    BOOL bSuccess = CImageProcessingEx::laplacianSharpen(imgIn, imgOut);
    if (!bSuccess)
    {
        AfxMessageBox(_T("拉普拉斯锐化处理失败！"));
        return;
    }

    AfxMessageBox(_T("拉普拉斯锐化处理成功！"));
    if (imgOut.m_rastercount >= 3)
        CImageDisplay::show(imgOut, this, _T("拉普拉斯锐化结果（4邻域模板）"), 1, 2, 3, 0);
    else
        CImageDisplay::show(imgOut, this, _T("拉普拉斯锐化结果（4邻域模板）"), 1, 1, 1, 0);
}

// PCA分析按钮点击响应
void CmyAlgDlg::OnClickedButtonPca()
{
    // 1. 校验：先读取输入图像
    if (imgIn.empty() || imgIn.m_rastercount < 2)  // 确保是多波段图像
    {
        AfxMessageBox(_T("请先点击「读取」按钮选择多波段实验数据（如lanier.img，波段数≥2）！"));
        return;
    }
    int nBandCount = imgIn.m_rastercount;  // 获取原始图像波段数（传给p值对话框做校验）

    // 2. 弹出自定义p值对话框
    CPcaPParamDlg dlgP;
    dlgP.SetBandCount(nBandCount);  // 传入波段数，限制p值范围（1≤p≤波段数）
    if (dlgP.DoModal() != IDOK)     // 用户点击取消则退出
        return;

    // 3. 执行PCA正变换（所有模式都需要）
    CImageDataset imgPCA;    // 输出：主成分影像
    CMatrix matT;            // 输出：PCA变换矩阵
    CMatrix matEigValues;    // 输出：特征值
    CMatrix matMean;         // 输出：均值
    if (!CImageProcessingEx::pcaForward(imgIn, imgPCA, matT, matEigValues, matMean))
    {
        AfxMessageBox(_T("PCA正变换失败！"));
        return;
    }

    // 4. 根据选择的模式分发处理
    // 模式1：仅正变换（保存并显示主成分影像）
    if (dlgP.m_bIsSingleMode == 0)
    {
        // 4.1 显示主成分影像（需拉伸到0-255显示）
        CImageDataset imgPCADisplay;
        if (CImageProcessingEx::linearStretch(imgPCA, imgPCADisplay))
        {
            if (imgPCADisplay.m_rastercount >= 3)
                CImageDisplay::show(imgPCADisplay, this, _T("主成分影像（前3分量）"), 1, 2, 3, 0);
            else
                CImageDisplay::show(imgPCADisplay, this, _T("主成分影像"), 1, 1, 1, 0);
        }

        // 4.2 保存主成分影像
        if (IDYES == AfxMessageBox(_T("PCA正变换成功！是否保存主成分影像？"), MB_YESNO))
        {
            CFileDialog dlgSavePCA(
                FALSE, _T("tif"), _T("PCA_Result.tif"), OFN_OVERWRITEPROMPT,
                _T("GeoTIFF文件 (*.tif)|*.tif|所有文件 (*.*)|*.*|")
            );
            if (dlgSavePCA.DoModal() == IDOK)
            {
                if (CImageIO::write(imgPCA, dlgSavePCA.GetPathName()))
                    AfxMessageBox(_T("主成分影像保存成功！"));
                else
                    AfxMessageBox(_T("保存失败！"));
            }
        }
    }
    // 模式2：单p值重建（正变换 + 反变换 + 显示重建 + 误差）
    else if (dlgP.m_bIsSingleMode == 1)
    {
        int nCustomP = dlgP.m_nSingleP;
        CImageDataset imgRecon;

        // 4.1 执行PCA反变换
        if (!CImageProcessingEx::pcaBackward(imgPCA, matT, matMean, nCustomP, imgRecon))
        {
            AfxMessageBox(_T("PCA反变换失败！"));
            return;
        }

        // 4.2 保存重建影像
        CString strDefaultFileName;
        strDefaultFileName.Format(_T("PCA_Recon_p%d"), nCustomP);
        CFileDialog dlgSave(
            FALSE, _T("tif"), strDefaultFileName, OFN_OVERWRITEPROMPT,
            _T("GeoTIFF文件 (*.tif)|*.tif|所有文件 (*.*)|*.*|")
        );
        if (dlgSave.DoModal() == IDOK)
        {
            if (CImageIO::write(imgRecon, dlgSave.GetPathName()))
                AfxMessageBox(_T("PCA重建影像保存成功！"));
            else
                AfxMessageBox(_T("保存重建影像失败！"));
        }

        // 4.3 计算并显示重建误差
        double dRmse = CImageProcessingEx::calcPcaRmse(imgIn, imgRecon);
        CString strRmseInfo;
        strRmseInfo.Format(_T("PCA重建完成（p=%d）！\n原始影像 vs 重建影像\nRMSE=%.6f"), nCustomP, dRmse);
        AfxMessageBox(strRmseInfo);

        // 4.4 显示结果
        CString strReconTitle;
        strReconTitle.Format(_T("PCA重建影像（p=%d）"), nCustomP);
        CImageDisplay::show(imgRecon, this, strReconTitle, 1, 2, 3, 0);
        CImageDisplay::show(imgIn, this, _T("原始影像（lanier.img）"), 1, 2, 3, 0);
    }
    // 模式3：多p值误差分析
    else
    {
        std::vector<int> vecMultiP = dlgP.m_vecMultiP;
        std::vector<double> vecRmseResults; // Store RMSE for chart
        std::vector<int> vecPResults;       // Store valid P for chart
        CString strErrorResult = _T("多p值PCA重建误差（RMSE）结果：\n");

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
                
                // Collect data for chart
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
        
        // Show Chart Dialog
        if (!vecPResults.empty())
        {
            CPcaChartDlg dlgChart;
            dlgChart.SetData(vecPResults, vecRmseResults);
            dlgChart.DoModal();
        }

        CFileDialog dlgSaveTxt(
            FALSE, _T("txt"), _T("PCA_MultiP_Error.txt"), OFN_OVERWRITEPROMPT,
            _T("文本文件 (*.txt)|*.txt|所有文件 (*.*)|*.*|")
        );
        if (dlgSaveTxt.DoModal() == IDOK)
        {
            CStdioFile file;
            if (file.Open(dlgSaveTxt.GetPathName(), CFile::modeCreate | CFile::modeWrite | CFile::typeText))
            {
                file.WriteString(strErrorResult);
                file.Close();
                AfxMessageBox(_T("多p值误差结果保存成功！"));
            }
        }
    }
}

// RGB-IHS变换按钮响应
void CmyAlgDlg::OnClickedButtonRgbIhs()
{
    // 1. 校验输入
    if (imgIn.empty() || imgIn.m_rastercount != 3)
    {
        AfxMessageBox(_T("请先读取3波段RGB图像（如color.jpg）！"));
        return;
    }

    // 2. 弹出选项对话框
    CRgbIhsParamDlg dlg;
    if (dlg.DoModal() != IDOK)
        return;

    // 3. RGB -> IHS 正变换
    CImageDataset imgIHS;
    if (!CImageProcessingEx::rgb2ihs(imgIn, imgIHS))
    {
        AfxMessageBox(_T("RGB-IHS变换失败！"));
        return;
    }

    // 获取文件名
    int nPos = m_strlmgInput.ReverseFind('\\');
    CString strFileName = (nPos == -1) ? m_strlmgInput : m_strlmgInput.Mid(nPos + 1);

    // 4. 根据选择显示 IHS 分量
    if (dlg.m_bShowI || dlg.m_bShowH || dlg.m_bShowS)
    {
        CImageDataset imgIHS_Display;
        CImageProcessingEx::linearStretch(imgIHS, imgIHS_Display);

        if (dlg.m_bShowI)
        {
            CString strTitleI;
            strTitleI.Format(_T("IHS变换分量 - 明度(I) (%s)"), strFileName);
            CImageDisplay::show(imgIHS_Display, this, strTitleI, 1, 1, 1, 0);
        }
        if (dlg.m_bShowH)
        {
            CString strTitleH;
            strTitleH.Format(_T("IHS变换分量 - 色调(H) (%s)"), strFileName);
            CImageDisplay::show(imgIHS_Display, this, strTitleH, 2, 2, 2, 0);
        }
        if (dlg.m_bShowS)
        {
            CString strTitleS;
            strTitleS.Format(_T("IHS变换分量 - 饱和度(S) (%s)"), strFileName);
            CImageDisplay::show(imgIHS_Display, this, strTitleS, 3, 3, 3, 0);
        }
    }

    // 5. 根据选择执行反变换并显示
    if (dlg.m_bShowRecon)
    {
        CImageDataset imgRGB_Recon;
        if (!CImageProcessingEx::ihs2rgb(imgIHS, imgRGB_Recon))
        {
            AfxMessageBox(_T("IHS-RGB反变换失败！"));
            return;
        }

        double dRmse = CImageProcessingEx::calcPcaRmse(imgIn, imgRGB_Recon);
        
        CString strTitleRecon;
        strTitleRecon.Format(_T("IHS反变换结果 (RMSE=%.6f)"), dRmse);
        CImageDisplay::show(imgRGB_Recon, this, strTitleRecon, 1, 2, 3, 0);

        // 显示原始图像对比
        CString strOriginTitle;
        strOriginTitle.Format(_T("原始影像 (%s)"), strFileName);
        CImageDisplay::show(imgIn, this, strOriginTitle, 1, 2, 3, 0);

        CString strMsg;
        strMsg.Format(_T("处理完成！\n重建RMSE = %.6f"), dRmse);
        AfxMessageBox(strMsg);
        
        // 准备保存结果
        imgRGB_Recon.duplicate(imgOut);
        m_strlmgOutput = _T("IHS_Recon.tif");
        UpdateData(FALSE);
    }
}

// Canny 边缘检测按钮响应
void CmyAlgDlg::OnClickedButtonCanny()
{
    // 1. 校验输入
    if (imgIn.empty())
    {
        AfxMessageBox(_T("请先读取图像！"));
        return;
    }

    // 2. 自动计算推荐阈值并初始化对话框
    double recLow, recHigh;
    CImageProcessingEx::getAutoCannyThresholds(imgIn, recLow, recHigh);
    
    CCannyParamDlg dlg;
    dlg.m_dLowThresh = recLow;
    dlg.m_dHighThresh = recHigh;
    dlg.m_pImgIn = &imgIn; // Pass input image for preview

    if (dlg.DoModal() != IDOK)
        return;

    // 3. 执行 Canny 边缘检测
    if (!CImageProcessingEx::cannyEdgeDetection(imgIn, imgOut, dlg.m_dSigma, dlg.m_dLowThresh, dlg.m_dHighThresh))
    {
        AfxMessageBox(_T("Canny 边缘检测失败！"));
        return;
    }

    // 4. 显示结果
    CString strTitle;
    strTitle.Format(_T("Canny 边缘检测 (Sigma=%.1f, Low=%.1f, High=%.1f)"), 
        dlg.m_dSigma, dlg.m_dLowThresh, dlg.m_dHighThresh);
    
    CImageDisplay::show(imgOut, this, strTitle, 1, 1, 1, 0);
    
    AfxMessageBox(_T("Canny 边缘检测完成！"));
    
    // 5. 准备保存
    m_strlmgOutput = _T("Canny_Edge.tif");
    UpdateData(FALSE);
}

