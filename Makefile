PREFIX ?= /usr/local
AR ?= ar
CXX ?= clang++

CXXFLAGS := -Wall -Wextra -fstack-protector-strong -O2 -pipe \
    -Werror=format-security -I/usr/local/include -std=c++17 -fPIC
LDFLAGS := -fPIC

OBJ = vmfpp.o 

all: libvmfpp.so libvmfpp.a

libvmfpp.so: $(OBJ)
	$(CXX) -shared $(LDFLAGS) -o $@ $(OBJ) 


libvmfpp.a: $(OBJ)
	$(AR) rcs $@ $(OBJ)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	-rm -r libvmfpp.a libvmfpp.so $(OBJ)

install:
	mkdir -p $(PREFIX)/lib
	mkdir -p $(PREFIX)/include
	install -m 0755 libvmfpp.so $(PREFIX)/lib
	install -m 0755 libvmfpp.a $(PREFIX)/lib
	install -m 0644 vmfpp.hpp $(PREFIX)/include

uninstall:
	-rm -f $(PREFIX)/lib/libvmfpp.so 
	-rm -f $(PREFIX)/lib/libvmfpp.a 
	-rm -f $(PREFIX)/include/vmfpp.h

.PHONY: clean install uninstall
.SUFFIXES: .cpp .o
