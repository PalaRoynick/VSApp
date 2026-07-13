#pragma once

#include <QWidget>
#include <QImage>
#include <QMouseEvent>

namespace vsapp {

class VideoWidget : public QWidget {
    Q_OBJECT
public:
    explicit VideoWidget(QWidget *parent = nullptr);

public slots:
    void displayFrame(const QImage &frame);

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QImage currentFrame_;
};

} // vsapps