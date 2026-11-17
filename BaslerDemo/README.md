# BaslerDemo — Qt + Pylon 二次开发示例教程

一个完整的巴斯勒(Basler)工业相机二次开发最小工程:

- **工作线程取图**(界面永不卡顿)
- **QGraphicsView 实时显示**(可拖拽查看,自适应缩放)
- **曝光 / 增益 / 触发控制界面**(连续采集 + 软触发单帧)

**本机环境(已验证编译通过)**:
| 组件 | 版本与位置 |
|---|---|
| pylon SDK | 6.1,`D:\Basler\pylon 6`(头文件在 `Development\include`,DLL 在 `Runtime\x64`) |
| Qt | 5.9.9 msvc2017_64,`D:\Qt\5.9.9\msvc2017_64` |
| Visual Studio | 2022 Professional,`D:\Microsoft Visual Studio\2022\Professional` |

---

## 1. 架构总览

```
┌─────────────── UI 线程 ───────────────┐   ┌──────── 工作线程(QThread)────────┐
│  MainWindow                          │   │  GrabWorker                        │
│   ├─ 按钮/输入框 ──信号槽(队列)──►──────┼──►│   ├─ openCamera / closeCamera      │
│   │                                  │   │   ├─ setExposure / setGain / 触发   │
│   ├─ QGraphicsView ◄──frameReady(QImage)◄─┼──┤   └─ QTimer(5ms) → pump()       │
│   ├─ FPS/状态栏  ◄──fpsChanged/error ──◄─┼──┤        └─ RetrieveResult() 取图   │
│   └─ spinbox 回显 ◄──exposureApplied ──◄─┼──┤            └─ toQImage() 深拷贝    │
└──────────────────────────────────────┘   └─────────────────────────────────────┘
              ▲                                            ▲
              └──── 所有相机操作只发生在工作线程 ────────────┘
```

核心原则两条:

1. **相机操作全部在工作线程执行**。UI 通过信号槽"发指令"(`openCamera`、`setExposureUs`…),
   工作线程通过信号"送结果"(`frameReady`、`fpsChanged`…)。跨线程的信号槽自动变成队列连接,
   两边互不阻塞。
2. **跨线程只传 QImage 的深拷贝**。pylon 的取图缓冲区会被回收复用,QImage 不拷贝就传出
   必然花屏/崩溃;另外 QPixmap 只能在 GUI 线程创建,所以传 QImage、到 UI 线程再转 QPixmap。

## 2. 构建与运行(一键)

### 2.1 构建

双击 **`build.bat`**(或在命令行执行)。它会:
调用 VS2022 的 vcvars64 环境 → 用 Qt 5.9 的 qmake 生成 Makefile → nmake 编译,
成功后产物在 `release\BaslerDemo.exe`。

### 2.2 运行

双击 **`run.bat`** —— 它把 Qt 的 bin 和 pylon 的 `Runtime\x64` 临时加进 PATH 再启动,
不用改系统环境变量。

> 重新编译前记得先关掉正在运行的程序窗口(文件会被占用)。

### 2.3 手动构建(可选)

在 **"x64 Native Tools Command Prompt for VS 2022"** 里:

```bat
cd /d e:\basler二次开发\BaslerDemo
"D:\Qt\5.9.9\msvc2017_64\bin\qmake.exe" BaslerDemo.pro
nmake
```

### 2.4 用 Qt Creator(可选)

Qt Creator 4.x 识别不了 VS2022,需要手动加编译器(工具→选项→Kits→编译器→
添加 MSVC,指向 `VC\Tools\MSVC\xx\bin\Hostx64\x64\cl.exe`),再建 Kit:
Qt 版本选 `D:\Qt\5.9.9\msvc2017_64\bin\qmake.exe`,套上该编译器。
嫌麻烦就直接用 build.bat,效果一样。

## 3. 界面操作说明

