# g++ examples/app_gst_sign.cpp -o workspace/example -I src/application/ -I third_party/jetson-utils-aarch64/include/jetson-utils/ -I /usr/local/cuda-10.2/targets/aarch64-linux/include -L workspace/ -ljetson-utils -lpro
# cd workspace && ./example


g++ examples/app_sign.cpp -o workspace/example -I src/application/ -I /usr/include/opencv4 -L workspace -L /usr/lib/aarch64-linux-gnu \
    -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lpro
cd workspace && ./example