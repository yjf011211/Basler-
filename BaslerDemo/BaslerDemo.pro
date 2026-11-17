#-------------------------------------------------
# BaslerDemo — Qt + Basler Pylon 二次开发示例
# 工作线程取图 + QGraphicsView 实时显示 + 曝光/触发控制
#-------------------------------------------------
QT       += core gui widgets
TEMPLATE  = app
TARGET    = BaslerDemo
CONFIG   += c++17

# 源码为无 BOM 的 UTF-8(含中文注释/字符串),必须让 MSVC 按 UTF-8 解析,
# 否则中文会被按 GBK 误读并破坏注释和字符串
QMAKE_CXXFLAGS += /utf-8

# ===== Pylon SDK 路径 =====
# 优先用环境变量 PYLON_ROOT,否则用本机安装位置
PYLON_ROOT = $$(PYLON_ROOT)
isEmpty(PYLON_ROOT) {
    PYLON_ROOT = "D:/Basler/pylon 6"
}

# 兼容两种安装布局:新版在 Development/ 下,旧版直接在根目录下
exists("$$PYLON_ROOT/Development/include") {
    PYLON_INC    = $$PYLON_ROOT/Development/include
    PYLON_LIBDIR = $$PYLON_ROOT/Development/lib/x64
} else {
    PYLON_INC    = $$PYLON_ROOT/include
    PYLON_LIBDIR = $$PYLON_ROOT/lib/x64
}

INCLUDEPATH += "$$PYLON_INC"
LIBS        += -L"$$PYLON_LIBDIR"

# 库名带版本号,换 pylon 版本时按 lib/x64 目录里的实际文件名改
LIBS += -lPylonBase_v6_1
LIBS += -lPylonUtility_v6_1
LIBS += -lGCBase_MD_VC141_v3_1_Basler_pylon
LIBS += -lGenApi_MD_VC141_v3_1_Basler_pylon

SOURCES += \
    main.cpp \
    GrabWorker.cpp \
    MainWindow.cpp

HEADERS += \
    GrabWorker.h \
    MainWindow.h
