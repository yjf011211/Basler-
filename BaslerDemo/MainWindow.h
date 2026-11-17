#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>

class QGraphicsView;
class QGraphicsScene;
class QGraphicsPixmapItem;
class QThread;
class QPushButton;
class QDoubleSpinBox;
class QRadioButton;
class QLabel;
class QCloseEvent;
class QResizeEvent;
class GrabWorker;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onFrame(const QImage& frame);
    void onOpened(const QString& info);
    void onClosed();
    void onGrabbingChanged(bool grabbing);
    void onFps(double fps);
    void onError(const QString& msg);
    void onExposureApplied(double us);
    void onGainApplied(double db);

private:
    void buildUi();
    void connectSignals();
    void updateButtonStates();
    void shutdownWorker();
    void fitView();

    GrabWorker* m_worker = nullptr;
    QThread*    m_thread = nullptr;

    // 显示
    QGraphicsView*       m_view = nullptr;
    QGraphicsScene*      m_scene = nullptr;
    QGraphicsPixmapItem* m_item = nullptr;

    // 控件
    QPushButton*    m_btnOpen = nullptr;
    QPushButton*    m_btnClose = nullptr;
    QPushButton*    m_btnStart = nullptr;
    QPushButton*    m_btnStop = nullptr;
    QPushButton*    m_btnTrigger = nullptr;
    QRadioButton*   m_radioContinuous = nullptr;
    QRadioButton*   m_radioSoft = nullptr;
    QDoubleSpinBox* m_spinExposure = nullptr;
    QDoubleSpinBox* m_spinGain = nullptr;
    QLabel*         m_lblFps = nullptr;

    // 状态
    bool m_isOpen = false;
    bool m_isGrabbing = false;
    bool m_isSoftTrigger = false;
};

#endif // MAINWINDOW_H
