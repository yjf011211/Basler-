#ifndef GRABWORKER_H
#define GRABWORKER_H

#include <QObject>
#include <QImage>
#include <QElapsedTimer>

class QTimer;

namespace Pylon { class CInstantCamera; }

// 取图工作对象。
// 通过 moveToThread() 放到独立 QThread 里运行:
// 所有相机操作(打开/关闭/取图/改参数)都发生在它所驻留的线程,
// UI 线程只通过信号槽发指令、收图像,互不阻塞。
class GrabWorker : public QObject
{
    Q_OBJECT
public:
    explicit GrabWorker(QObject* parent = nullptr);
    ~GrabWorker() override;

public slots:
    void openCamera();              // 枚举设备并打开第一台相机
    void closeCamera();             // 停止采集并关闭相机
    void startGrabbing();           // 开始采集(连续 或 软触发等待)
    void stopGrabbing();            // 停止采集
    void setExposureUs(double us);  // 设置曝光时间(微秒)
    void setGainDb(double db);      // 设置增益(dB)
    void setTriggerMode(bool softTrigger); // true=软触发, false=连续
    void executeSoftwareTrigger();  // 软触发一帧

signals:
    void opened(const QString& cameraInfo);   // 相机已打开(附型号/序列号)
    void closed();                            // 相机已关闭
    void grabbingChanged(bool grabbing);      // 采集状态变化
    void frameReady(const QImage& frame);     // 新图像(已深拷贝)
    void fpsChanged(double fps);              // 帧率统计(约 1s 一次)
    void exposureApplied(double us);          // 相机实际生效的曝光
    void gainApplied(double db);              // 相机实际生效的增益
    void errorOccurred(const QString& message);

private slots:
    void pump();                  // 定时取一帧并发给 UI

private:
    void stopGrabbingInternal();
    void applyExposure(double us);
    void applyGain(double db);

    Pylon::CInstantCamera* m_camera = nullptr;
    QTimer* m_timer = nullptr;    // 泵图定时器(worker 的子对象,随 moveToThread 迁移)
    QElapsedTimer m_fpsClock;
    int  m_frameCount = 0;
    bool m_grabbing = false;
    bool m_softTrigger = false;
};

#endif // GRABWORKER_H
