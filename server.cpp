#include "myfunc.h"

#define READ_BUF_SIZE 8192
#define WRITE_BUF_SIZE 8192


int main(int argc, char** argv) {
    int server_socket = Socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(8080);

    Bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));

    Listen(server_socket, 5);

    printf("Server listening on port 8080...\n");

    socklen_t addrlen = sizeof(server_addr);
    int client_socket = Accept(server_socket, (struct sockaddr*)&server_addr, &addrlen);
    
    printf("Client connected: %s:%d\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));


    char* read_buffer = (char *)malloc(READ_BUF_SIZE*sizeof(char));
    char* write_buffer = (char *)malloc(WRITE_BUF_SIZE*sizeof(char));
    memset(read_buffer, 0, READ_BUF_SIZE);
    memset(write_buffer, 0, WRITE_BUF_SIZE);


    /* 接收文件名 */
    recv(client_socket, read_buffer, READ_BUF_SIZE, 0);

    printf("Received file name: %s\n", read_buffer);

    /* 读取文件 */
    char data_dir[2048] = "./server_data/";
    strcat(data_dir, read_buffer);
    FILE* file = Fopen(data_dir, "rb");

    printf("Finding file: %s\n", data_dir);


    long fileSize = WRITE_BUF_SIZE;

    /* 解析文件名 */
    char* file_type = parse_file_name(read_buffer);
    if(strcmp(file_type, "jpg") == 0 || strcmp(file_type, "png") == 0 || strcmp(file_type, "jpeg") == 0) {
        // 获取文件大小
        fseek(file, 0, SEEK_END);
        fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);
    }

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
        msghdr *__message = Create_msghdr(write_buffer, bytes_read); 
        Sendmsg(client_socket, __message, 0);
        free(__message);

        // 清空缓存
        memset(read_buffer, 0, sizeof(read_buffer));
        memset(write_buffer, 0, sizeof(write_buffer));

        // 更新剩余文件大小
        fileSize -= bytes_read;
    }
    printf("--------------------------\n");

    /* 关闭文件和套接字 */
    fclose(file);
    close(client_socket);
    close(server_socket);

    printf("File sent successfully!\n");
    memset(read_buffer, 0, sizeof(read_buffer));
    memset(write_buffer, 0, sizeof(write_buffer));

}