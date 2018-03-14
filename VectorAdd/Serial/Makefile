CC = g++
CFLAGS = 
#CFLAGS = -DDEBUG
PNAME = vectoradd

all: $(PNAME)

$(PNAME): $(PNAME).c
	$(CC) $(CFLAGS) -o $@ $^ -lm 

clean:
	rm -f $(PNAME)

print-%  : ; @echo $* = $($*)
