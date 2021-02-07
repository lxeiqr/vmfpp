PREFIX ?= /usr/local
AR ?= ar
CXX ?= clang++

CXXFLAGS := -Wall -Wextra -fstack-protector-strong -O2 -pipe \
    -Werror=format-security -I/usr/local/include -std=c++17 -fPIC
LDFLAGS := -fPIC

OBJ = vmf.o 

all: libvmf.so libvmf.a

libvmf.so: $(OBJ)
	$(CXX) -shared $(LDFLAGS) -o $@ $(OBJ) 


libvmf.a: $(OBJ)
	$(AR) rcs $@ $(OBJ)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	-rm -r libvmf.a libvmf.so $(OBJ)

install:
	mkdir -p $(PREFIX)/lib
	mkdir -p $(PREFIX)/include
	install -m 0755 libvmf.so $(PREFIX)/lib
	install -m 0755 libvmf.a $(PREFIX)/lib
	install -m 0644 vmf.hpp $(PREFIX)/include

uninstall:
	-rm -f $(PREFIX)/lib/libvmf.so 
	-rm -f $(PREFIX)/lib/libvmf.a 
	-rm -f $(PREFIX)/include/vmf.h

.PHONY: clean install uninstall
.SUFFIXES: .cpp .o
