#pragma once
#include "afxdialogex.h"
#include "RSDIPLib.h"

// CFourierInputDlg 对话框
class CFourierInputDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CFourierInputDlg)

public:
    CFourierInputDlg(CWnd* pParent = nullptr);
    virtual ~CFourierInputDlg();

    // 对话框数据
    enum { IDD = IDD_FOURIERINPUTDLG };

    // === 输入参数 ===
    
    // 选择模式
    int m_selectionMode;          // 0-生成图像，1-读取图像
    
    // 图像类型
    int m_genImageType;           // 0-矩形，1-正弦波，2-棋盘格
    
    // 通用图像参数
    int m_genWidth;
    int m_genHeight;
    
    // 矩形图像参数
    int m_rectWidth;
    int m_rectHeight;
    int m_bgColor;
    int m_rectColor;
    
    // 正弦波图像参数
    double m_sineFrequency;
    double m_sinePhase;
    
    // 棋盘格图像参数
    int m_checkerSize;
    
    // 读取图像参数
    CString m_readImagePath;
    
    // === 输出结果 ===
    CImageDataset m_inputImage;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    
    // 消息处理函数
    afx_msg void OnBnClickedRadioGenerate();
    afx_msg void OnBnClickedRadioRead();
    afx_msg void OnCbnSelchangeComboGenType();
    afx_msg void OnBnClickedButtonBrowse();
    afx_msg void OnBnClickedButtonPreview();
    
    // 辅助函数
    void UpdateUIState();          // 更新界面显示
    BOOL GeneratePreviewImage();   // 生成预览图像
    void UpdateParametersVisibility(); // 更新参数组显示
};