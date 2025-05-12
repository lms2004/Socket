#include "myfunc.h"


#define READ_BUF_SIZE 8192
#define WRITE_BUF_SIZE 8192

// 发送端初始化
msghdr *Create_msghdr(char** messages, size_t* messages_len, int num_messages) { 
    msghdr *__message = (msghdr *)malloc(sizeof(msghdr));
    memset(__message, 0, sizeof(msghdr));

    // 分配足够的内存空间来存储多个 iovec
    __message->msg_iov = (iovec *)malloc(num_messages * sizeof(iovec));

    // 循环为每个 iovec 设置数据
    for (int i = 0; i < num_messages; i++) {
        __message->msg_iov[i].iov_base = messages[i];
        __message->msg_iov[i].iov_len = messages_len[i];
    }

    // 设置 msg_iovlen 为 num_messages
    __message->msg_iovlen = num_messages;

    // 控制信息保持为空
    __message->msg_control = NULL;
    __message->msg_controllen = 0;
    __message->msg_flags = 0;

    return __message;
}

// 接收端初始化
void InitRecvMsg(struct msghdr *msg, char **buffers, size_t buf_sizes, int num_buffers) {
    // 分配足够的内存来存储多个 iovec
    struct iovec *iov = (iovec *)malloc(num_buffers * sizeof(struct iovec));
    
    // 设置每个 iovec 的 base 和 length
    for (int i = 0; i < num_buffers; i++) {
        iov[i].iov_base = buffers[i];
        iov[i].iov_len = buf_sizes;
    }

    // 清空 msghdr 结构体
    memset(msg, 0, sizeof(*msg));

    // 设置 msghdr 的 iovec 和 iovec 长度
    msg->msg_iov = iov;
    msg->msg_iovlen = num_buffers;
}


char* parse_file_name(char* file_name) {
    for(char* p = file_name; *p != '\0'; p++) {
        if(*p == '.') {
            return p + 1;
        }
    }
    return NULL;
}

