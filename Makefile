cc        := g++
nvcc      := /usr/local/cuda/bin/nvcc

cpp_srcs := $(shell find src -name "*.cpp")
cpp_objs := $(cpp_srcs:.cpp=.o)
cpp_objs := $(cpp_objs:src/%=objs/%)
cpp_mk   := $(cpp_objs:.o=.mk)

cu_srcs := $(shell find src -name "*.cu")
cu_objs := $(cu_srcs:.cu=.cuo)
cu_objs := $(cu_objs:src/%=objs/%)
cu_mk   := $(cu_objs:.cuo=.cumk)

include_paths := src /usr/include/opencv4 \
	src/infer \
                                 third_party \
                                 src/application/app_gst_sign

library_paths := /usr/lib/aarch64-linux-gnu \
                /usr/local/cuda/lib64

link_librarys := nvinfer nvinfer_plugin \
                        cuda cublas cudart cudnn \
                        stdc++ dl
empty         :=
export_path   := $(subst $(empty) $(empty),:,$(library_paths))

paths     := $(foreach item,$(library_paths),-Wl,-rpath=$(item))
include_paths := $(foreach item,$(include_paths),-I$(item))
library_paths := $(foreach item,$(library_paths),-L$(item))
link_librarys := $(foreach item,$(link_librarys),-l$(item))

cpp_compile_flags := -std=c++11 -fPIC -g -fopenmp -w -O3
cu_compile_flags  := -std=c++11 -Xcompiler -fPIC -g -w -O3 -gencode=arch=compute_72,code=sm_72
link_flags        := -pthread -fopenmp -Wl,-rpath='$$ORIGIN'

# 如果是其他显卡，请修改-gencode=arch=compute_75,code=sm_75为对应显卡的能力
# 显卡对应的号码参考这里：https://developer.nvidia.com/zh-cn/cuda-gpus#compute
cpp_compile_flags += $(include_paths)
cu_compile_flags  += $(include_paths)
link_flags 		  += $(library_paths) $(link_librarys) $(paths)

ifneq ($(MAKECMDGOALS), clean)
-include $(cpp_mk) $(cu_mk)
endif

pro    : workspace/libpro.so
workspace/libpro.so : $(cpp_objs) $(cu_objs)
	@echo Link $@
	@mkdir -p $(dir $@)
	@g++ $^ -shared -o $@ $(link_flags)

objs/%.o : src/%.cpp
	@echo Compile CXX $<
	@mkdir -p $(dir $@)
	@g++ -c $< -o $@ $(cpp_compile_flags)

objs/%.cuo : src/%.cu
	@echo Compile CUDA $<
	@mkdir -p $(dir $@)
	@${nvcc} -c $< -o $@ $(cu_compile_flags)

objs/%.mk : src/%.cpp
	@echo Compile depends CXX $<
	@mkdir -p $(dir $@)
	@g++ -M $< -MF $@ -MT $(@:.mk=.o) $(cpp_compile_flags)
	
objs/%.cumk : src/%.cu
	@echo Compile depends CUDA $<
	@mkdir -p $(dir $@)
	@${nvcc} -M $< -MF $@ -MT $(@:.cumk=.cuo) $(cu_compile_flags)

run : workspace/pro
	@cd workspace && ./pro

clean :
	@rm -rf objs workspace/pro

.PHONY : clean yolo run

# 导出符号，使得运行时能够链接上
export LD_LIBRARY_PATH:=$(export_path):$(LD_LIBRARY_PATH)
