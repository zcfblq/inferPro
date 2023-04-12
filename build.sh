g++ examples/app_gst_sign.cpp -o workspace/example -I src/application/ -I third_party/jetson-utils-aarch64/include/jetson-utils/ -I /usr/local/cuda-10.2/targets/aarch64-linux/include -L workspace/ -ljetson-utils -lpro
cd workspace && ./example
