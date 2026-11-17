#include "GrabWorker.h"

#include <QTimer>
#include <QVector>
#include <pylon/PylonIncludes.h>
#include <pylon/ParameterIncludes.h>   // CFloatParameter/CEnumParameter 等参数类(pylon 7 里改叫 CEnumerationParameter)

using namespace Pylon;

namespace {

// 把 pylon 抓到的一帧转成 QImage。
// 注意:必须 .copy() 深拷贝!pylon 的取图缓冲区在 grabResult 释放后
// 会被回收复用,不拷贝的话界面线程会读到失效内存(花屏/崩溃)。
//
// 兼容说明:本机 Qt 5.9 没有 Format_Grayscale8(5.13+)和 Format_BGR888(5.14+),
// 所以按 Qt 版本做了分支;更老版本下 BGR8 走格式转换器统一转 RGB8。
QImage toQImage(CGrabResultPtr& res)
{
    const int w = static_cast<int>(res->GetWidth());
    const int h = static_cast<int>(res->GetHeight());
    const uchar* buf = static_cast<const uchar*>(res->GetBuffer());

    switch (res->GetPixelType()) {
    case PixelType_Mono8:
#if QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
        return QImage(buf, w, h, w, QImage::Format_Grayscale8).copy();
#else
    {
        // Qt 5.13 之前用 Indexed8 + 256 级灰度调色板
        QImage img(buf, w, h, w, QImage::Format_Indexed8);
        QVector<QRgb> gray(256);
        for (int i = 0; i < 256; ++i) gray[i] = qRgb(i, i, i);
        img.setColorTable(gray);
        return img.copy();
    }
#endif
    case PixelType_RGB8packed:
        return QImage(buf, w, h, 3 * w, QImage::Format_RGB888).copy();
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    case PixelType_BGR8packed:
        return QImage(buf, w, h, 3 * w, QImage::Format_BGR888).copy();
#endif
    default:
        break;
    }

    // 其它像素格式(以及老 Qt 下的 BGR8)统一转成 RGB8
    CImageFormatConverter converter;
    converter.OutputPixelFormat = PixelType_RGB8packed;
    CPylonImage converted;
    converter.Convert(converted,
                      res->GetBuffer(),
                      res->GetImageSize(),
                      res->GetPixelType(),
                      res->GetWidth(),
                      res->GetHeight(),
                      0,                       // 行内填充字节
                      ImageOrientation_TopDown);
    return QImage(static_cast<const uchar*>(converted.GetBuffer()),
                  static_cast<int>(converted.GetWidth()),
                  static_cast<int>(converted.GetHeight()),
                  3 * static_cast<int>(converted.GetWidth()),
                  QImage::Format_RGB888).copy();
}

} // namespace

GrabWorker::GrabWorker(QObject* parent)
    : QObject(parent)
    , m_timer(new QTimer(this)) // 子对象,moveToThread 时随本对象一起迁移
{
    m_timer->setInterval(5);    // 每 5ms 泵一次;30fps 相机足以逐帧显示
    connect(m_timer, &QTimer::timeout, this, &GrabWorker::pump);
}

GrabWorker::~GrabWorker()
{
    // 兜底清理(正常流程 closeCamera 已经做过一遍)
    try {
        if (m_camera) {
            if (m_grabbing) m_camera->StopGrabbing();
            m_camera->Close();
            delete m_camera;
            m_camera = nullptr;
        }
    } catch (...) {
    }
}

void GrabWorker::openCamera()
{
    if (m_camera) closeCamera();

    try {
        DeviceInfoList_t devices;
        CTlFactory::GetInstance().EnumerateDevices(devices);
        if (devices.empty()) {
            emit errorOccurred(QStringLiteral("未发现相机:请检查电源/网线/USB,并关闭 pylon Viewer 后重试"));
            return;
        }

        m_camera = new CInstantCamera(CTlFactory::GetInstance().CreateFirstDevice());
        m_camera->Open();

        // 读回型号与序列号,用于状态栏显示
        CDeviceInfo info = m_camera->GetDeviceInfo();
        const QString camInfo = QStringLiteral("%1 | SN %2")
            .arg(QString::fromUtf8(info.GetModelName().c_str()),
                 QString::fromUtf8(info.GetSerialNumber().c_str()));
        emit opened(camInfo);

        // 把相机当前的曝光/增益读回来,让界面与相机一致
        GenApi::INodeMap& nodemap = m_camera->GetNodeMap();
        try {
            emit exposureApplied(CFloatParameter(nodemap, "ExposureTime").GetValue());
        } catch (...) {}
        try {
            emit gainApplied(CFloatParameter(nodemap, "Gain").GetValue());
        } catch (...) {}

    } catch (const GenericException& e) {
        emit errorOccurred(QString::fromUtf8(e.GetDescription()));
        delete m_camera;
        m_camera = nullptr;
    }
}

