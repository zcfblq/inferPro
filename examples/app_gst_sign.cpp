#include "app_gst_sign/sign.hpp"
#include <chrono>
#include "videoSource.h"


#include "videoOptions.h"

int main() {
	using namespace std;
    auto solution = sign::create_solution("yolov5n_sign_640x640_huang_20221124_epoch40.trtmodel");
    videoOptions option;
    option.zeroCopy = true;
    option.height   = 1080;
    option.width    = 1920;
    //option.codec    = videoOptions::Codec::CODEC_H264;
    //option.resource = "rtsp://admin:xmrbi123@192.168.175.232:554/Streaming/Channels/101";
    option.resource ="/dev/video0";
    // option.resource = "rtsp://admin:dahua123456@39.184.150.8:10554/cam/realmonitor?channel=2&subtype=0";
    videoSource* input_stream = videoSource::Create(option);
    if (!input_stream) {
        return 1;
    }
    unsigned char* p = new unsigned char[3 * 1920 * 1080];
    for (int i = 0; i < 20; ++i) {
        auto t20 = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count() /
                   1000.0;
        auto result = solution->commit(p, 1920,1080, false);
        auto t21 = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count() /
                   1000.0;
        printf("cpu infer: %.2f\n", float(t21 - t20));
    }
    delete[] p;
    uchar3* image = nullptr;
    while (true) {
        auto t1 = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count() /
                  1000.0;
        if (!input_stream->Capture(&image, 1000)) {
            // check for EOS
            if (!input_stream->IsStreaming())
                break;
            continue;
        }
        auto t2 = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count() /
                  1000.0;
        int width   = input_stream->GetWidth();
        int height  = input_stream->GetHeight();
        auto result = solution->commit(image, width, height, false);
	auto t3 = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count() /
                  1000.0;
        printf("capture:%.2f; infer: %.2f\n", float(t2 - t1), float(t3 - t2));
    }
}

