﻿# myAlg 图像处理程序使用说明（完整版）

> 本 README 适用于当前仓库中“完成 Otsu 算法之后”的版本，覆盖所有已经实现并在主界面可用的功能：空间域滤波、直方图处理、傅里叶频谱分析、理想/巴特沃斯/高斯高通滤波、Wedge 条纹去噪以及 Otsu 图像分割等。

---

## 1. 环境与工程说明

- 操作系统：Windows
- 开发工具：Visual Studio（工程最初为 VS2010，推荐 VS2010 及以上版本）
- 解决方案入口：根目录下的 `myAlg.sln`
- 主要工程：`myAlg`（MFC 对话框应用）

构建步骤：

1. 打开 Visual Studio → `File` → `Open` → `Project/Solution...`
2. 选择本仓库根目录下的 `myAlg.sln`
3. 选择编译配置（Debug 或 Release，Win32）
4. `Build` → `Build Solution`
5. 运行：`Debug` → `Start Debugging` 或直接运行生成的 `myAlg.exe`

---

## 2. 主界面与整体工作流

主界面是一个 MFC 对话框，核心工作流非常简单：

1. 使用 **Read** 按钮读取输入图像（存入内部的 `imgIn`）并显示；
2. 选择任意一种处理功能（直方图、中值滤波、双边滤波、傅里叶变换等）；
3. 处理结果会被写入 `imgOut`，并根据功能弹出或更新图像显示窗口；
4. 使用 **Write** 按钮将 `imgOut` 中的结果图像保存到磁盘。

> 约定：所有“流程型”功能（如理想高通、Wedge 去条带、Otsu 分割）在流程结束时都会把最终结果复制到主界面的输出图像缓冲区 `imgOut`，统一由 **Write** 按钮负责写盘。

主界面主要按钮与对应功能如下表：

| 按钮 | 功能概述 |
|------|----------|
| **Read** (`ID_BUTTON_READ`) | 读取输入图像到 `imgIn` 并显示 |
| **Write** (`IDC_BUTTON_WRITE`) | 将 `imgOut` 保存到文件，并预览 |
| **Hist** (`IDC_BUTTON_HIST`) | 直方图均衡化 |
| **HistMatch** (`IDC_BUTTON_hismatch`) | 直方图匹配（指定参考图像） |
| **Median** (`IDC_BUTTON_median`) | 中值滤波（可调窗口大小） |
| **Bilateral Filter** (`IDC_BUTTON_BilateralFilter`) | 双边滤波（σs、σr、窗口可调） |
| **Laplacian** (`IDC_BUTTON_Laplacian`) | 拉普拉斯锐化 |
| **Fourier** (`IDC_BUTTON_Fourier`) | 基本傅里叶变换与频谱显示 |
| **Ideal High-pass** (`IDC_BUTTON_IDEALHIGHPASS`) | 理想/巴特沃斯/高斯高通滤波完整工作流 |
| **Wedge** (`IDC_BUTTON1`) | Wedge 频域条纹去噪工作流 |
| **Otsu** (`IDC_BUTTON_OSTU`) | Otsu 图像分割（单/多阈值，含预处理和直方图） |

以下章节分别介绍各个模块的使用方法。

---

## 3. 读写图像：Read / Write

### 3.1 Read（读取输入图像）

- 点击 **Read**：弹出标准文件打开对话框；
- 选择待处理图像（支持常见格式，如 BMP/JPG/PNG 等）；
- 成功后：
   - 文件路径会显示在主对话框的输入路径编辑框；
   - 图像数据读入内部缓冲 `imgIn`；
   - 若为彩色图（通道数 ≥ 3），以 RGB 模式显示；否则灰度显示。

### 3.2 Write（保存输出图像）

- 处理完成后，结果会写入 `imgOut`；
- 点击 **Write**：
   - 选择保存路径和文件名；
   - 成功保存后，输出路径更新到主界面编辑框；
   - 会弹出结果显示窗口展示 `imgOut`。

