#pragma once
#include "afxdialogex.h"
#include <vector>

// PCA p值输入对话框：支持单p值重建、多p值误差计算
class CPcaPParamDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CPcaPParamDlg)

public:
    CPcaPParamDlg(CWnd* pParent = nullptr);  // 构造函数
    virtual ~CPcaPParamDlg();                // 析构函数
    enum { IDD = IDD_DIALOG_PCA_P };         // 对话框资源ID
protected:
    virtual void DoDataExchange(CDataExchange* pDX);  // 数据交换函数：实现控件与变量的绑定
    DECLARE_MESSAGE_MAP()

public:
    // 输出参数：用户输入的PCA降维p值
    int m_nSingleP;                // 单p值（单值模式下的PCA降维维度）
    std::vector<int> m_vecMultiP;   // 多p值列表（多值模式下用于误差计算的PCA维度列表）
    int m_bIsSingleMode;           // 单选按钮组状态（0=单p值模式，1=多p值模式，适配DDX_Radio规则）
    CString m_strPInput;           // 编辑框输入字符串（支持单值如"3"、多值如"2,3,4"）

    // 输入参数：设置图像波段数（用于校验p值≤波段数）
    void SetBandCount(int nBandCount) { m_nBandCount = nBandCount; }
    
    virtual BOOL OnInitDialog();               // 对话框初始化函数
    afx_msg void OnBnClickedRadioMode();       // 单选按钮模式切换事件响应函数

    // 重写OnOK函数：解析输入的p值并校验合法性
    virtual void OnOK();

private:
    int m_nBandCount;  // 图像波段数
};