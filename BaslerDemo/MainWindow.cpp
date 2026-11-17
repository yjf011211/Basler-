#include "MainWindow.h"
#include "GrabWorker.h"

#include <QThread>
#include <QCloseEvent>
#include <QResizeEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QRadioButton>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QStatusBar>
#include <QSignalBlocker>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    buildUi();

    // ---- 创建工作线程并把 GrabWorker 搬进去 ----
    // GrabWorker 不能带父对象才能 moveToThread;
    // 它的子对象(定时器)会跟着一起迁移。
    m_thread = new QThread(this);
    m_worker = new GrabWorker();
    m_worker->moveToThread(m_thread);
    m_thread->start();

    connectSignals();
    updateButtonStates();
    statusBar()->showMessage(QStringLiteral("就绪 — 请先点击“打开相机”"));
}

MainWindow::~MainWindow()
{
    shutdownWorker();
}

void MainWindow::connectSignals()
{
    // ====== UI → 工作线程(自动为 QueuedConnection)======
    connect(m_btnOpen,    &QPushButton::clicked,  m_worker, &GrabWorker::openCamera);
    connect(m_btnClose,   &QPushButton::clicked,  m_worker, &GrabWorker::closeCamera);
    connect(m_btnStart,   &QPushButton::clicked,  m_worker, &GrabWorker::startGrabbing);
    connect(m_btnStop,    &QPushButton::clicked,  m_worker, &GrabWorker::stopGrabbing);
    connect(m_btnTrigger, &QPushButton::clicked,  m_worker, &GrabWorker::executeSoftwareTrigger);
    connect(m_radioSoft,  &QRadioButton::toggled, m_worker, &GrabWorker::setTriggerMode);
    // 键盘输入结束/点箭头才发值,避免打字过程中刷参数
    connect(m_spinExposure, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            m_worker, &GrabWorker::setExposureUs);
    connect(m_spinGain, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            m_worker, &GrabWorker::setGainDb);

    // 触发模式单选 → 更新界面状态
    connect(m_radioSoft, &QRadioButton::toggled, this, [this](bool soft) {
        m_isSoftTrigger = soft;
        updateButtonStates();
    });

    // ====== 工作线程 → UI ======
    connect(m_worker, &GrabWorker::frameReady,        this, &MainWindow::onFrame);
    connect(m_worker, &GrabWorker::opened,            this, &MainWindow::onOpened);
    connect(m_worker, &GrabWorker::closed,            this, &MainWindow::onClosed);
    connect(m_worker, &GrabWorker::grabbingChanged,   this, &MainWindow::onGrabbingChanged);
    connect(m_worker, &GrabWorker::fpsChanged,        this, &MainWindow::onFps);
    connect(m_worker, &GrabWorker::errorOccurred,     this, &MainWindow::onError);
    connect(m_worker, &GrabWorker::exposureApplied,   this, &MainWindow::onExposureApplied);
    connect(m_worker, &GrabWorker::gainApplied,       this, &MainWindow::onGainApplied);
}

void MainWindow::buildUi()
{
    setWindowTitle(QStringLiteral("Basler Pylon 二次开发示例 — Qt 取图与显示"));
    resize(1100, 700);

    auto* central = new QWidget(this);
    auto* root = new QHBoxLayout(central);

    // ---------- 左侧控制面板 ----------
    auto* panel = new QGroupBox(QStringLiteral("控制面板"));
    panel->setFixedWidth(280);
    auto* lay = new QVBoxLayout(panel);

    auto* row1 = new QHBoxLayout;
    m_btnOpen  = new QPushButton(QStringLiteral("打开相机"));
    m_btnClose = new QPushButton(QStringLiteral("关闭相机"));
    row1->addWidget(m_btnOpen);
    row1->addWidget(m_btnClose);
    lay->addLayout(row1);

    auto* row2 = new QHBoxLayout;
    m_btnStart = new QPushButton(QStringLiteral("开始采集"));
    m_btnStop  = new QPushButton(QStringLiteral("停止采集"));
    row2->addWidget(m_btnStart);
    row2->addWidget(m_btnStop);
    lay->addLayout(row2);

    auto* grpTrig = new QGroupBox(QStringLiteral("触发模式"));
    auto* trigLay = new QVBoxLayout(grpTrig);
    m_radioContinuous = new QRadioButton(QStringLiteral("连续采集"));
    m_radioSoft       = new QRadioButton(QStringLiteral("软触发(单帧)"));
    m_radioContinuous->setChecked(true);
    trigLay->addWidget(m_radioContinuous);
    trigLay->addWidget(m_radioSoft);
    m_btnTrigger = new QPushButton(QStringLiteral("单帧触发"));
    trigLay->addWidget(m_btnTrigger);
    lay->addWidget(grpTrig);

    lay->addWidget(new QLabel(QStringLiteral("曝光时间 (μs):")));
    m_spinExposure = new QDoubleSpinBox;
    m_spinExposure->setRange(1.0, 1000000.0);
    m_spinExposure->setDecimals(0);
    m_spinExposure->setSingleStep(100.0);
    m_spinExposure->setValue(10000.0);
    m_spinExposure->setKeyboardTracking(false); // 输入结束才生效
    lay->addWidget(m_spinExposure);

    lay->addWidget(new QLabel(QStringLiteral("增益 (dB):")));
    m_spinGain = new QDoubleSpinBox;
    m_spinGain->setRange(0.0, 36.0);
    m_spinGain->setDecimals(1);
    m_spinGain->setSingleStep(0.5);
    m_spinGain->setKeyboardTracking(false);
    lay->addWidget(m_spinGain);

    m_lblFps = new QLabel(QStringLiteral("FPS: —"));
    lay->addWidget(m_lblFps);
    lay->addStretch();

    // ---------- 右侧图像显示 ----------
    m_scene = new QGraphicsScene(this);
    m_view  = new QGraphicsView(m_scene);
    m_view->setDragMode(QGraphicsView::ScrollHandDrag);
    m_view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    root->addWidget(panel);
    root->addWidget(m_view, 1);
    setCentralWidget(central);
}

