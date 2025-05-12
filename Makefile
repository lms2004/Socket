CC = g++
CFLAGS = -Wall
INCLUDES = -I ./include  # 头文件所在的目录

# OpenSSL 库链接
LIBS = -lssl -lcrypto

# 目标可执行文件
EXECUTABLES = server client

all: $(EXECUTABLES)

# 编译 .cpp 文件为可执行文件
%: %.cpp
	$(CC) $(INCLUDES) $(CFLAGS) $< -o $@ $(LIBS) 

clean:
	rm -f $(EXECUTABLES)
