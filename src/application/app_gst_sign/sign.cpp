#include "sign.hpp"

#include "infer/yolo.hpp"
#include "track/bytetrack/BYTETracker.h"

namespace sign {
using namespace std;
vector<Object> det2tracks(const yolo::BoxArray &array) {
    vector<Object> outputs;
    for (int i = 0; i < array.size(); ++i) {
        auto &abox = array[i];
        Object obox;
        obox.prob    = abox.confidence;
        obox.label   = abox.class_label;
        obox.rect[0] = abox.left;
        obox.rect[1] = abox.top;
        obox.rect[2] = abox.right - abox.left;
        obox.rect[3] = abox.bottom - abox.top;
        outputs.emplace_back(obox);
    }
    return outputs;
}
class SolutionImpl : public Solution {
public:
    virtual ~SolutionImpl() {
        yolo_.reset();
        tracker_.reset();
    }

    virtual BoxArray commit(const void *rgbptr, int width, int heigh, bool use_device) override {
        auto objs   = yolo_->forward(yolo::Image(rgbptr, width, heigh));
        auto tracks = tracker_->update(det2tracks(objs));
        BoxArray output;
        for (size_t t = 0; t < tracks.size(); t++) {
            auto &track = tracks[t];
            auto obj    = objs[track.detection_index];
            output.emplace_back(obj.left, obj.top, obj.right, obj.bottom, obj.confidence, obj.class_label,
                                track.track_id);
        }
        return output;
    }
    virtual bool startup(const string &det_name) {
        yolo_ = yolo::load(det_name, yolo::Type::V5);
        // warm up
        unsigned char *p = new unsigned char[3 * 640 * 640];
        for (int i = 0; i < 10; ++i) {
            yolo_->forward(yolo::Image(p, 640, 640));
        }
        delete[] p;
        tracker_ = make_shared<BYTETracker>();
        tracker_->config()
            .set_initiate_state({0.1, 0.1, 0.1, 0.1, 0.2, 0.2, 1, 0.2})
            .set_per_frame_motion({0.1, 0.1, 0.1, 0.1, 0.2, 0.2, 1, 0.2})
            .set_max_time_lost(150);
        if (yolo_ == nullptr || tracker_ == nullptr) {
            return false;
        }
        return true;
    }

private:
    shared_ptr<yolo::Infer> yolo_;
    shared_ptr<BYTETracker> tracker_;
};
std::shared_ptr<Solution> create_solution(const std::string &det_name) {
    shared_ptr<SolutionImpl> instance(new SolutionImpl());
    if (!instance->startup(det_name)) {
        instance.reset();
    }
    return instance;
}
}  // namespace sign
