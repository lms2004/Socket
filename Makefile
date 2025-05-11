CC = g++
CFLAGS = -Wall

# 目标可执行文件
EXECUTABLES = server client

all: $(EXECUTABLES)


# 编译 .cpp 文件为可执行文件
%.out: %.cpp
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXECUTABLES)