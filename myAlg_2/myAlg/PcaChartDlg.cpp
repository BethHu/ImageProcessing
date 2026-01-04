// PcaChartDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "myAlg.h"
#include "PcaChartDlg.h"
#include "afxdialogex.h"
#include <algorithm>
#include <atlimage.h> // For CImage

#define ID_BTN_SAVE 3001

// CPcaChartDlg 对话框

IMPLEMENT_DYNAMIC(CPcaChartDlg, CDialogEx)

CPcaChartDlg::CPcaChartDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(CPcaChartDlg::IDD, pParent)
{

}

CPcaChartDlg::~CPcaChartDlg()
{
}

void CPcaChartDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

void CPcaChartDlg::SetData(const std::vector<int>& vecP, const std::vector<double>& vecRmse)
{
    m_vecP = vecP;
    m_vecRmse = vecRmse;
}

BEGIN_MESSAGE_MAP(CPcaChartDlg, CDialogEx)
    ON_WM_PAINT()
    ON_BN_CLICKED(ID_BTN_SAVE, &CPcaChartDlg::OnBnClickedButtonSave)
END_MESSAGE_MAP()

// CPcaChartDlg 消息处理程序

BOOL CPcaChartDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // 创建字体
    m_font.CreatePointFont(90, _T("Arial"));

    // 创建保存按钮
    CRect rectDlg;
    GetClientRect(&rectDlg);
    CRect rectBtn;
    // 调整按钮位置和大小，留出更多空间
    rectBtn.SetRect(rectDlg.right - 180, rectDlg.bottom - 55, rectDlg.right - 20, rectDlg.bottom - 15);
    m_btnSave.Create(_T("Save Image"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rectBtn, this, ID_BTN_SAVE);
    m_btnSave.SetFont(&m_font);

    return TRUE;
}

void CPcaChartDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    
    // 获取绘图区域（IDC_STATIC_CHART）
    CWnd* pWnd = GetDlgItem(IDC_STATIC_CHART);
    if (!pWnd) return;
    
    CRect rect;
    pWnd->GetWindowRect(&rect);
    ScreenToClient(&rect);
    
    // 底部留出空间给按钮，避免重叠
    rect.bottom -= 50;

    DrawChart(&dc, rect);
}

