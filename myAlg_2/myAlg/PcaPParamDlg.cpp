#include "stdafx.h"
#include "myAlg.h"
#include "PcaPParamDlg.h"
#include "afxdialogex.h"

IMPLEMENT_DYNAMIC(CPcaPParamDlg, CDialogEx)

// 构造函数：初始化默认值
CPcaPParamDlg::CPcaPParamDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(CPcaPParamDlg::IDD, pParent)
    , m_nSingleP(3)                // 默认单p值=3（指导书推荐）
    , m_bIsSingleMode(0)           // 0=默认选中“单p值模式”（DDX_Radio中0对应第一个按钮）
    , m_nBandCount(0)              // 初始波段数=0（需主对话框传入）
    , m_strPInput(_T("3"))         // 编辑框默认显示"3"，与默认p值一致
{
}

CPcaPParamDlg::~CPcaPParamDlg()
{
}

// 核心：数据绑定（控件↔变量），必须与资源控件ID严格匹配
void CPcaPParamDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    // 1. 编辑框（IDC_EDIT_P_VALUE）↔ 输入字符串（m_strPInput）
    DDX_Text(pDX, IDC_EDIT_P_VALUE, m_strPInput);
    // 2. 单选按钮组（第一个按钮IDC_RADIO_PCA_ONLY）↔ 状态变量（m_bIsSingleMode）
    // 规则：m_bIsSingleMode=0 → 选中第一个按钮（仅正变换），=1 → 选中第二个按钮（单p重建），=2 → 选中第三个按钮（多p误差）
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
    OnBnClickedRadioMode(); // 初始化控件状态
    return TRUE;
}

void CPcaPParamDlg::OnBnClickedRadioMode()
{
    UpdateData(TRUE); // 获取当前 Radio 选中状态
    // 模式0：仅正变换 -> 禁用 P 值输入
    // 模式1：单 P 重建 -> 启用
    // 模式2：多 P 误差 -> 启用
    if (GetDlgItem(IDC_EDIT_P_VALUE))
        GetDlgItem(IDC_EDIT_P_VALUE)->EnableWindow(m_bIsSingleMode != 0);
}

// 解析p值并校验合法性（修正模式判断逻辑）
void CPcaPParamDlg::OnOK()
{
    UpdateData(TRUE);  // 从控件读取数据到变量（关键：同步编辑框和单选按钮状态）

    // 1. 校验：必须先读取多波段图像（传入波段数）
    if (m_nBandCount <= 0)
    {
        AfxMessageBox(_T("请先读取多波段图像（如lanier.img）！"));
        GetDlgItem(IDC_EDIT_P_VALUE)->SetFocus();
        return;
    }

    // 2. 处理单p值模式（0=仅正变换，1=单p重建）
    if (m_bIsSingleMode == 0 || m_bIsSingleMode == 1)
    {
        // 严格校验：单p值模式下不允许输入逗号（防止用户误输入多值）
        // 同时检测英文逗号和中文逗号
        if (m_strPInput.Find(_T(',')) != -1 || m_strPInput.Find(_T("，")) != -1)
        {
            AfxMessageBox(_T("当前为单p值模式，请输入单个整数（如3），不要包含逗号！"));
            GetDlgItem(IDC_EDIT_P_VALUE)->SetFocus();
            return;
        }

        m_nSingleP = _ttoi(m_strPInput);  // 字符串转整数p值
        // 校验p值范围：1≤p≤波段数
        if (m_nSingleP < 1 || m_nSingleP > m_nBandCount)
        {
            CString strErr;
            strErr.Format(_T("单p值需满足1≤p≤%d（当前波段数=%d）！"), m_nBandCount, m_nBandCount);
            AfxMessageBox(strErr);
            GetDlgItem(IDC_EDIT_P_VALUE)->SetFocus();
            return;
        }
    }
    // 3. 处理多p值模式（m_bIsSingleMode=2 → 选中“多p值”按钮）
    else
    {
        m_vecMultiP.clear();  // 清空原有列表，避免残留数据
        CString strTemp = m_strPInput;
        strTemp.Replace(_T("，"), _T(",")); // 兼容性处理：将中文逗号统一替换为英文逗号

        int nPos = 0;
        // 按英文逗号分割（如"2,3,4"→["2","3","4"]，符合之前解析逻辑）
        CString strToken = strTemp.Tokenize(_T(","), nPos);
        while (!strToken.IsEmpty())
        {
            int nP = _ttoi(strToken);
            // 校验每个p值合法性
            if (nP < 1 || nP > m_nBandCount)
            {
                CString strErr;
                strErr.Format(_T("多p值中存在非法值%d！需满足1≤p≤%d"), nP, m_nBandCount);
                AfxMessageBox(strErr);
                GetDlgItem(IDC_EDIT_P_VALUE)->SetFocus();
                return;
            }

            // 校验重复值
            for (size_t i = 0; i < m_vecMultiP.size(); i++)
            {
                if (m_vecMultiP[i] == nP)
                {
                    CString strErr;
                    strErr.Format(_T("多p值中存在重复值%d！请确保每个p值唯一。"), nP);
                    AfxMessageBox(strErr);
                    GetDlgItem(IDC_EDIT_P_VALUE)->SetFocus();
                    return;
                }
            }

            m_vecMultiP.push_back(nP);
            strToken = strTemp.Tokenize(_T(","), nPos);
        }
        // 校验：多p值列表不能为空，且必须至少包含2个值
        if (m_vecMultiP.size() < 2)
        {
            AfxMessageBox(_T("多p值误差分析需要至少2个p值！请用逗号分隔（如2,3,4）"));
            GetDlgItem(IDC_EDIT_P_VALUE)->SetFocus();
            return;
        }
    }

    CDialogEx::OnOK();  // 校验通过，关闭对话框
}