#pragma once

#include <QApplication>

namespace vsapp {

class MainWindow;

class Application : public QApplication {
public:
    Application(int &argc, char **argv);
    ~Application() override;

    int run();

    void setupEnvironment();

private:
    MainWindow *mainWindow_ = nullptr;

    void createMainWindow();
};

} // vsapp