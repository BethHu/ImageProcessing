// MedianFilterDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "myAlg.h"
#include "MedianFilterDlg.h"
#include "afxdialogex.h"


// CMedianFilterDlg 对话框

IMPLEMENT_DYNAMIC(CMedianFilterDlg, CDialogEx)

CMedianFilterDlg::CMedianFilterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMedianFilterDlg::IDD, pParent)
{
	// 设置默认值为3（3x3窗口）
	m_windowSize = 3;
}

CMedianFilterDlg::~CMedianFilterDlg()
{
}

void CMedianFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_window, m_windowSize);
	DDV_MinMaxInt(pDX, m_windowSize, 1, 15);
}


BEGIN_MESSAGE_MAP(CMedianFilterDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_sure, &CMedianFilterDlg::OnClickedButtonSure)
	ON_EN_CHANGE(IDC_EDIT_window, &CMedianFilterDlg::OnEnChangeEditwindow)
END_MESSAGE_MAP()


// CMedianFilterDlg 消息处理程序

BOOL CMedianFilterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	// 设置对话框标题
	SetWindowText(_T("中值滤波参数设置"));
	
	// 设置编辑框默认值
	SetDlgItemInt(IDC_EDIT_window, m_windowSize);
	
	return TRUE;
}

void CMedianFilterDlg::OnClickedButtonSure()
{
	// 1. 从控件获取用户输入的数据
	UpdateData(TRUE);
	
	// 2. 验证窗口大小必须是奇数
	if (m_windowSize % 2 == 0)
	{
		AfxMessageBox(_T("窗口大小必须是奇数（如3、5、7...）！"));
		
		// 选中编辑框内容，方便用户修改
		CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_window);
		if (pEdit)
		{
			pEdit->SetSel(0, -1);
			pEdit->SetFocus();
		}
		return;
	}
	
	// 3. 验证窗口大小不能太大
	if (m_windowSize > 15)
	{
		AfxMessageBox(_T("窗口大小建议不要超过15！"));
		CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_window);
		if (pEdit)
		{
			pEdit->SetSel(0, -1);
			pEdit->SetFocus();
		}
		return;
	}
	
	// 4. 所有验证通过，关闭对话框
	EndDialog(IDOK);
}

void CMedianFilterDlg::OnOK()
{
	// 当用户按Enter键时，也执行确定按钮的逻辑
	OnClickedButtonSure();
}

void CMedianFilterDlg::OnCancel()
{
	// 用户点击取消或按ESC键
	EndDialog(IDCANCEL);
}

void CMedianFilterDlg::OnEnChangeEditwindow()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}