> 提示：包括理想高通、Wedge 去条纹、Otsu 分割在内的流程型功能，结束时都会自动把重建/分割结果复制到 `imgOut`，此时直接使用 **Write** 即可保存。

---

## 4. 空间域直方图与滤波功能

### 4.1 直方图均衡化（Hist）

- 按钮：**Hist**
- 前置条件：`imgIn` 非空（已通过 Read 读入图像）。
- 行为：
   - 调用 `CImageProcessingEx::histeq(imgIn, imgOut)`；
   - 将直方图均衡化后的结果写入 `imgOut`；
   - 自动弹出结果显示窗口（彩色或灰度显示）。
- 保存：使用主界面 **Write** 保存 `imgOut`。

### 4.2 直方图匹配（HistMatch）

- 按钮：**HistMatch**
- 前置条件：`imgIn` 非空。
- 使用步骤：
   1. 点击 **HistMatch**；
   2. 选择一幅“参考图像”；
   3. 程序先显示参考图像预览；
   4. 调用 `CImageProcessingEx::histMatch(imgIn, refImage, imgOut)` 对 `imgIn` 做直方图匹配；
   5. 显示匹配结果图，并提示“请使用 Write 按钮保存结果”。

### 4.3 中值滤波（Median）

- 按钮：**Median**
- 前置条件：`imgIn` 非空。
- 使用步骤：
   1. 点击 **Median**；
   2. 弹出 `CMedianFilterDlg` 参数对话框，设置窗口大小（必须为奇数，如 3、5、7...）；
   3. 点击确定后，调用 `CImageProcessingEx::medianFilter(imgIn, imgOut, windowSize)`；
   4. 显示滤波结果。

### 4.4 双边滤波（Bilateral Filter）

- 按钮：**Bilateral Filter**
- 前置条件：`imgIn` 非空。
- 参数对话框（`BilateralFilter`）：
   - 空间域标准差 `σs`（控制空间距离权重）；
   - 灰度域标准差 `σr`（控制灰度差异权重）；
   - 滤波窗口大小（奇数）。
- 流程：
   1. 设置参数后点击确定；
   2. 调用 `CImageProcessingEx::bilateralFilter(imgIn, imgOut, sigmaS, sigmaR, windowSize)`；
   3. 显示结果并提示“请使用保存按钮保存结果”。

### 4.5 拉普拉斯锐化（Laplacian）

- 按钮：**Laplacian**
- 前置条件：`imgIn` 非空。
- 行为：
   - 调用 `CImageProcessingEx::laplacianSharpen(imgIn, imgOut)`（或扩展版本）；
   - 增强边缘和细节；
   - 显示锐化结果并提示可保存。

---

## 5. 傅里叶变换与频谱分析（Fourier）

该模块对应 `IDC_BUTTON_Fourier`，主要用于：

- 生成或读取一幅图像；
- 计算 2D DFT；
- 在 `CFourierSpectrumDlg` 中显示：
   - 幅度谱（可选对数变换、归一化、居中/非居中显示）；
   - 相位谱（多种相位范围）；
   - 角谱 / 径向谱；
- （可选）进一步进入逆变换与误差分析（通过 InverseDFTDlg）。

### 5.1 使用步骤

1. 点击 **Fourier** 按钮；
2. `CFourierInputDlg`：
    - 选择“生成图像”或“读取图像”；
    - 若生成图像，则指定大小等参数；
    - 完成后生成 `m_inputImage`；
3. 若选择 Fourier 继续：
    - 弹出 `CFourierSpectrumDlg`：
       - 自动计算 DFT（保留居中与非居中两个版本）；
       - 勾选 **幅度谱 / 相位谱 / 角谱 / 径向谱** 中的一种或多种；
       - 根据需要设置：对数变换、归一化范围、角/径向 bin 数等；
       - 点击 **Apply** 按钮显示选择的谱图。

> 说明：更详细的实现细节、误差分析与逆变换接口见 `FourierSpectrumDlg.cpp` 及相关注释。

---

## 6. 理想 / 巴特沃斯 / 高斯高通滤波工作流

