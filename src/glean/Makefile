# Makefile for glean

include $(GLEAN_ROOT)/make/common.mak

TARGET=glean

ifeq ($(PLATFORM), Unix)
	LIB=-ldsurf -llex -limage -lstats -ltimer -ltiff -lGLU -lGL -lXmu -lXext -lX11 $(EXTRALIBS)
endif # Unix
ifeq ($(PLATFORM), BeOS)
	LIB=-ldsurf -llex -limage -lstats -ltimer -ltiff -lGL -lbe $(EXTRALIBS)
endif # BeOS
ifeq ($(PLATFORM), MacOSX)
	LIB=-framework GLUT -framework OpenGL -framework AGL -framework Carbon -ldsurf -llex -limage -lstats -ltimer -ltiff
endif # MacOSX

include $(GLEAN_ROOT)/make/app.mak
