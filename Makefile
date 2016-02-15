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
