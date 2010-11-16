VPATH = src:obj

EE_LIB_DIR = lib/
EE_SRC_DIR = src/
EE_INC_DIR = include/
EE_DVP = dvp-as

EE_OBJS = dma.o output.o screen.o vram.o vu1.o math.o matrix.o

EE_LIB = lib/libpdk.a

VU_SOURCES=$(wildcard *.vsm)
VU_OBJECTS=$(patsubst %.vsm,%.o,$(VU_SOURCES))

all: $(EE_OBJS_DIR) $(VU_OBJECTS) $(EE_LIB_DIR) $(EE_LIB)

%.o: %.vsm
	$(EE_DVP) $< -o $@

$(EE_LIB_DIR):
	mkdir $(EE_LIB_DIR)

clean:
	rm -f -r *.o $(EE_LIB_DIR)


include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal

