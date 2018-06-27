#ifndef __WINDOW_H_
#define __WINDOW_H_

#include <iostream>
#include <sstream>
#include <fstream>

#include <QWidget>
#include <QDesktopWidget>
#include <QPainter>
#include <QGuiApplication>
#include <QApplication>
#include <QScreen>
#include <QTimer>
#include <QImage>

#include "hack.h"

class QWindow : public QWidget
{
    Q_OBJECT
    public:
        explicit QWindow(QWidget *parent = 0);
        std::array<EntityInfo,64> entitiesToScreen;
        std::vector<float> params;
        QPixmap pixmap;
        Hack* hack;
    
    protected:
        void paintEvent(QPaintEvent *event);

    signals:

    public slots:
        void callback();
};


int execQApp(Hack *hack);

#endif