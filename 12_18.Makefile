CC = gcc

CFLAGS = -Wall -g -I./include -Ibus/bus -Ibus/controller -Ibus/adapter

# 定义源文件路径
BUS_DIR = bus/bus
CONTROLLER_DIR = bus/controller
ADAPTER_DIR = bus/adapter
BRIDGE_DIR = bridge/manager
AUTH_DIR = bridge/authentication
TRANS_DIR = bridge/transmission
INFO_DIR = bus/discovery
ROOT_DIR = .

# 源文件
SRCS = $(ROOT_DIR)/main.c \
	   $(BUS_DIR)/softbus_manager.c\
	   $(BRIDGE_DIR)/bridge_manager.c\
	   $(INFO_DIR)/dis_manager.c\
	   $(AUTH_DIR)/auth_manager.c\
	   $(CONTROLLER_DIR)/control_manger.c\
	   $(CONTROLLER_DIR)/soft_arbiter.c\
	   $(CONTROLLER_DIR)/soft_control.c\
	   $(ADAPTER_DIR)/adapter_manager.c\
	   $(ADAPTER_DIR)/soft_adapter.c 
# main.c  softbus_manager.c control_manager.c soft_arbiter.c soft_control.c  adapter_manager.c soft_adapter.c 

# 目标文件 (自动生成 .o 文件列表)
OBJS = $(SRCS:.c=.o)

# 最终生成的可执行文件
TARGET = main

# 默认目标
all: $(TARGET)

# 链接目标文件生成最终可执行文件
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

# 编译每个 .c 文件生成 .o 文件
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 清理生成的文件
clean:
	rm -f $(OBJS) $(TARGET)

# 伪目标，不生成文件
.PHONY: all clean