| 步骤 | 操作 | 说明 |
|---|---|---|
| 1 | 点"打开相机" | 枚举设备并打开第一台,状态栏显示型号/序列号 |
| 2 | 点"开始采集" | 连续模式下画面以相机帧率实时刷新 |
| 3 | 拖动"曝光时间" | 单位微秒,实时生效;图像越暗就把值调大 |
| 4 | 拖动"增益" | dB,暗处可适当加大,但会引入噪点 |
| 5 | 选"软触发(单帧)" | 画面停住;每点一次"单帧触发"出一帧 |
| 6 | 点"停止采集"→"关闭相机" | 也可以直接关窗口,程序会自动释放相机 |

## 4. 代码导读

### 4.1 `main.cpp` — pylon 生命周期

`PylonInitialize()` 必须在任何 pylon 对象之前调用;`PylonTerminate()` 必须在所有相机对象
析构**之后**调用,所以用了嵌套作用域:窗口全部销毁完才 Terminate。

### 4.2 `GrabWorker` — 取图核心

| 方法 | 作用 |
|---|---|
| `openCamera()` | `CTlFactory::EnumerateDevices` 枚举 → `CreateFirstDevice` → `Open()`;失败发 `errorOccurred` |
| `startGrabbing()` | 先写触发参数(`TriggerSelector=FrameStart`、`TriggerSource=Software`、`TriggerMode=On/Off`),再 `StartGrabbing(LatestImageOnly)`,启动 5ms 泵图定时器 |
| `pump()` | 每 5ms 调一次 `RetrieveResult(1ms, Return)`,取到就 `frameReady(深拷贝的 QImage)` |
| `setExposureUs()` | `CFloatParameter(nodemap, "ExposureTime").SetValue(us)`,然后**读回**实际值发 `exposureApplied`(相机可能取整/截断) |
| `executeSoftwareTrigger()` | `camera->ExecuteSoftwareTrigger()`,必须在 `TriggerMode=On` 且正在采集时调用 |
| `stopGrabbing()/closeCamera()` | 先停定时器,再 `StopGrabbing()`/`Close()` |

为什么用"定时器泵图"而不是在循环里 `while(true) RetrieveResult()`?
因为工作线程的**事件循环必须保持通畅**才能接收 UI 指令(改曝光、停采集、软触发)。
阻塞式死循环会让这些指令永远排队排不上。5ms 定时 + 1ms 非阻塞取图,延迟对人眼不可感。

> 进阶:想要更低开销,可改用 pylon 的 `CImageEventHandler`(OnImageGrabbed 回调,由 pylon
> 内部取图线程驱动),见第 6 节扩展练习。

### 4.3 `toQImage()` — 像素格式转换

- Mono8 → 灰度图,RGB8 → `Format_RGB888`,BGR8 → `Format_BGR888`。
- **Qt 版本兼容**:本机 Qt 5.9 没有 `Format_Grayscale8`(Qt 5.13+)和 `Format_BGR888`
  (Qt 5.14+),所以代码里用 `QT_VERSION` 做了分支:老版本下灰度图走
  `Indexed8 + 灰度调色板`,BGR8 走格式转换器转 RGB8。升级 Qt 后自动启用新快速路径。
- 其它像素格式(Bayer、Mono12 等)用 `CImageFormatConverter` 统一转成 RGB8 再包 QImage。
- **每个分支都要 `.copy()`**:QImage 包着的是 pylon 的缓冲区,grabResult 释放后缓冲区
  就被复用,不拷贝 = 界面读到脏内存。

### 4.4 `MainWindow` — 信号槽连接

UI→工作线程(自动队列连接):

| 控件 | 信号 | GrabWorker 槽 |
|---|---|---|
| 打开相机 | `clicked` | `openCamera` |
| 开始/停止采集 | `clicked` | `startGrabbing` / `stopGrabbing` |
| 单帧触发 | `clicked` | `executeSoftwareTrigger` |
| 软触发单选 | `toggled(bool)` | `setTriggerMode(bool)` |
| 曝光/增益 | `valueChanged(double)` | `setExposureUs` / `setGainDb` |

工作线程→UI:`frameReady→onFrame`(QImage→QPixmap 放进 QGraphicsScene)、
`exposureApplied/gainApplied→回显`(用 `QSignalBlocker` 防止回显再触发一次设置)。

