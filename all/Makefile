CC = arm-linux-gcc
EXTRA_LIBS += -lpthread
objects = main.o tty.o camera.o drv-v4l.o grab-ng.o struct-dump.o struct-v4l.o

result: $(objects)
	$(CC) -o result $(objects) $(EXTRA_LIBS)

main.o: card_ctrl.h tty.h camera.h
tty.o: tty.h
camera.o: fb.h grab-ng.h
drv-v4l.o: grab-ng.h struct-dump.h struct-v4l.h
grab-ng.o: grab-ng.h
struct-dump.o: struct-dump.h
struct-v4l.o: struct-dump.h struct-v4l.h

.PHONY : clean
clean:
	rm -f *.o result