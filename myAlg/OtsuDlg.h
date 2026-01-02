#pragma once


// COtsuDlg 对话框

class COtsuDlg : public CDialogEx
{
	DECLARE_DYNAMIC(COtsuDlg)

public:
	COtsuDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~COtsuDlg();

// 对话框数据
	enum { IDD = IDD_OTSUDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
