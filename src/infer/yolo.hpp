#ifndef __YOLO_HPP__
#define __YOLO_HPP__

#include <future>
#include <memory>
#include <string>
#include <vector>

namespace yolo {

enum class Type : int { V5 = 0, X = 1, V3 = 2, V7 = 3, V8 = 5 };

struct Box {
    float left, top, right, bottom, confidence;
    int class_label;

    Box() = default;
    Box(float left, float top, float right, float bottom, float confidence, int class_label)
        : left(left), top(top), right(right), bottom(bottom), confidence(confidence), class_label(class_label) {}
};

struct Image {
    const void* bgrptr = nullptr;
    int width = 0, height = 0;
    bool use_device = false;

    Image() = default;
    Image(const void* bgrptr, int width, int height, bool use_device = false)
        : bgrptr(bgrptr), width(width), height(height), use_device(use_device) {}
};

typedef std::vector<Box> BoxArray;

class Infer {
public:
    virtual BoxArray forward(const Image& image, void* stream = nullptr)                             = 0;
    virtual std::vector<BoxArray> forwards(const std::vector<Image>& images, void* stream = nullptr) = 0;
};

const char* type_name(Type type);
Infer* loadraw(const std::string& engine_file, Type type, float confidence_threshold = 0.25f,
               float nms_threshold = 0.5f);
std::shared_ptr<Infer> load(const std::string& engine_file, Type type, float confidence_threshold = 0.25f,
                            float nms_threshold = 0.5f);

std::tuple<uint8_t, uint8_t, uint8_t> hsv2bgr(float h, float s, float v);
std::tuple<uint8_t, uint8_t, uint8_t> random_color(int id);

};  // namespace yolo

#endif  // __YOLO_HPP__