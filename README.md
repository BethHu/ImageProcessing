# ImageProcessing（MFC 图像处理实验工程）

> 该仓库是一个基于 MFC 的 Windows 桌面程序工程，包含若干经典数字图像处理算法的参数对话框与可视化展示。
>
> **2026-01：新增“能量谱（功率谱）”显示**：在傅里叶谱显示对话框中增加“能量谱”选项，且其显示参数与幅度谱**联动复用**（中心化/非中心化、对数变换、归一化与显示范围）。

## 1. 构建与运行

- 平台：Windows
- 工具链：Visual Studio（建议 VS2019/VS2022）
- 依赖：MFC（本工程为 MFC Dialog App）
- 打开方式：直接用 VS 打开 `myAlg.vcxproj`，选择 `Debug|Win32`（或你的目标配置）后生成并运行。

## 2. 功能总览

主窗口提供图像读写与多种算法入口（按钮/对话框）。各算法通常遵循：**输入图像 → 参数对话框 → 处理 → 结果显示/写盘**。

包含模块（按文件/对话框命名归类）：

- 中值滤波：`MedianFilterDlg`
- 双边滤波：`BilateralFilter`
- 傅里叶变换与频谱显示：`FourierInputDlg` + `FourierSpectrumDlg` + `InverseDFTDlg`
- 频域高通滤波（理想/巴特沃斯/高斯）：`IdealFilterDlg`
- Wedge 条纹/噪声角度抑制：`WedgeFilterDlg`
- Otsu 阈值分割（单阈值/多阈值）：`OtsuDlg`
- Canny 边缘检测参数：`CannyParamDlg`
- PCA（主成分）与误差分析/图表：`PcaPParamDlg` + `PcaChartDlg`
- RGB↔IHS 分解/显示：`RgbIhsParamDlg`

> 说明：下面的“参数范围”优先以代码中的 `DDV_MinMax*`、显式 clamp、或 `AfxMessageBox` 校验为准；若代码未强制限制，会明确标注为“建议”。

---

## 3. 各功能说明与参数范围

### 3.1 中值滤波（Median Filter）

用途：去除椒盐噪声等脉冲噪声。

参数：
- **窗口大小 `windowSize`（像素）**：`1 ~ 15`，且**必须为奇数**（如 3/5/7/…）。
  - 默认：3
  - 约束来源：`DDV_MinMaxInt(1,15)` + “必须奇数”校验

输出：滤波后图像。

---

### 3.2 双边滤波（Bilateral Filter）

用途：在平滑的同时尽量保持边缘。

参数：
- **空间域 σs `sigmaS`**：`0 ~ 100`（浮点）
- **强度域/颜色域 σr `sigmaR`**：`0 ~ 100`（浮点）
- **窗口大小 `windowSize`（像素）**：`1 ~ 15`（整数）
  - 备注：代码里未强制要求奇数，但双边滤波通常建议使用奇数窗口（3/5/7…）。

输出：滤波后图像。

---

### 3.3 傅里叶变换：输入选择（Fourier Input）

用途：选择/生成测试图像作为傅里叶分析输入。

模式：
- **生成测试图像**：矩形 / 正弦波 / 棋盘格
- **读取已有图像**：支持 `*.bmp;*.jpg;*.png`

通用参数（生成模式）：
- **图像宽度 `genWidth`**：`32 ~ 2048`（整数）
- **图像高度 `genHeight`**：`32 ~ 2048`（整数）

矩形图像参数：
- **矩形宽度 `rectWidth`**：`> 0`（整数）
- **矩形高度 `rectHeight`**：`> 0`（整数）
- **背景灰度 `bgColor`**：`0 ~ 255`（整数）
- **矩形灰度 `rectColor`**：`0 ~ 255`（整数）

正弦波图像参数：
- **频率 `sineFrequency`**：`> 0`（浮点）
- **相位 `sinePhase`**：代码未限制（浮点，单位由实现决定；通常可用弧度）。

