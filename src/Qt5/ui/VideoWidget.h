#pragma once

#include <QWidget>
#include <QImage>

namespace vsapp {

class VideoWidget : public QWidget {
    Q_OBJECT
public:
    explicit VideoWidget(QWidget *parent = nullptr);

public slots:
    void displayFrame(const QImage &frame);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QImage currentFrame_;
};

} // vsapps