该工作流从主界面的 **Ideal High-pass** 按钮（`IDC_BUTTON_IDEALHIGHPASS`）开始，完整流程包括：

1. **傅里叶输入选择**（`CFourierInputDlg`）；
2. **频谱显示与 DFT 计算**（`CFourierSpectrumDlg`，自动计算并可查看频谱）；
3. **高通滤波器参数设置**（`CIdealFilterDlg`）；
4. **逆变换与重建图像**（`CInverseDFTDlg`），重建结果复制到 `imgOut`；
5. 返回主界面，使用 **Write** 保存结果。

### 6.1 滤波器类型与参数

在 `CIdealFilterDlg` 中可选择三种高通滤波器：

- 理想高通（Ideal High-pass）
- 巴特沃斯高通（Butterworth High-pass）
- 高斯高通（Gaussian High-pass）

主要参数：

- 截止频率 \( D_0 \)：
   - 通常归一化到 \([0, 0.5]\) 或 \([0.01, 0.49]\) 区间；
   - 可通过滑块与编辑框联动设置；
- 巴特沃斯阶数 \( n \)：
   - 仅在巴特沃斯模式下启用；
   - 一般取 1–10；
- （可选）High-boost 系数：
   - 用于适当增强高频成分，改善重建亮度与细节。

### 6.2 使用步骤（概要）

1. 点击 **Ideal High-pass** 按钮；
2. 在输入对话框中生成/读取图像 → 下一步；
3. 在频谱对话框中查看居中频谱 → 下一步；
4. 在滤波器对话框中：
    - 选择滤波器类型（理想/巴特沃斯/高斯）；
    - 设置截止频率等参数；
    - 点击 **预览** 查看频谱效果（居中幅度谱）；
    - 点击 **确定** 应用滤波；
5. 在逆变换对话框中执行 IDFT，生成空间域图像；
6. 流程结束后，重建结果已复制到主界面 `imgOut`，可使用 **Write** 保存。

> 更详细的实现与调试说明，请参考：
> - `IDEAL_FILTER_USAGE.md` – 使用指南
> - `IDEAL_FILTER_IMPLEMENTATION_GUIDE.md` – 实现细节
> - `WORKFLOW_DESIGN.md` / `TEST_CHECKLIST.md` / `PROBLEM_SOLVED.md` – 工作流设计与问题修复记录

---

## 7. Wedge 条纹去噪滤波工作流

该工作流从主界面的 **Wedge** 按钮开始，处理目标是：去除图像中沿某一固定方向分布的条纹噪声，对应在频域中的楔形高能谱区。

### 7.1 整体流程

1. `CFourierInputDlg`：选择/生成输入图像；
2. `CFourierSpectrumDlg`：计算并显示居中 DFT 频谱；
3. `CWedgeFilterDlg`：
    - 设置楔形角度范围（起始角、结束角）；
    - 设置半径（频率半径上限，可通过滑条调节）；
    - （可选）自动检测条纹主方向；
    - 点击 **Preview** 在频谱上预览楔形抑制效果；
    - 点击 **OK** 应用滤波，生成过滤后的 DFT；
4. `CInverseDFTDlg`：对过滤后的 DFT 做 IDFT，得到去条纹结果；
5. 将重建结果复制到主界面 `imgOut`，提示可通过 **Write** 保存。

### 7.2 关键参数

- 起始角 / 结束角（度）：
   - 定义楔形扇区的角度范围；
   - 程序内部会自动计算中心角与半角，并在 \([-180°, 180°]\) 上归一化；
- 半径（像素）：
   - 若半径 ≤ 0，则默认作用到频谱边缘；
   - 若半径 > 0，则仅抑制距离中心小于该半径的频率点；
- 自动检测:
   - 在居中 DFT 上按角度统计能量，估计条纹主方向，并自动填入角度范围初值。

> 详细算法和调参建议，见根目录的 `readme_wedge.md`。

---

## 8. Otsu 图像分割

**入口按钮**：主界面 **Otsu**（`IDC_BUTTON_OSTU`）

### 8.1 功能特性概述

在 `COtsuDlg` 中实现了如下功能：

