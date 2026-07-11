#include "Application.h"
#include "../ui/MainWindow.h"
#include <QDateTime>
#include <QDebug>

extern "C" {
#include <libavformat/avformat.h>
}

namespace vsapp {

Application::Application(int &argc, char **argv) 
    : QApplication(argc, argv) 
{
    setApplicationName("MinimalVideoPlayer");
    setApplicationVersion("1.0");

    setupEnvironment();
    createMainWindow();
}

Application::~Application() {
    delete mainWindow_; 
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

    avformat_network_init();

    qInstallMessageHandler([](QtMsgType type, const QMessageLogContext &ctx, const QString &msg) {
        Q_UNUSED(ctx);
        QString level;
        switch (type) {
            case QtDebugMsg: level = "DEBUG"; break;
            case QtWarningMsg: level = "WARN "; break;
            case QtCriticalMsg: level = "CRIT "; break;
            default: level = "INFO "; break;
        }
        fprintf(stderr, "[%s] [%s] %s\n", 
                QDateTime::currentDateTime().toString("hh:mm:ss").toLatin1().constData(),
                level.toLatin1().constData(), 
                msg.toLocal8Bit().constData());
    });

    // Global dark theme (QSS)
    setStyleSheet(
        "QWidget { background-color: #2b2b2b; color: #efe0e0; }"
        "QPushButton { background-color: #3c3f41; border: 1px solid #555; padding: 5px; }"
        "QPushButton:hover { background-color: #4e5254; }"
    );

    QStringList args = arguments();
    if (args.size() > 1) {
        setProperty("startupFile", args.at(1));
    }
}

} // vsapp