#include <QCoreApplication>
#include "connect2server.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <QDateTime>
#include <QDebug>
#include <time.h>
using namespace cv;

const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    cv::Mat img;
    cout<<"ryu shai: "<<currentDateTime()<<std::endl;
    img = imread("/home/ryu/Pictures/20180119_212519.jpg",CV_LOAD_IMAGE_COLOR);

    Connect2Server con2server;
    con2server.testMat = img;
    con2server.Mat2Base64(con2server.testMat);
//    con2server.SetUrl("ws://quanly.shopcam.vp9.vn");
    con2server.SetUrl("ws://localhost:8224/sender");
    std::string json = con2server.WrapData();
//    std::cout<<json<<std::endl;
//    con2server.Connect();
    return a.exec();
}
