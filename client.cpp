#include "myfunc.h"
#include "utils.h"

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

    /* 分配读写缓冲区 */
    char* read_buffer = (char *)malloc(READ_BUF_SIZE*sizeof(char));
    char* write_buffer = (char *)malloc(WRITE_BUF_SIZE*sizeof(char));
    memset(read_buffer, 0, READ_BUF_SIZE);
    memset(write_buffer, 0, WRITE_BUF_SIZE);
    
    /* SSL 握手 */
    // ...


    /* 发送文件名 */
    strcpy(write_buffer, argv[2]);

    Send(client_socket, write_buffer, strlen(write_buffer), 0);
    printf("Send file name: %s\n", write_buffer);
    
    char file_path[WRITE_BUF_SIZE] = "./client_data/";
    strcat(file_path, write_buffer);
    
    /* 若文件不存在，则创建文件， */  
    int file = Open(file_path, O_WRONLY | O_CREAT | O_TRUNC, 0666); 

    /* 接收文件 */
    ssize_t bytes_received;  // 保存实际接收的字节数

    msghdr * __message = (msghdr *)malloc(sizeof(msghdr));
    InitRecvMsg(__message, &read_buffer, READ_BUF_SIZE, 1);
    
    while ((bytes_received = recvmsg(client_socket, __message, 0)) > 0) {
        Write(file, read_buffer, bytes_received);
    }
    
    /* 关闭文件 */
    close(file);

    /* 关闭套接字 */
    close(client_socket);

    memset(read_buffer, 0, READ_BUF_SIZE);
    memset(write_buffer, 0, WRITE_BUF_SIZE);
    printf("File received successfully!\n");
    printf("Saved to: %s\n", file_path);
    memset(file_path, 0, WRITE_BUF_SIZE);
}