### 4.5 关闭流程

`closeEvent → shutdownWorker()`:
`BlockingQueuedConnection` 同步调用 `stopGrabbing`、`closeCamera`(保证相机释放)→
`thread->quit()` + `wait()` → `delete m_worker`。之后 `main` 里才 `PylonTerminate()`。

## 5. 踩坑记录(本工程实测)

| 现象 | 原因 | 解决 |
|---|---|---|
| `error C2447 "{" 缺少函数标题` 等一堆怪错 | 源码是无 BOM UTF-8,MSVC 默认按 GBK 读中文注释,把后面的代码吞了 | `.pro` 里加 `QMAKE_CXXFLAGS += /utf-8`(已加) |
| `C3861: CEnumerationParameter 找不到` | **pylon 6 叫 `CEnumParameter`,pylon 7 才改名 `CEnumerationParameter`** | 用 `CEnumParameter`(已按 6.x 写) |
| `C2065: INodeMap 未声明` | `INodeMap` 在 `GenApi` 命名空间,不在 `Pylon` | 写 `GenApi::INodeMap`(已改) |
| 编译通过但启动报缺 `Qt5Core.dll`/`PylonBase_v6_1.dll` | Qt bin 和 pylon `Runtime\x64` 不在 PATH | 用 `run.bat` 启动(已内置 PATH) |
| 打开相机报"资源被占用" | pylon Viewer 开着 | 关掉 pylon Viewer |
| 一直没画面 | 停在软触发模式没触发 / 曝光太小 | 切回"连续采集";曝光调大 |
| 链接 `LNK1181` 找不到 pylon 库 | pylon 版本变了,库名带版本号 | 按 `Development\lib\x64` 里的实际文件名改 `.pro` 的 `LIBS` |

## 6. 扩展练习(由浅入深)

1. **保存一帧**:在 `onFrame` 里加 `frame.save("frame.bmp")`(加个按钮控制,别每帧都存)。
2. **限制帧率**:界面加一个帧率输入框,写参数前先 `CBooleanParameter(nodemap,
   "AcquisitionFrameRateEnable").SetValue(true)` 再设 `AcquisitionFrameRate`。
3. **事件驱动取图**:把定时泵换成 `camera->RegisterImageEventHandler(...)`,
   在 `OnImageGrabbed` 回调里转换并发射 `frameReady`(注意回调运行在 pylon 内部线程,
   发信号是安全的,槽仍会在 UI 线程执行)。
4. **多相机**:openCamera 时枚举出列表让用户选,给每台相机各建一个 GrabWorker+QThread。
5. **接入 OpenCV**:把 `toQImage` 换成 `cv::Mat`,跑边缘检测/模板匹配后再显示。

## 7. 文件清单

| 文件 | 内容 |
|---|---|
| `BaslerDemo.pro` | qmake 工程:pylon 路径、库链接、`/utf-8` |
| `build.bat` | 一键构建(VS2022 + Qt5.9 qmake + nmake) |
| `run.bat` | 一键运行(自动补 Qt/pylon DLL 路径) |
| `main.cpp` | 入口:pylon 初始化/释放、启动窗口 |
| `GrabWorker.h/.cpp` | 取图工作对象(独立线程,全部相机操作) |
| `MainWindow.h/.cpp` | 控制面板 + QGraphicsView 显示 |

## 8. 换环境/换版本时要改什么

- **pylon 7**:库名按新版 `lib\x64` 实际名字改 `.pro` 的 `LIBS`;`CEnumParameter` 改名
  `CEnumerationParameter`(或加一行 typedef);SDK 目录布局若无 `Development\` 层,
  `.pro` 里的分支已兼容两种布局。
- **Qt 5.14+**:`toQImage` 里的版本分支会自动启用 `Format_Grayscale8`/`Format_BGR888`
  快速路径,无需改动。
- **只用 Visual Studio 不用命令行**:装 Qt VS Tools 扩展(支持 VS2022 的 2.7+),
  Qt VS Tools → Open Qt Project File 打开 `BaslerDemo.pro` 即可。
