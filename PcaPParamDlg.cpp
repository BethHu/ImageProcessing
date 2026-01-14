#include "stdafx.h"
#include "myAlg.h"
#include "PcaPParamDlg.h"
#include "afxdialogex.h"

IMPLEMENT_DYNAMIC(CPcaPParamDlg, CDialogEx)

CPcaPParamDlg::CPcaPParamDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(CPcaPParamDlg::IDD, pParent)
	, m_nSingleP(3)
	, m_bIsSingleMode(0)
	, m_nBandCount(0)
	, m_strPInput(_T("3"))
{
}

CPcaPParamDlg::~CPcaPParamDlg()
{
}

void CPcaPParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_P_VALUE, m_strPInput);
	DDX_Radio(pDX, IDC_RADIO_PCA_ONLY, m_bIsSingleMode);
}

BEGIN_MESSAGE_MAP(CPcaPParamDlg, CDialogEx)
	ON_BN_CLICKED(IDC_RADIO_PCA_ONLY, &CPcaPParamDlg::OnBnClickedRadioMode)
	ON_BN_CLICKED(IDC_RADIO_SINGLE_P, &CPcaPParamDlg::OnBnClickedRadioMode)
	ON_BN_CLICKED(IDC_RADIO_MULTI_P, &CPcaPParamDlg::OnBnClickedRadioMode)
END_MESSAGE_MAP()

BOOL CPcaPParamDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	OnBnClickedRadioMode();
	return TRUE;
}

void CPcaPParamDlg::OnBnClickedRadioMode()
{
	UpdateData(TRUE);
	if (GetDlgItem(IDC_EDIT_P_VALUE))
		GetDlgItem(IDC_EDIT_P_VALUE)->EnableWindow(m_bIsSingleMode != 0);
}

void CPcaPParamDlg::OnOK()
{
	UpdateData(TRUE);

	if (m_nBandCount <= 0)
	{
		AfxMessageBox(_T("Please load a multi-band image (e.g., lanier.img) first!"));
		GetDlgItem(IDC_EDIT_P_VALUE)->SetFocus();
		return;
	}

	if (m_bIsSingleMode == 0 || m_bIsSingleMode == 1)
	{
		TCHAR fullwidthComma = (TCHAR)0xff0c; // '，'
		if (m_strPInput.Find(_T(',')) != -1 || m_strPInput.Find(fullwidthComma) != -1)
		{
			AfxMessageBox(_T("Single-p mode: please input ONE integer (e.g., 3), without comma."));
			GetDlgItem(IDC_EDIT_P_VALUE)->SetFocus();
			return;
		}

		m_nSingleP = _ttoi(m_strPInput);
		if (m_nSingleP < 1 || m_nSingleP > m_nBandCount)
		{
			CString strErr;
			strErr.Format(_T("Single p must be in [1, %d] (band count=%d)."), m_nBandCount, m_nBandCount);
			AfxMessageBox(strErr);
			GetDlgItem(IDC_EDIT_P_VALUE)->SetFocus();
			return;
		}
	}
	else
	{
		m_vecMultiP.clear();
		CString strTemp = m_strPInput;
		TCHAR fullwidthComma2 = (TCHAR)0xff0c; // '，'
		strTemp.Replace(fullwidthComma2, _T(','));

		int nPos = 0;
		CString strToken = strTemp.Tokenize(_T(","), nPos);
		while (!strToken.IsEmpty())
		{
			int nP = _ttoi(strToken);
			if (nP < 1 || nP > m_nBandCount)
			{
				CString strErr;
				strErr.Format(_T("Invalid p=%d in list; must be in [1, %d]."), nP, m_nBandCount);
				AfxMessageBox(strErr);
				GetDlgItem(IDC_EDIT_P_VALUE)->SetFocus();
				return;
			}

			for (size_t i = 0; i < m_vecMultiP.size(); i++)
			{
				if (m_vecMultiP[i] == nP)
				{
					CString strErr;
					strErr.Format(_T("Duplicate p=%d in list; please ensure each p is unique."), nP);
					AfxMessageBox(strErr);
					GetDlgItem(IDC_EDIT_P_VALUE)->SetFocus();
					return;
				}
			}

			m_vecMultiP.push_back(nP);
			strToken = strTemp.Tokenize(_T(","), nPos);
		}

		if (m_vecMultiP.size() < 2)
		{
			AfxMessageBox(_T("Multi-p error analysis needs at least 2 p values; use commas, e.g., 2,3,4."));
			GetDlgItem(IDC_EDIT_P_VALUE)->SetFocus();
			return;
		}
	}

	CDialogEx::OnOK();
}
