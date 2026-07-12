#pragma once

#include <QApplication>

namespace vsapp {

class MainWindow;

class Application : public QApplication {
    Q_OBJECT
public:
    Application(int &argc, char **argv);
    ~Application() override = default;

    int run();

private slots:
    void cleanup();

private:
    MainWindow *mainWindow_ = nullptr;

    void setupEnvironment();
    void createMainWindow();
};

} // vsapp