- 输入图像通道选择：
   - 自动灰度：按标准加权或内部规则将 RGB 自动转换为灰度；
   - 手动选择 RGB 通道：指定通道索引进行灰度构建；
- 预处理：通过组合框选择：
   - 不平滑；
   - 高斯平滑（可设置核大小与 σ）；
   - 中值滤波（可设置窗口大小）；
- 阈值类型：
   - 单阈值 Otsu（经典二值分割）；
   - 多阈值 Otsu（阈值个数可设置，例如 2–5 阶分割）；
- 结果显示：
   - 原始图像；
   - 灰度直方图，并在直方图上用竖线标出阈值位置（单/多阈值）；
   - 分割结果图（多级灰度）。
- 预览与确认：
   - **预览按钮**：按当前参数计算并仅显示分割结果，不写回主界面；
   - **确定按钮**：
      - 计算并显示原图 + 直方图 + 分割结果；
      - 将分割结果复制到主界面 `imgOut`；
      - 不关闭 Otsu 参数对话框，便于继续调参；
   - 关闭 Otsu 对话框时，其子图像窗口（原图/直方图/结果）会一并关闭。

### 8.2 使用步骤

1. 确保已通过 **Read** 读入图像（支持灰度和彩色）；
2. 点击 **Otsu** 按钮，打开 `COtsuDlg`：
    - 若输入为彩色图，可选择自动灰度或指定 RGB 通道；
    - 在预处理组合框中选择“不平滑 / 高斯 / 中值”，并按需要设置核大小、σ 等参数；
    - 选择“单阈值”或“多阈值”，多阈值时指定阈值数量；
3. 点击 **预览**：
    - 快速查看当前参数下的分割结果图像；
4. 点击 **确定**：
    - 显示原图、直方图（含阈值线）和最终分割结果；
    - 分割结果复制到主界面 `imgOut`，会有提示“点击 Write 可保存”；
    - 参数对话框保持打开，可继续尝试其他参数；
5. 关闭 Otsu 对话框后，相关图像窗口会同时关闭；
6. 在主界面使用 **Write** 保存分割结果。

---

## 9. 常见问题与调试建议

- 所有需要输入图像的功能（Hist / HistMatch / Median / Bilateral / Laplacian / Otsu 等）在 `imgIn.empty()` 时都会提示先使用 **Read**；
- 频域相关功能（Fourier / Ideal High-pass / Wedge）内部都带有丰富的 `TRACE` 和 `AfxMessageBox` 提示，可通过 Visual Studio 的 **Output → Debug** 窗口查看；
- 理想高通工作流相关的已知问题与修复过程，见：
   - `TEST_CHECKLIST.md` – 测试步骤与成功标志；
   - `PROBLEM_SOLVED.md` – 工作流中断问题（直接跳到反变换）已通过修改 `CFourierSpectrumDlg::OnOK` 解决；
- 若保存结果出现全黑或非常暗的情况：
   - 确认对应频域流程中已做 IDFT 和归一化；
   - 检查截止频率/楔形角度是否设置得过于极端；
- 若界面或对话框行为与说明不符，可优先检查对应 `.cpp` 源文件中的消息映射与控制逻辑。

---

## 10. 相关文档一览

本仓库中还有若干专门文档，对特定功能有更详细的说明和设计记录：

- `IDEAL_FILTER_IMPLEMENTATION_GUIDE.md` – 理想/巴特沃斯/高斯高通滤波器的实现指南
- `IDEAL_FILTER_USAGE.md` – 理想高通工作流的使用说明
- `WORKFLOW_DESIGN.md` – 理想滤波器整体工作流设计文档
- `TEST_CHECKLIST.md` – 理想高通滤波器工作流测试清单
- `PROBLEM_SOLVED.md` – 频谱对话框跳过滤波器问题的修复记录
- `readme_wedge.md` – Wedge 条纹去噪滤波的详细说明

如需在现有基础上继续扩展（例如增加其它空间域滤波器、更多频域滤波器，或增强 Otsu 结果可视化），可以在本 README 所概述的工作流基础上增量开发。
