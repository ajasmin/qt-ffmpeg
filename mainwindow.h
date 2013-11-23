#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <cstdint>
#include <ctime>
#include <vector>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavfilter/avfilter.h>
}

#include <boost/filesystem.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include <QMainWindow>
#include <QBitmap>
#include <QLabel>
#include <QLayout>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void showRandomFrame();
    
private:
    Ui::MainWindow *ui;
    boost::random::mt19937 gen;
};

#endif // MAINWINDOW_H
