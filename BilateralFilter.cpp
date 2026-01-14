// BilateralFilter.cpp : 实现文件
//

#include "stdafx.h"
#include "myAlg.h"
#include "BilateralFilter.h"
#include "afxdialogex.h"


// BilateralFilter 对话框

IMPLEMENT_DYNAMIC(BilateralFilter, CDialogEx)

BilateralFilter::BilateralFilter(CWnd* pParent /*=NULL*/)
	: CDialogEx(BilateralFilter::IDD, pParent)
{

	m_sigmaS = 0.0f;
	m_sigmaR = 0.0;
	m_windowSize = 0;
}

BilateralFilter::~BilateralFilter()
{
}

void BilateralFilter::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SIGMA_S, m_sigmaS);
	DDV_MinMaxFloat(pDX, m_sigmaS, 0, 100);
	DDX_Text(pDX, IDC_EDIT_SIGMA_R, m_sigmaR);
	DDV_MinMaxDouble(pDX, m_sigmaR, 0, 100);
	DDX_Text(pDX, IDC_EDIT_windowsize, m_windowSize);
	DDV_MinMaxInt(pDX, m_windowSize, 1, 15);
}


BEGIN_MESSAGE_MAP(BilateralFilter, CDialogEx)
END_MESSAGE_MAP()


// BilateralFilter 消息处理程序
