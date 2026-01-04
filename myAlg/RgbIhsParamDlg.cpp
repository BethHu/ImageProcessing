#include "stdafx.h"
#include "myAlg.h"
#include "RgbIhsParamDlg.h"
#include "afxdialogex.h"

IMPLEMENT_DYNAMIC(CRgbIhsParamDlg, CDialogEx)

CRgbIhsParamDlg::CRgbIhsParamDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRgbIhsParamDlg::IDD, pParent)
	, m_bShowI(TRUE)
	, m_bShowH(TRUE)
	, m_bShowS(TRUE)
	, m_bShowRecon(TRUE)
{
}

CRgbIhsParamDlg::~CRgbIhsParamDlg()
{
}

void CRgbIhsParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_SHOW_I, m_bShowI);
	DDX_Check(pDX, IDC_CHECK_SHOW_H, m_bShowH);
	DDX_Check(pDX, IDC_CHECK_SHOW_S, m_bShowS);
	DDX_Check(pDX, IDC_CHECK_SHOW_RECON, m_bShowRecon);
}

BEGIN_MESSAGE_MAP(CRgbIhsParamDlg, CDialogEx)
END_MESSAGE_MAP()