void MainWindow::updateButtonStates()
{
    m_btnOpen->setEnabled(!m_isOpen);
    m_btnClose->setEnabled(m_isOpen);
    m_btnStart->setEnabled(m_isOpen && !m_isGrabbing);
    m_btnStop->setEnabled(m_isOpen && m_isGrabbing);
    m_btnTrigger->setEnabled(m_isOpen && m_isGrabbing && m_isSoftTrigger);
    m_spinExposure->setEnabled(m_isOpen);
    m_spinGain->setEnabled(m_isOpen);
}

// ============ 工作线程 → UI 的槽 ============

void MainWindow::onFrame(const QImage& frame)
{  //第一次创建，后续直接替换像素
    if (!m_item) {
        m_item = m_scene->addPixmap(QPixmap::fromImage(frame));
        m_scene->setSceneRect(m_item->boundingRect());
        fitView();
    } else {
        m_item->setPixmap(QPixmap::fromImage(frame));
        m_scene->setSceneRect(m_item->boundingRect());
    }
}

void MainWindow::onOpened(const QString& info)
{
    m_isOpen = true;
    updateButtonStates();
    statusBar()->showMessage(QStringLiteral("已打开:%1").arg(info));
}

void MainWindow::onClosed()
{
    m_isOpen = false;
    m_isGrabbing = false;
    updateButtonStates();
    statusBar()->showMessage(QStringLiteral("相机已关闭"));
}

void MainWindow::onGrabbingChanged(bool grabbing)
{
    m_isGrabbing = grabbing;
    updateButtonStates();
    statusBar()->showMessage(grabbing ? QStringLiteral("采集中…")
                                      : QStringLiteral("已停止采集"));
}

void MainWindow::onFps(double fps)
{
    m_lblFps->setText(QStringLiteral("FPS: %1").arg(fps, 0, 'f', 1));
}

void MainWindow::onError(const QString& msg)
{
    statusBar()->showMessage(QStringLiteral("错误:%1").arg(msg), 8000);
}

void MainWindow::onExposureApplied(double us)
{
    // 相机回读的值可能与设定值不同(取整/范围截断),同步回界面
    const QSignalBlocker blocker(m_spinExposure);
    m_spinExposure->setValue(us);
}

void MainWindow::onGainApplied(double db)
{
    const QSignalBlocker blocker(m_spinGain);
    m_spinGain->setValue(db);
}

// ============ 生命周期 ============

void MainWindow::fitView()
{
    if (m_item && !m_item->pixmap().isNull())
        m_view->fitInView(m_item, Qt::KeepAspectRatio);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    fitView();
}

void MainWindow::shutdownWorker()
{
    if (!m_worker) return;

    if (m_thread->isRunning()) {
        // 同步等这两个操作做完,保证相机被释放
        QMetaObject::invokeMethod(m_worker, "stopGrabbing", Qt::BlockingQueuedConnection);
        QMetaObject::invokeMethod(m_worker, "closeCamera", Qt::BlockingQueuedConnection);
        m_thread->quit();
        m_thread->wait(3000);
    }
    delete m_worker;
    m_worker = nullptr;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    shutdownWorker();
    QMainWindow::closeEvent(event);
}
