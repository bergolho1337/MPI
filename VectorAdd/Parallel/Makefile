MPICXX = mpicxx
MPICXXFLAGS = 
#CFLAGS = -DDEBUG
PNAME = vectoradd

all: $(PNAME)

$(PNAME): $(PNAME).c
	$(MPICXX) $(MPICXXFLAGS) -o $@ $^ -lm 

clean:
	rm -f $(PNAME)

print-%  : ; @echo $* = $($*)
