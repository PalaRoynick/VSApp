#include "VideoWidget.h"
#include <QPainter>

namespace vsapp {

VideoWidget::VideoWidget(QWidget *parent) : QWidget(parent) {
    setAutoFillBackground(true);
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, Qt::black);
    setPalette(palette);

    setFocusPolicy(Qt::NoFocus);

    setCursor(Qt::PointingHandCursor);
}

void VideoWidget::displayFrame(const QImage &frame) {
    currentFrame_ = frame;
    update(); 
}

void VideoWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    painter.fillRect(rect(), Qt::black);

    if (!currentFrame_.isNull()) {
        QImage scaledFrame = currentFrame_.scaled(
            size(), 
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
        );

        int x = (width() - scaledFrame.width()) / 2;
        int y = (height() - scaledFrame.height()) / 2;

        painter.drawImage(x, y, scaledFrame);
    }
}

void VideoWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }

    QWidget::mousePressEvent(event);
}

} // vsapp