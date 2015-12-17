CC=g++
OPENCV_INC_DIR=/usr/local/include # or where you put OpenCV include folder
OPENCV_LIB_DIR=/usr/local/lib # or where you put OpenCV lib folder
OPENCV_LIBS=-lopencv_core -lopencv_objdetect -lopencv_imgproc -lopencv_highgui


IDIR=include
LDIR=lib
ODIR=obj
SDIR=src
INTRAFACE_LIB=$(LDIR)/libintraface.a
_DEPS=XXDescriptor.h FaceAlignment.h Marcos.h binary_model_file.h
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS)) 
_OBJ=DemoDetector.o DemoTracker.o binary_model_file.o Robot_s_head.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ)) 

CFLAGS=-I$(OPENCV_INC_DIR) -I$(IDIR)
LDFLAGS=-L$(OPENCV_LIB_DIR)

EXECUTABLE1=DemoDetector
EXECUTABLE2=DemoTracker
EXECUTABLE3=Robot_s_head
CLEAN=clean

all: $(EXECUTABLE1) $(EXECUTABLE2) $(EXECUTABLE3)
$(EXECUTABLE1): $(ODIR)/$(EXECUTABLE1).o $(ODIR)/binary_model_file.o $(INTRAFACE_LIB) 
	$(CC) -o $@ $^ $(LDFLAGS) $(OPENCV_LIBS)
$(EXECUTABLE2): $(ODIR)/$(EXECUTABLE2).o $(ODIR)/binary_model_file.o $(INTRAFACE_LIB) 
	$(CC) -o $@ $^ $(LDFLAGS) $(OPENCV_LIBS)
$(EXECUTABLE3): $(ODIR)/$(EXECUTABLE3).o $(ODIR)/binary_model_file.o $(INTRAFACE_LIB) 
	$(CC) -o $@ $^ $(LDFLAGS) $(OPENCV_LIBS)	
	
$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CC) -c -O3 -o $@ $< $(CFLAGS)
clean: 
	rm $(OBJ) $(EXECUTABLE1) $(EXECUTABLE2) $(EXECUTABLE3)
