SRC = XMLConfDocument XMLConfParser XMLConfWriter
OBJ = $(addsuffix .o,$(SRC))

INC = -I/usr/include/libxml2
LIB = -lxml2

#CFLAGS	 = -O -Wall -fPIC -g3 -m32
CFLAGS	 = -O -Wall -fPIC -g3
SOFLAGS	 = -shared
CC	 = g++
CCC	 = gcc

all: libxmlpreprocessor.so libxmlpreprocessor.a

# Tool invocations
%.o: %.cpp %.h
	$(CC) -o $@ -c $< $(CFLAGS) $(INC) 
	
libxmlpreprocessor.so: $(OBJ)
	$(CC) $(SOFLAGS) -Wl,-soname,$@ -o $@ $^ $(CFLAGS) $(LIB)

libxmlpreprocessor.a: $(OBJ)
	ar rvs $@ $^

# Other Targets
clean:
	rm -rf *.so
	rm -rf *.a
	rm -rf *.o

.PHONY: all clean dependents
.SECONDARY:

