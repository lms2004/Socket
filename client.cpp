#include "myfunc.h"

#define READ_BUF_SIZE 4096
#define WRITE_BUF_SIZE 1024

#define MESSAGE_MAX 4096


int main(int argc, char** argv) {
    if(argc < 3){
        printf("Usage: %s <IP> <File>\n", argv[0]);
        return 1;
    }
    int client_socket = Socket(AF_INET, SOCK_STREAM, 0);

    /* 服务器地址信息 */
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(8080);

    Connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    printf("Connected to server...\n");

    char file_name[WRITE_BUF_SIZE] = {0};
    char read_buffer[READ_BUF_SIZE] = {0};
    strcpy(file_name, argv[2]);

    Send(client_socket, file_name, strlen(file_name), 0);
    printf("Send file name: %s\n", file_name);
    
    char file_path[WRITE_BUF_SIZE] = "./client_data/";
    strcat(file_path, file_name);
    
    /* 若文件不存在，则创建文件， */  
    int file = Open(file_path, O_WRONLY | O_CREAT | O_TRUNC, 0666); 

    /* 接收文件 */
    ssize_t bytes_received;  // 保存实际接收的字节数

    msghdr * __message = (msghdr *)malloc(sizeof(msghdr));
    InitRecvMsg(__message, read_buffer, READ_BUF_SIZE);
    
    while ((bytes_received = recvmsg(client_socket, __message, 0)) > 0) {
        Write(file, read_buffer, bytes_received);
    }
    
    /* 关闭文件 */
    close(file);

    /* 关闭套接字 */
    close(client_socket);

    memset(read_buffer, 0, READ_BUF_SIZE);
    memset(file_name, 0, WRITE_BUF_SIZE);
    printf("File received successfully!\n");
    printf("Saved to: %s\n", file_path);
    memset(file_path, 0, WRITE_BUF_SIZE);
}