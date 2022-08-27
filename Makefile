TARGET = Factorunner
OBJS = audio.o tex.o utils.o input.o gr.o main.o

CC=cc
LD=cc

LIBS = -lm -lraylib

CFLAGSA = -ffast-math -Wall -DRAYLIB=1 $(LIBS)
ifdef DEBUG
CFLAGSA += -O0 -g
endif

CFLAGS = $(CFLAGSA) -std=c99 -I.
LDFLAGS = $(CFLAGSA)

DEPS := $(OBJS:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "[LD] $(TARGET)" && \
	$(LD) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.c
	@echo "[CC] $@" && mkdir -p $(dir $@) && \
	$(CC) $(CFLAGS) -c $< -o $@

.phony: clean

clean:
	rm -rf *.o $(TARGET) dist

-include $(DEPS)

run:
	./Factorunner

dist: .FORCE
	rm -rf dist ; \
	mkdir -p dist ; \
	cp -rv assets dist/assets ; \
	rm -rf dist/assets/*.pgf ; \
	cp *.txt $(TARGET) dist ; \
	rm -f factorunner_pc.zip ; \
	zip -r factorunner_pc.zip dist/*

# PLAYSTATION PORTABLE
psp:
	$(MAKE) -f Makefile.psp
psp_run:
	$(MAKE) -f Makefile.psp run
psp_clean:
	$(MAKE) -f Makefile.psp clean
psp_dist:
	$(MAKE) -f Makefile.psp dist

.phony: .FORCE
.FORCE:
