// OtsuDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "myAlg.h"
#include "OtsuDlg.h"
#include "afxdialogex.h"


// COtsuDlg 对话框

IMPLEMENT_DYNAMIC(COtsuDlg, CDialogEx)

COtsuDlg::COtsuDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(COtsuDlg::IDD, pParent)
{

}

COtsuDlg::~COtsuDlg()
{
}

void COtsuDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COtsuDlg, CDialogEx)
END_MESSAGE_MAP()


// COtsuDlg 消息处理程序
