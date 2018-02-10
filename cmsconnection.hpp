#ifndef CMS_CONNECTION_H
#define CMS_CONNECTION_H

#include <stdio.h>
#include <string>
#include <vector>

#include <curl/curl.h>
#include <jsoncpp/json/json.h>
#include <vector>
#define CHECK_JSON_FORMAT

using namespace std;

//#define URL_DEFAULT     "http://stmc.vp9.vn:8000/getFaceInfo"
//#define URL_DEFAULT     "http://10.12.11.143:8000/getFaceInfo"
//#define URL_DEFAULT     "http://10.13.10.239:8000/plateInfo"
//#define URL_DEFAULT     "http://stmc.vp9.vn:8000/faceInfo"
//#define URL_DEFAULT     "http://stmc.vp9.vn:8000/frame"
//#define URL_DEFAULT     "http://10.12.11.77:8000/frame"

// #define URL_DEFAULT     "http://localhost:8000/engineInfo"
// #define URL_DEFAULT     "http://10.13.10.239:8000/engineInfo"
#define URL_DEFAULT     "http://10.13.11.63:8000/engineInfo"

namespace tq {
    struct Rect {
        Rect():x(0),y(0),width(0),height(0){}
        Rect(int x, int y, int w, int h) :
        x(x), y(y), width(w), height(h) {}
        int x;
        int y;
        int width;
        int height;
    };
}
struct PushInfo {
    std::string  id; // engine id
    std::string  key;
    tq::Rect     rect;
    long         timestamp;
};

struct PushInfo2 {
    std::string  id; // engine id
    std::vector<std::string>  keys;
    std::vector<tq::Rect> rects;
    long         timestamp;
};

#define CMSInstance CMSConnection::getInstance()
class CMSConnection
{
public:
    CMSConnection();
    static CMSConnection& getInstance(void);
    PushInfo get(std::string id);
    void push(PushInfo info);
    void push(PushInfo2 info);
    void init(std::string url);

    string makeData2(std::string faceId,std::string camId,std::string logId,std::string img )
    {
        Json::Value jdata;
        jdata["faceId"] = faceId;
        jdata["timestamp"] = to_string(info2.timestamp);
        jdata["length"] = (int)info2.rects.size();
        return jdata.toStyledString();
    };
    /***************************************
        #include <cmsconnection.hpp>

        // Call: CMSInstance.push(x,y,w,h,ts                                                                                        );
        // Ex: 
        CMSInstance.push(0,119,400,250,1516037394458);
    ****************************************/
    void push(int x,int y,int w,int h,long ts,std::string id = "123456")
    {
        PushInfo info;
        info.id = id;
        info.rect = tq::Rect(x,y,w,h);
        info.timestamp = ts;
        push(info);
    }
    void push(std::vector<tq::Rect> v, long ts, std::string id = "123456") 
    {
        PushInfo2 info;
        info.id = id;
        info.rects = v;
        info.timestamp = ts;
        push(info);
    }
    template<class r>
    void push(std::vector<r> v, long ts, std::string id = "123456") 
    {
        std::vector<tq::Rect> rects;
        for (auto &t : v) {
            tq::Rect rect(t.x, t.y, t.width, t.height);
            rects.push_back(rect);
        }
        PushInfo2 info;
        info.id = id;
        info.rects = rects;
        info.timestamp = ts;
        push(info);
    }

private:
    std::string m_url;
    std::string makeData(PushInfo);
    std::string makeData(PushInfo2);
    std::string makeData(
        std::string engine_id,
        std::string key,
        tq::Rect rect,
        long timestamp
    );
    void postToServer(std::string);
};


CMSConnection::CMSConnection()
{
    m_url = URL_DEFAULT;
}

void CMSConnection::init(std::string url)
{
    m_url = url;
}

CMSConnection& CMSConnection::getInstance(void)
{
    static CMSConnection instance;
    return instance;
}

void CMSConnection::push(PushInfo info)
{
    std::string jsonstr = makeData(
        info.id,
        info.key,
        info.rect,
        info.timestamp
    );
    postToServer(jsonstr);
}

std::string CMSConnection::makeData(PushInfo2 info2)
{
    Json::Value jdata;
    Json::Value jrect;
    jdata["engine_id"] = info2.id;
    jdata["timestamp"] = to_string(info2.timestamp);
    jdata["length"] = (int)info2.rects.size();
    for (int i = 0; i < (int)info2.rects.size(); i++) {
        // jrect["key"] = info2.key[i];
        jrect["x"] = info2.rects[i].x;
        jrect["y"] = info2.rects[i].y;
        jrect["width"] = info2.rects[i].width;
        jrect["height"] = info2.rects[i].height;
        jdata["rects"][i] = jrect;
    }
    // Output to see the result
    // cout<<"creating nested Json::Value Example pretty print: "
    //     <<endl<<jdata.toStyledString()
    //     <<endl;
    return jdata.toStyledString();
}

void CMSConnection::push(PushInfo2 info2)
{
    std::string jsonstr = makeData(info2);
    postToServer(jsonstr);
}
// static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
// {
//     // cout << (std::string*)userp << endl;
//     printf("%s\n",userp);
//     return 0;
//     ((std::string*)userp)->append((char*)contents, size * nmemb);
//     return size * nmemb;
// }
size_t disable_log(char *ptr, size_t size, size_t nmemb, void *userdata)
{
   return size * nmemb;
}
void CMSConnection::postToServer(string jsonstr)
{
    // CURLcode ret;
    CURL *hnd;
    struct curl_slist *slist1;
    slist1 = NULL;
    slist1 = curl_slist_append(slist1, "Content-Type: application/json");
    hnd = curl_easy_init();
    // get plate
    curl_easy_setopt(hnd, CURLOPT_URL, m_url.c_str());
    curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, jsonstr.c_str());
    curl_easy_setopt(hnd, CURLOPT_USERAGENT, "VP9-AHFR");
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
    curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(hnd, CURLOPT_TIMEOUT, 2L); /* 2 seconds timeout */
    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, &disable_log);
    curl_easy_perform(hnd);
    curl_easy_cleanup(hnd);
    hnd = NULL;
    curl_slist_free_all(slist1);
    slist1 = NULL;
    // printf("Post(%s)\n", m_url.c_str());
}

std::string CMSConnection::makeData(
    std::string engine_id,
    std::string key,
    tq::Rect rect,
    long timestamp
)
{
    string x = to_string(rect.x);
    string y = to_string(rect.y);
    string w = to_string(rect.width);
    string h = to_string(rect.height);
    string ts = to_string(timestamp);

    #ifndef CHECK_JSON_FORMAT
    #endif
        std::string jsonstr = " {\"engine_id\":\"";
        jsonstr.append(engine_id);
        jsonstr.append("\",\"key\":\"");
        jsonstr.append(key);
        jsonstr.append("\",\"x\":\"");
        jsonstr.append(x);
        jsonstr.append("\",\"y\":\"");
        jsonstr.append(y);
        jsonstr.append("\",\"width\":\"");
        jsonstr.append(w);
        jsonstr.append("\",\"height\":\"");
        jsonstr.append(h);
        jsonstr.append("\",\"timestamp\":\"");
        jsonstr.append(ts);
        jsonstr.append(" \" }");
    #ifdef CHECK_JSON_FORMAT
        printf("%s\n",jsonstr.c_str());
    #endif
    return jsonstr;
}
#endif // CMS_CONNECTION_H
