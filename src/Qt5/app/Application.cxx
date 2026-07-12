#include "Application.h"
#include "../ui/MainWindow.h"

#include <QDateTime>
#include <QDebug>
#include <QSettings>

namespace vsapp {

Application::Application(int &argc, char **argv) 
    : QApplication(argc, argv) 
{
    setApplicationName("MinimalVideoPlayer");
    setApplicationVersion("1.0");

    setupEnvironment();
    createMainWindow();

    connect(this, &QCoreApplication::aboutToQuit, this, &Application::cleanup);
}

int Application::run() {
    mainWindow_->show();
    return exec();
}

void Application::createMainWindow() {
    mainWindow_ = new MainWindow();
}

void Application::setupEnvironment() {
    setOrganizationName("VS_App");
    setOrganizationDomain("vsapp.local");

    qInstallMessageHandler([](QtMsgType type, const QMessageLogContext &ctx, const QString &msg) {
        Q_UNUSED(ctx);
        QString level;
        switch (type) {
            case QtMsgType::QtDebugMsg:    level = "DEBUG"; break;
            case QtMsgType::QtWarningMsg:  level = "WARN "; break;
            case QtMsgType::QtCriticalMsg: level = "CRIT "; break;
            default: level = "INFO "; break;
        }
        fprintf(stderr, "[%s] [%s] %s\n", 
                QDateTime::currentDateTime().toString("hh:mm:ss").toLatin1().constData(),
                level.toLatin1().constData(),
                msg.toLocal8Bit().constData());
    });

    // Global dark theme (QSS)
    setStyleSheet(
        "QWidget { background-color: #058a74; color: #ffffff; }"
        "QSlider { background-color:  #7df1eb; color: #ffffff; }"
        "QPushButton { background-color: #3c3f41; border: 1px solid #555; padding: 5px; }"
        "QPushButton:hover { background-color: #4e5254; }"
    );

    QStringList args = arguments();
    if (args.size() > 1) {
        setProperty("startupFile", args.at(1));
        // any second argument means webCam usage, the first one must be correct web video source file
        if (args.size() > 2) {
            setProperty("webCam", args.at(2));
        }
    }
}

void Application::cleanup() {
    qDebug() << "Application::cleanup() triggered. Releasing resources...";

    delete mainWindow_;
    mainWindow_ = nullptr;
}

} // vsapp