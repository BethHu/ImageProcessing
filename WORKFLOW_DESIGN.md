# 理想高通滤波器工作流程设计文档

## 📋 完整工作流程

```
用户点击"理想高通滤波器"按钮
                ↓
[1] CFourierInputDlg 对话框
    - 用户选择生成图像或读取图像
    - 输出: m_inputImage (CImageDataset)
                ↓
[2] CFourierSpectrumDlg 对话框  
    - 计算 DFT: CFourierTransform::DFT2D()
    - 输出: m_dftResult (std::vector<std::complex<double>>)
    - 可选: 显示频谱 (幅度、相位、角向、径向)
                ↓
[3] CIdealFilterDlg 对话框
    - 设置滤波参数:
      * 截断频率 (m_dCutoffFrequency)
      * 滤波类型 (低通/高通)
      * 强度 (0-1)
    - 执行滤波: ApplyIdealHighPassFilter() / ApplyIdealLowPassFilter()
    - 输出: m_filteredDFT (std::vector<std::complex<double>>)
                ↓
[4] CInverseDFTDlg 对话框
    - 执行反变换: CFourierTransform::IDFT2D()
    - 输出: 滤波后的图像
    - 显示结果
```

## 🔧 关键类和数据流

### 1. CFourierInputDlg
```cpp
public:
    CImageDataset m_inputImage;  // 输出：输入图像
```

### 2. CFourierSpectrumDlg
```cpp
public:
    std::vector<std::complex<double>> m_dftResult;  // 输出：DFT结果
    int m_dftWidth, m_dftHeight;                     // DFT尺寸
```

### 3. CIdealFilterDlg
```cpp
public:
    // 输入（从 CFourierSpectrumDlg 获取）
    CFourierSpectrumDlg* m_pSpectrumDlg;  
    
    // 参数
    double m_dCutoffFrequency;   // 截断频率
    int m_nFilterType;            // 0=低通, 1=高通
    double m_dFilterStrength;     // 强度 (0-1)
    
    // 输出
    std::vector<std::complex<double>> m_filteredDFT;  // 滤波后的DFT
    
    // 方法
    BOOL ApplyIdealHighPassFilter();   // 执行高通滤波
    BOOL ApplyIdealLowPassFilter();    // 执行低通滤波
    void OnBnClickedButtonPreview();   // 预览滤波后频谱
    void OnBnClickedButtonInverse();   // 进行反变换
```

### 4. CInverseDFTDlg
```cpp
public:
    // 新增成员：接收滤波后的DFT数据
    std::vector<std::complex<double>> m_filteredDFT;
    int m_dftWidth, m_dftHeight;
    
    // 使用m_filteredDFT而不是从m_pParentDlg获取
```

## 📝 实现细节

### 理想高通滤波算法
```cpp
void CIdealFilterDlg::OnBnClickedButtonApplyFilter()
{
    UpdateData(TRUE);  // 获取用户输入的参数
    
    // 计算到图像中心的距离
    int centerX = width / 2;
    int centerY = height / 2;
    
    // 对每个频域点应用滤波器
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            double distance = sqrt((x-centerX)² + (y-centerY)²);
            
            if (m_nFilterType == 1)  // 高通
            {
                if (distance < m_dCutoffFrequency)
                    m_filteredDFT[idx] *= (1.0 - m_dFilterStrength);
            }
            else  // 低通
            {
                if (distance > m_dCutoffFrequency)
                    m_filteredDFT[idx] *= (1.0 - m_dFilterStrength);
            }
        }
    }
}
```

## 🎯 消息处理流程

在 `myAlgDlg.cpp` 中：

```cpp
void CmyAlgDlg::OnClickedButtonIdealFilter()
{
    // 步骤1: 获取输入图像
    CFourierInputDlg inputDlg;
    if (inputDlg.DoModal() != IDOK) return;
    
    // 步骤2: 显示输入图像
    CImageDisplay::show(inputDlg.m_inputImage, ...);
    
    // 步骤3: 计算DFT
    CFourierSpectrumDlg spectrumDlg(&inputDlg.m_inputImage, this);
    if (spectrumDlg.DoModal() != IDOK) return;
    
    // 步骤4: 进行滤波
    CIdealFilterDlg filterDlg(&spectrumDlg, this);
    if (filterDlg.DoModal() != IDOK) return;
    
    // 完成
    AfxMessageBox(_T("理想滤波完成！"));
}
```

## 💾 文件修改清单

✅ 已创建/修改的文件：

1. **IdealFilterDlg.h** (新建)
   - CIdealFilterDlg 类定义
   - 滤波参数和方法

2. **IdealFilterDlg.cpp** (新建)
   - 滤波算法实现
   - 高通/低通滤波
   - 频谱预览

3. **myAlgDlg.h** (修改)
   - 添加 #include "IdealFilterDlg.h"
   - 添加 OnClickedButtonIdealFilter() 声明

4. **myAlgDlg.cpp** (修改)
   - 添加消息映射: ON_BN_CLICKED(IDC_BUTTON_IDEAL_FILTER, ...)
   - 实现 OnClickedButtonIdealFilter() 函数

5. **InverseDFTDlg.h** (修改)
   - 添加成员: m_filteredDFT, m_dftWidth, m_dftHeight
   - 支持接收滤波后的DFT数据

6. **InverseDFTDlg.cpp** (修改)
   - 构造函数初始化新成员变量

## 🚀 使用步骤

1. 在资源编辑器中：
   - 为 IDD_IDEALFILTER 对话框添加以下控件：
     * IDC_EDIT_CUTOFF_FREQ (编辑框) - 截断频率
     * IDC_RADIO_LOWPASS / IDC_RADIO_HIGHPASS (单选按钮) - 滤波类型
     * IDC_EDIT_FILTER_STRENGTH (编辑框) - 强度
     * IDC_BUTTON_APPLY_FILTER (按钮) - "应用滤波"
     * IDC_BUTTON_PREVIEW (按钮) - "预览频谱"
     * IDC_BUTTON_INVERSE (按钮) - "反变换"

2. 在主对话框添加按钮：
   - IDC_BUTTON_IDEAL_FILTER (按钮) - "理想高通滤波器"

3. 编译并测试

## 🔍 关键要点

1. **数据传递链**：
   - CFourierInputDlg → CFourierSpectrumDlg → CIdealFilterDlg → CInverseDFTDlg

2. **DFT 结果保留**：
   - CFourierSpectrumDlg 计算一次 DFT，不销毁
   - CIdealFilterDlg 复制 DFT 并进行滤波
   - CInverseDFTDlg 使用滤波后的 DFT 进行反变换

3. **错误处理**：
   - 各对话框验证输入数据有效性
   - 使用 try-catch 处理异常
   - TRACE 日志用于调试

4. **用户交互**：
   - 每个对话框完成后显示结果
   - 用户可以预览滤波后的频谱
   - 选择是否继续到反变换