棋盘格图像参数：
- **方格大小 `checkerSize`**：`> 0`（整数）

读取模式参数：
- **文件路径 `readImagePath`**：通过“浏览...”选择；读取失败会提示并清空路径。

---

### 3.4 傅里叶谱显示（Magnitude / Energy / Phase / Angular / Radial）

用途：对输入图像计算 2D DFT，并按选项显示各类频谱。

可选谱类型：
- **幅度谱**：$|F(u,v)|$
- **能量谱（功率谱）**（新增）：$|F(u,v)|^2$
- **相位谱**：$\angle F(u,v)$
- **角谱**：按角度 bin 聚合的频域统计
- **径向谱**：按半径 bin 聚合的频域统计

#### 3.4.1 幅度谱/能量谱（参数联动）

> 幅度谱与能量谱**共享同一套显示参数**：勾选任意一个后，这些参数控件都会启用，且对两者计算流程一致（仅基础值不同：$|F|$ vs $|F|^2$）。

共享参数：
- **非中心化 `center`**（勾选=非中心化；不勾选=中心化显示）
  - 作用：只影响显示使用的 DFT 数据（中心化/非中心化切换）。
- **对数变换 `logTransform`**（checkbox）
  - **底数 `logBase`**：默认 10.0（代码未硬性限制；建议 `> 0` 且 `!= 1`）
  - **偏移 `logOffset`**：默认 1.0（建议 `>= 0`；计算时使用 `log(value + offset)`，若 `value+offset<=0` 则置 0）
- **归一化 `normalize`**（checkbox）
  - 若勾选：
    - **显示最小值 `displayMin`**：默认 0.0（代码未硬性限制）
    - **显示最大值 `displayMax`**：默认 255.0（代码未硬性限制）
    - 行为：将当前数据线性映射到 `[displayMin, displayMax]`
  - 若不勾选：自动线性拉伸到 `0 ~ 255`

显示标题会标注“中心化/非中心化”。

#### 3.4.2 相位谱

参数：
- **相位范围**（单选）：
  - `[-π, π]`
  - `[0, 2π]`
  - `[-180°, 180°]`

输出：0~255 灰度映射的相位图。

#### 3.4.3 角谱

参数：
- **分 bin 数 `angularBins`**：`> 0`（默认 360；代码中若 `<=0` 则回退 360）
- **角度范围 `angularRange`**：界面提供（默认 360.0），当前实现中主要使用 `angularBins`（范围参数作为预留/显示用）。

输出：1×N 的角谱曲线图像（按实现显示）。

#### 3.4.4 径向谱

参数：
- **分 bin 数 `radialBins`**：`> 0`（默认 100；代码中若 `<=0` 则回退 100）
- **最大半径 `maxRadius`**：界面提供（默认 0.0），当前实现中主要使用 `radialBins`（该参数作为预留/显示用）。

输出：1×N 的径向谱曲线图像（按实现显示）。

---

### 3.5 逆傅里叶变换与误差分析（Inverse DFT）

用途：对当前频谱做 IDFT 得到重建图像，并计算误差指标。

输出：
- 重建图像
- 误差图
- 指标：MSE、PSNR、最大绝对误差

参数：无额外用户输入参数（按对话框按钮触发计算）。

---

### 3.6 频域高通滤波器（Ideal / Butterworth / Gaussian HP）

用途：在频域对 DFT 进行高通滤波，并可预览滤波后的幅度谱。

参数（所有滤波器类型共用）：
- **截止频率 `cutoffFrequency`**：会被钳制到 `0.01 ~ 0.49`（浮点）
  - 说明：此处为归一化频率的常见取值区间（避免 0 和接近 Nyquist 的极端值）

Butterworth 高通额外参数：
- **阶数 `order`**：会被钳制到 `1 ~ 10`（整数）

备注：界面上“Sigma 参数”目前在代码中未看到被读取参与计算（若后续接线，可在 README 更新）。

---

### 3.7 Wedge 滤波（条纹/特定角度噪声抑制）

