#pragma once
#include "afxdialogex.h"

// BilateralFilter 对话框

class BilateralFilter : public CDialogEx
{
	DECLARE_DYNAMIC(BilateralFilter)

public:
	BilateralFilter(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~BilateralFilter();

// 对话框数据
	enum { IDD = IDD_BILATERALFILTER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	float m_sigmaS;
	double m_sigmaR;
	int m_windowSize;
};