void GrabWorker::closeCamera()
{
    try {
        stopGrabbingInternal();
        if (m_camera) {
            m_camera->Close();
            delete m_camera;
            m_camera = nullptr;
        }
        emit closed();
    } catch (const GenericException& e) {
        emit errorOccurred(QString::fromUtf8(e.GetDescription()));
    }
}

void GrabWorker::startGrabbing()
{
    if (!m_camera || !m_camera->IsOpen()) {
        emit errorOccurred(QStringLiteral("请先打开相机"));
        return;
    }
    if (m_grabbing) return;

    try {
        // 触发参数在 StartGrabbing 之前设置(连续模式下 TriggerMode=Off)
        GenApi::INodeMap& nodemap = m_camera->GetNodeMap();
        CEnumParameter(nodemap, "TriggerSelector").SetValue("FrameStart");
        CEnumParameter(nodemap, "TriggerSource").SetValue("Software");
        CEnumParameter(nodemap, "TriggerMode").SetValue(m_softTrigger ? "On" : "Off");

        m_camera->StartGrabbing(GrabStrategy_LatestImageOnly);
        m_grabbing = true;
        m_frameCount = 0;
        m_fpsClock.start();
        m_timer->start();
        emit grabbingChanged(true);
    } catch (const GenericException& e) {
        emit errorOccurred(QString::fromUtf8(e.GetDescription()));
    }
}

void GrabWorker::stopGrabbingInternal()
{
    if (m_timer) m_timer->stop();
    if (m_camera && m_grabbing) {
        try {
            m_camera->StopGrabbing();
        } catch (...) {
        }
    }
    m_grabbing = false;
}

void GrabWorker::stopGrabbing()
{
    const bool wasGrabbing = m_grabbing;
    stopGrabbingInternal();
    if (wasGrabbing) emit grabbingChanged(false);
}

void GrabWorker::pump()
{
    if (!m_grabbing || !m_camera) return;

    try {
        CGrabResultPtr res;
        // 超时 1ms + TimeoutHandling_Return:没图立即返回,不阻塞事件循环
        if (m_camera->RetrieveResult(1, res, TimeoutHandling_Return)) {
            if (res->GrabSucceeded()) {
                emit frameReady(toQImage(res));

                // 约每秒报一次帧率
                ++m_frameCount;
                const qint64 ms = m_fpsClock.elapsed();
                if (ms >= 1000) {
                    emit fpsChanged(m_frameCount * 1000.0 / ms);
                    m_frameCount = 0;
                    m_fpsClock.restart();
                }
            } else {
                emit errorOccurred(QString::fromUtf8(res->GetErrorDescription()));
            }
        }
    } catch (const GenericException& e) {
        emit errorOccurred(QString::fromUtf8(e.GetDescription()));
    }
}

void GrabWorker::setTriggerMode(bool softTrigger)
{
    m_softTrigger = softTrigger;
    if (m_camera && m_camera->IsOpen()) {
        try {
            GenApi::INodeMap& nodemap = m_camera->GetNodeMap();
            CEnumParameter(nodemap, "TriggerSelector").SetValue("FrameStart");
            CEnumParameter(nodemap, "TriggerSource").SetValue("Software");
            CEnumParameter(nodemap, "TriggerMode").SetValue(m_softTrigger ? "On" : "Off");
        } catch (const GenericException& e) {
            emit errorOccurred(QString::fromUtf8(e.GetDescription()));
        }
    }
}

void GrabWorker::executeSoftwareTrigger()
{
    if (!m_grabbing || !m_camera) return;
    try {
        m_camera->ExecuteSoftwareTrigger();
    } catch (const GenericException& e) {
        emit errorOccurred(QString::fromUtf8(e.GetDescription()));
    }
}

void GrabWorker::setExposureUs(double us)
{
    if (m_camera && m_camera->IsOpen()) applyExposure(us);
}

void GrabWorker::applyExposure(double us)
{
    try {
        CFloatParameter p(m_camera->GetNodeMap(), "ExposureTime");
        p.SetValue(us);
        emit exposureApplied(p.GetValue()); // 读回实际生效值(可能被相机取整/截断)
    } catch (const GenericException& e) {
        emit errorOccurred(QString::fromUtf8(e.GetDescription()));
    }
}

void GrabWorker::setGainDb(double db)
{
    if (m_camera && m_camera->IsOpen()) applyGain(db);
}

void GrabWorker::applyGain(double db)
{
    try {
        CFloatParameter p(m_camera->GetNodeMap(), "Gain");
        p.SetValue(db);
        emit gainApplied(p.GetValue());
    } catch (const GenericException& e) {
        emit errorOccurred(QString::fromUtf8(e.GetDescription()));
    }
}