void CPcaChartDlg::DrawChart(CDC* pDC, CRect rect)
{
    // 1. 绘制背景
    pDC->FillSolidRect(rect, RGB(255, 255, 255));
    pDC->FrameRect(rect, &CBrush(RGB(0, 0, 0)));

    if (m_vecP.empty() || m_vecRmse.empty()) return;

    // 2. 准备数据范围
    double maxRmse = *std::max_element(m_vecRmse.begin(), m_vecRmse.end());
    double minRmse = *std::min_element(m_vecRmse.begin(), m_vecRmse.end());
    
    // Y轴稍微扩展一点范围，避免贴边
    double range = maxRmse - minRmse;
    if (range < 1e-6) range = 1.0; // 避免除零
    double yMax = maxRmse + range * 0.1;
    double yMin = minRmse - range * 0.1;
    if (yMin < 0) yMin = 0; // RMSE通常非负

    int maxP = *std::max_element(m_vecP.begin(), m_vecP.end());
    int minP = *std::min_element(m_vecP.begin(), m_vecP.end());

    // 3. 定义绘图区边距（增加边距，解决局促问题）
    int marginL = 70; // 左边距留给Y轴文字
    int marginR = 40;
    int marginT = 40;
    int marginB = 60; // 下边距留给X轴文字

    int plotLeft = rect.left + marginL;
    int plotRight = rect.right - marginR;
    int plotTop = rect.top + marginT;
    int plotBottom = rect.bottom - marginB;
    int plotW = plotRight - plotLeft;
    int plotH = plotBottom - plotTop;

    // 4. 绘制网格和坐标轴
    CPen penGrid(PS_DASH, 1, RGB(200, 200, 200));
    CPen penAxis(PS_SOLID, 2, RGB(0, 0, 0));
    CFont* pOldFont = pDC->SelectObject(&m_font);

    // Y轴网格和刻度
    int nTicks = 5;
    for (int i = 0; i <= nTicks; i++)
    {
        int y = plotBottom - (int)((double)i / nTicks * plotH);
        double val = yMin + (double)i / nTicks * (yMax - yMin);

        // 网格线
        CPen* pOldPen = pDC->SelectObject(&penGrid);
        pDC->MoveTo(plotLeft, y);
        pDC->LineTo(plotRight, y);
        pDC->SelectObject(pOldPen);

        // 刻度值
        CString strVal;
        strVal.Format(_T("%.2f"), val);
        CRect rectText(rect.left, y - 10, plotLeft - 5, y + 10);
        pDC->DrawText(strVal, rectText, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
    }

    // X轴网格和刻度
    // 假设P值是离散的整数
    for (size_t i = 0; i < m_vecP.size(); i++)
    {
        int pVal = m_vecP[i];
        // 简单的线性映射：根据索引分布，或者根据P值分布
        // 这里根据P值线性分布
        int x = plotLeft + (int)((double)(pVal - minP) / (maxP - minP) * plotW);
        if (maxP == minP) x = plotLeft + plotW / 2;

        // 网格线（竖向）
        CPen* pOldPen = pDC->SelectObject(&penGrid);
        pDC->MoveTo(x, plotTop);
        pDC->LineTo(x, plotBottom);
        pDC->SelectObject(pOldPen);

        // 刻度值
        CString strVal;
        strVal.Format(_T("%d"), pVal);
        CRect rectText(x - 20, plotBottom + 5, x + 20, plotBottom + 25);
        pDC->DrawText(strVal, rectText, DT_CENTER | DT_TOP | DT_SINGLELINE);
    }

    // 绘制实线坐标轴
    CPen* pOldPen = pDC->SelectObject(&penAxis);
    // Y轴
    pDC->MoveTo(plotLeft, plotTop);
    pDC->LineTo(plotLeft, plotBottom);
    // X轴
    pDC->MoveTo(plotLeft, plotBottom);
    pDC->LineTo(plotRight, plotBottom);
    pDC->SelectObject(pOldPen);

    // 轴标题
    pDC->TextOut(plotLeft + plotW / 2 - 40, plotBottom + 30, _T("Principal Components (p)"));
    // Y轴标题
    pDC->TextOut(rect.left + 10, rect.top + 5, _T("RMSE"));


    // 5. 绘制数据曲线
    CPen penLine(PS_SOLID, 2, RGB(0, 0, 255));
    CBrush brushPoint(RGB(255, 0, 0));
    pOldPen = pDC->SelectObject(&penLine);
    CBrush* pOldBrush = pDC->SelectObject(&brushPoint);

    CPoint prevPoint;
    for (size_t i = 0; i < m_vecP.size(); i++)
    {
        int pVal = m_vecP[i];
        double rmse = m_vecRmse[i];

        int x = plotLeft + (int)((double)(pVal - minP) / (maxP - minP) * plotW);
        if (maxP == minP) x = plotLeft + plotW / 2;

        int y = plotBottom - (int)((rmse - yMin) / (yMax - yMin) * plotH);

        // 连线
        if (i > 0)
        {
            pDC->MoveTo(prevPoint);
            pDC->LineTo(x, y);
        }
        prevPoint = CPoint(x, y);

        // 绘制点
        int r = 3;
        pDC->Ellipse(x - r, y - r, x + r, y + r);
        
        // 在点旁边显示数值
        CString strVal;
        strVal.Format(_T("%.2f"), rmse);
        pDC->TextOut(x + 5, y - 15, strVal);
    }

    pDC->SelectObject(pOldPen);
    pDC->SelectObject(pOldBrush);
    pDC->SelectObject(pOldFont);
}

void CPcaChartDlg::OnBnClickedButtonSave()
{
    CFileDialog dlg(FALSE, _T("png"), _T("PCA_Error_Chart.png"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("PNG Image (*.png)|*.png|Bitmap (*.bmp)|*.bmp||"), this);
    if (dlg.DoModal() == IDOK)
    {
        CString filePath = dlg.GetPathName();

        // 获取绘图区域大小
        CWnd* pWnd = GetDlgItem(IDC_STATIC_CHART);
        if (!pWnd) return;
        CRect rect;
        pWnd->GetWindowRect(&rect);
        // 宽高创建图片
        int w = rect.Width();
        int h = rect.Height();

        CImage image;
        image.Create(w, h, 24);

        // 获取Image的DC
        CDC* pDC = CDC::FromHandle(image.GetDC());
        
        // 创建内存DC绘制
        CDC memDC;
        memDC.CreateCompatibleDC(pDC);
        CBitmap bmp;
        bmp.CreateCompatibleBitmap(pDC, w, h);
        CBitmap* pOldBmp = memDC.SelectObject(&bmp);

        // 填充白色背景
        memDC.FillSolidRect(0, 0, w, h, RGB(255, 255, 255));

        // 调用绘图函数
        CRect drawRect(0, 0, w, h);
        DrawChart(&memDC, drawRect);

        // 将内存DC内容复制到Image DC 
        pDC->BitBlt(0, 0, w, h, &memDC, 0, 0, SRCCOPY);

        memDC.SelectObject(pOldBmp);
        image.ReleaseDC();

        // 保存
        HRESULT hr = image.Save(filePath);
        if (SUCCEEDED(hr))
            AfxMessageBox(_T("Image saved successfully!"));
        else
            AfxMessageBox(_T("Failed to save image."));
    }
}
