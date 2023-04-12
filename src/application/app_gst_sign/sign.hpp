#ifndef __SIGN_HPP__
#define __SIGN_HPP__
#include <memory>
#include <string>
#include <vector>
namespace sign {
struct Box {
    float left, top, right, bottom, confidence;
    int class_label;
    int id;  // track id

    Box() = default;
    Box(float left, float top, float right, float bottom, float confidence, int class_label, int id)
        : left(left),
          top(top),
          right(right),
          bottom(bottom),
          confidence(confidence),
          class_label(class_label),
          id(id) {}
};
typedef std::vector<Box> BoxArray;

class Solution {
	public:
    virtual BoxArray commit(const void* rgbptr, int width, int heigh, bool use_device) = 0;
};
std::shared_ptr<Solution> create_solution(const std::string& det_name);

}  // namespace sign
#endif
