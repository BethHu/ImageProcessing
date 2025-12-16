# 理想高通滤波器工作流问题修复

## 问题描述
在频谱显示对话框点击"下一步"后，程序直接跳到反变换对话框，没有显示理想高通滤波器对话框。

## 根本原因
**CFourierSpectrumDlg::OnOK() 函数中直接创建并显示了 CInverseDFTDlg，导致工作流被打断。**

### 原有代码（错误）：
```cpp
void CFourierSpectrumDlg::OnOK()
{
	CInverseDFTDlg dlg(this);
	dlg.DoModal();     // ❌ 直接显示反变换对话框，跳过了理想高通滤波器
}
```

这会导致：
```
点击"理想高通滤波器"按钮
  ↓
CFourierInputDlg（输入对话框）
  ↓
CFourierSpectrumDlg（频谱对话框）
  ↓ 点击"下一步"
  ↓
CFourierSpectrumDlg::OnOK() 直接显示 CInverseDFTDlg ❌ 跳过了理想高通滤波器！
  ↓
CInverseDFTDlg（反变换对话框）
```

## 解决方案
**修改 CFourierSpectrumDlg::OnOK() 以仅关闭对话框，而不直接显示反变换对话框。**

### 新代码（正确）：
```cpp
void CFourierSpectrumDlg::OnOK()
{
	// 仅关闭频谱对话框，返回到调用者
	// 不要在这里直接显示反变换对话框，让调用者（myAlgDlg）继续工作流
	CDialogEx::OnOK();   // ✅ 只是关闭这个对话框
}
```

现在工作流正确：
```
点击"理想高通滤波器"按钮
  ↓
OnClickedButtonIdealhighpass() 中的工作流开始
  ↓
CFourierInputDlg（输入对话框）→ 点击"下一步"
  ↓
CFourierSpectrumDlg（频谱对话框）→ 点击"下一步"
  ↓
CFourierSpectrumDlg::OnOK() 只关闭对话框，返回控制权
  ↓
继续执行 OnClickedButtonIdealhighpass() 中的代码
  ↓
CIdealFilterDlg（理想高通滤波器对话框）✅ 现在正确显示！
  ↓
CInverseDFTDlg（反变换对话框）
  ↓
显示最终结果
```

## 修改的文件
- **FourierSpectrumDlg.cpp** - OnOK() 函数

## 现在的用户体验
✅ 点击"理想高通滤波器"按钮  
✅ 进入输入对话框，选择/生成图像，点击"下一步"  
✅ 进入频谱对话框，显示 DFT，点击"下一步"  
✅ **现在正确显示理想高通滤波器对话框** ⭐  
  - 显示截止频率编辑框（默认 0.3）
  - 可以调整参数
  - 可以点击"预览"查看效果
  - 点击"确定"应用滤波
✅ 进入反变换对话框，显示滤波后的结果

## 原理说明
理想高通滤波器的完整工作流应该由 OnClickedButtonIdealhighpass() 函数控制，它按顺序显示四个对话框：

1. **CFourierInputDlg** - 获取输入图像
2. **CFourierSpectrumDlg** - 计算DFT并显示频谱
3. **CIdealFilterDlg** - **让用户调整滤波参数** ← 关键！
4. **CInverseDFTDlg** - 使用过滤后的数据进行反变换

如果 CFourierSpectrumDlg 在 OnOK() 中直接显示反变换对话框，就会跳过第 3 步，导致用户无法调整滤波参数。