用途：在中心化频谱中按角度扇区（wedge）将频率分量置零，用于抑制条纹噪声。

参数：
- **起始角 `startAngle`（度）**：浮点（代码未硬性限制）
- **终止角 `endAngle`（度）**：浮点（代码未硬性限制）
  - 内部计算：`center=(start+end)/2`，`half=|end-start|/2`
  - **半角 `halfAngle` 会被钳制到 `0.5° ~ 90°`**
- **楔形半径 `wedgeRadius`**：`0 ~ maxRadius`
  - `maxRadius = 0.5 * min(DFT宽, DFT高)`（若无 DFT 数据则默认 100）
  - 行为：当频点落在 wedge 角度内时，若 `r <= wedgeRadius` 则置零；若 `r > wedgeRadius` 则保留。

输出：可预览滤波后的幅度谱，并用于后续 IDFT。

---

### 3.8 Otsu 图像分割（单阈值 / 多阈值）

用途：Otsu 自动阈值分割，支持多阈值（多类）分割，并提供可选预处理。

参数：
- **通道选择**：
  - 自动灰度（RGB 先转灰度）或
  - 选择 RGB 通道：`1 ~ 3`（超出会被钳制回 1~3）
- **预处理方式**：不平滑 / 高斯 / 中值（来自下拉框）
- **卷积核大小 `kernelSize`**：`> 0`（整数）
  - 若输入为偶数，代码会 **自动 +1 调整为奇数**
- **高斯 σ `sigma`**：`> 0`（否则默认 1.0）
- **阈值类型**：
  - 单阈值
  - 多阈值：阈值个数 `thresholdCount` 会被钳制到 `2 ~ 5`

输出：分割结果图（灰度分层），并可复制回主窗口输出以便保存。

---

### 3.9 Canny 边缘检测（参数对话框）

用途：对输入图像执行 Canny 边缘检测，并提供预览。

参数：
- **Gaussian σ `sigma`**：`0.1 ~ 10.0`
- **低阈值 `lowThreshold`**：`0.0 ~ 2000.0`
- **高阈值 `highThreshold`**：`0.0 ~ 2000.0`
- 约束：`lowThreshold <= highThreshold`

输出：边缘图（预览/执行结果）。

---

### 3.10 PCA 参数（p 选择 / 多 p 误差分析）

用途：对多波段图像执行 PCA，并支持对不同 p 值做误差分析。

参数：
- 需要前置条件：必须先加载多波段图像（bandCount>0）
- **单 p 模式**：
  - 输入：一个整数 `p`
  - 范围：`1 ~ bandCount`
  - 限制：不允许包含逗号
- **多 p 模式**：
  - 输入：用逗号分隔的整数列表（支持中文逗号 `，`）
  - 每个 `p` 范围：`1 ~ bandCount`
  - 限制：不能重复；至少需要 2 个 p 值

输出：按实现显示 PCA/误差图表。

---

### 3.11 RGB ↔ IHS（显示选项）

用途：对 RGB 图像做 IHS 分解并选择性显示。

参数（checkbox）：
- 显示 I
- 显示 H
- 显示 S
- 显示重建 RGB

---

## 4. GitHub 上传（本地准备好的命令）

你要求上传到：`https://github.com/BethHu/ImageProcessing.git`。

在本机 PowerShell 里（仓库根目录）可按以下流程执行：

```powershell
cd f:\rsdiplib\code\myAlg\myAlg

# 初始化仓库（如还没有 .git）
git init -b main

git add .
git commit -m "Add energy(power) spectrum display and update docs"

git remote add origin https://github.com/BethHu/ImageProcessing.git

git push -u origin main
```

如果 push 提示需要认证：
- 推荐使用 GitHub Personal Access Token（PAT）作为密码
- 或在本机配置 Git Credential Manager

（我可以在你确认后，直接在当前目录帮你执行上述 git 命令到“push 失败/成功”为止，并把失败原因贴出来。）
