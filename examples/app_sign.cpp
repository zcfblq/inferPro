#include <opencv2/opencv.hpp>

#include "app_gst_sign/sign.hpp"

using namespace std;
void app_sign() {
    auto solution = sign::create_solution("yolov5n_sign_640x640_huang_20221124_epoch40.FP16.trtmodel");
    cv::Mat image = cv::imread("inference/car.jpg");
    printf("image.cols%d", image.cols);
    auto result = solution->commit(image.data, image.cols, image.rows, false);
    printf("infer: %ld\n", result.size());
}
int main() {
    app_sign();
    return 0;
}
