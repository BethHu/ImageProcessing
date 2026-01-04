#pragma once
#include "afxdialogex.h"
#include <vector>

// CPcaChartDlg 对话框
class CPcaChartDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CPcaChartDlg)

public:
    CPcaChartDlg(CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CPcaChartDlg();

// 对话框数据
    enum { IDD = IDD_DIALOG_PCA_CHART };

    // 设置数据接口
    void SetData(const std::vector<int>& vecP, const std::vector<double>& vecRmse);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint(); // 绘图函数
    afx_msg void OnBnClickedButtonSave(); // 保存图片

    DECLARE_MESSAGE_MAP()

private:
    void DrawChart(CDC* pDC, CRect rect); // 绘图核心逻辑

    std::vector<int> m_vecP;
    std::vector<double> m_vecRmse;
    CButton m_btnSave;
    CFont m_font;
};
