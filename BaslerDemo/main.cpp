#include <QApplication>
#include <pylon/PylonIncludes.h>

#include "MainWindow.h"

int main(int argc, char* argv[])
{
    // pylon 运行时初始化(任何 pylon 对象创建之前调用)
    Pylon::PylonInitialize();  //调用加载底层驱动和资源

    int ret = 0;
    {
        QApplication app(argc, argv);
        MainWindow w;
        w.show();
        ret = app.exec();
    } // 窗口与相机对象在此全部析构完毕,再释放 pylon

    Pylon::PylonTerminate();
    return ret;
}
