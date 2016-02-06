ifneq ($(shell echo $(JLL_HOME)), "")
 include $(shell echo $(JLL_HOME))/ProjectMakefile.mk
else
 $(error "You need to set enviroment variable JLL_HOME to the jl_lib directory")
endif

deps-download:
	cd deps/ && \
	wget https://codeload.github.com/Itseez/opencv/zip/3.0.0 && \
	unzip 3.0.0 && \
	rm 3.0.0

deps-build:
	cd deps/opencv-3.0.0 && \
	mkdir -p build/usr_local/ && \
	cmake -D CMAKE_BUILD_TYPE=RELEASE \
		-D CMAKE_INSTALL_PREFIX=usr_local \
		-D INSTALL_C_EXAMPLES=ON \
		-D INSTALL_PYTHON_EXAMPLES=OFF \
		-D OPENCV_EXTRA_MODULES_PATH=~/opencv_contrib-3.0.0/modules \
		-D BUILD_EXAMPLES=OFF . && \
	make -j4 && \
	sudo make install && \
	sudo ldconfig
#	rm -r `find modules/ -type f -name '*_main.cpp.o'` \
#	rm -r `find modules/ -type f -name '*test_*.cpp.o'` \
#	ld -r `find modules/ -type f -name '*.cpp.o'` -o ../../cv2.o
#	ar csr ../../build/deps/cv2.o `find modules/core/ modules/calib3d/ modules/features2d modules/flann modules/hal modules/highgui modules/imgcodecs modules/imgproc modules/ml modules/objdetect modules/photo modules/shape modules/stitching modules/superres modules/videoio modules/videostab -type f -name '*.cpp.o'`

#define HAVE_OPENCV_FLANN
#define HAVE_OPENCV_HAL
#define HAVE_OPENCV_HIGHGUI
#define HAVE_OPENCV_IMGCODECS
#define HAVE_OPENCV_IMGPROC
#define HAVE_OPENCV_ML
#define HAVE_OPENCV_OBJDETECT
#define HAVE_OPENCV_PHOTO
#define HAVE_OPENCV_SHAPE
#define HAVE_OPENCV_STITCHING
#define HAVE_OPENCV_SUPERRES
#define HAVE_OPENCV_VIDEO
#define HAVE_OPENCV_VIDEOIO
#define HAVE_OPENCV_VIDEOSTAB
