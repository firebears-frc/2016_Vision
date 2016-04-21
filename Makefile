LINKER_LIBS = \
       `pkg-config --libs opencv --cflags`\
       -lntcore
#      -lopencv_imgcodecs -lopencv_imgproc -lopencv_core -lopencv_videoio\
#      -lcv -lhighgui -lcvaux -lopencv_imgproc -lopencv_core -lopencv_highgui\
#      -lopencv_video -lopencv_video_proc -lopencv_calib3d -lopencv_imgcodecs\

ifneq ($(shell echo $(JLL_HOME)), "")
 include $(shell echo $(JLL_HOME))/compile-scripts/ProjectMakefile.mk
else
 $(error "You need to set enviroment variable JLL_HOME to the jl_lib directory")
endif
