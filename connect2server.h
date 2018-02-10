#ifndef CONNECT2SERVER_H
#define CONNECT2SERVER_H
#include "easywsclient.hpp"
#include <iostream>
#include <stdio.h>
#include <string>
#include "base64.h"
#include <opencv2/opencv.hpp>
#include <jsoncpp/json/json.h>

#include <sys/types.h>
#include <sys/sysinfo.h>

using namespace cv;
using namespace std;

using easywsclient::WebSocket;

class Connect2Server
{
public:
    Connect2Server();
    void SetUrl(string urlIn);
    void Connect();
    bool Mat2Base64(cv::Mat source);
    std::string WrapData();

    cv::Mat testMat;
private:
    string url;
    static WebSocket::pointer ws;
    std::string base64Data;
    std::string faceId;
    std::string camId;
    std::string time;
    std::string cpuUsage;
    std::string ramUsage;
    std::string diskUsage;

};

#endif // CONNECT2SERVER_H
