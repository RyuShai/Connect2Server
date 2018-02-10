#include "connect2server.h"
#include "easywsclient.hpp"
easywsclient::WebSocket::pointer Connect2Server::ws = NULL;
Connect2Server::Connect2Server()
{
   faceId="faceID";
   camId="camID";
   time="7/2/2018-13:14:22";
   base64Data = "base64";
   cpuUsage = "cpuUsage";
   ramUsage = "ramUsage";
   diskUsage = "diskUsage";
}

void Connect2Server::SetUrl(string urlIn)
{
    url = urlIn;
}

void Connect2Server::Connect()
{
    ws = WebSocket::from_url(url);
    std::string data = WrapData();
    if(!base64Data.empty())
    {
        ws->send(data);
        cout<<"ready state: "<<ws->getReadyState()<<endl;
        while(ws->getReadyState() != WebSocket::CLOSED)
        {
                    //wait until connected
                    ws->poll(-1);
                    ws->dispatch([](const std::string& message){
                        cout<<"from client: "<<message<<endl;
                    });

        //            ws->close();

        }
    }
}

bool Connect2Server::Mat2Base64(cv::Mat source)
{
    std::vector<uchar> buffer;
       base64Data = "";
       cv::imencode(".png",source,buffer);
       uchar *msgBase64 = new uchar[buffer.size()];
       for(int i=0; i<buffer.size();i++)
       {
           msgBase64[i] = buffer[i];
       }
       base64Data = base64_encode(msgBase64, buffer.size());
       buffer.clear();
       delete msgBase64 ;
       return base64Data == "" ? false : true;
}

std::string Connect2Server::WrapData()
{
    Json::Value jdata;
    jdata["FaceId"] = faceId;
    jdata["CamId"] = camId;
    jdata["Time"] = time;
    jdata["Img"] = base64Data;
    jdata["Cpu"] = cpuUsage;
    jdata["Ram"] = ramUsage;
    jdata["Disk"] = diskUsage;
    // Output to see the result
    // cout<<"creating nested Json::Value Example pretty print: "
    //     <<endl<<jdata.toStyledString()
    //     <<endl;
    return jdata.toStyledString();
}
