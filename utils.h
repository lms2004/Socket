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

void Send_mymsg(long fileSize, FILE* file, char* file_type, int client_socket, char* read_buffer, char* write_buffer) {
    /* 大文件分片读取发送 */
    printf("--------------------------\n");
    printf("File size: %ld\n", fileSize);
    printf("File type: %s\n", file_type);
    printf("Sending file...\n");
    while(fileSize > 0){
        size_t bytes_to_read = (fileSize < WRITE_BUF_SIZE) ? fileSize : WRITE_BUF_SIZE;
        
        // 从文件读取数据
        size_t bytes_read = Fread(write_buffer, sizeof(char), bytes_to_read, file);
        if(bytes_read == 0){
            break;
        }
        
        /* 发送消息 */
        msghdr *__message = Create_msghdr((char**)&write_buffer, &bytes_read, 1);
        Sendmsg(client_socket, __message, 0);
        free(__message->msg_iov);
        free(__message);

        // 清空缓存
        memset(read_buffer, 0, sizeof(read_buffer));
        memset(write_buffer, 0, sizeof(write_buffer));

        // 更新剩余文件大小
        fileSize -= bytes_read;
    }
    printf("--------------------------\n");
}

