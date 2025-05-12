#include "myfunc.h"
#include "utils.h"


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

    /* 分配读写缓冲区 */
    char* read_buffer = (char *)malloc(READ_BUF_SIZE*sizeof(char));
    char* write_buffer = (char *)malloc(WRITE_BUF_SIZE*sizeof(char));
    memset(read_buffer, 0, READ_BUF_SIZE);
    memset(write_buffer, 0, WRITE_BUF_SIZE);

    // // SSL 握手
    // while(1){
    //     if(Recv(client_socket, read_buffer, READ_BUF_SIZE, 0) > 0) {
    //         if(strcmp(read_buffer, "hello") == 0) {
    //             Send(client_socket, "hello", strlen("hello"), 0);
    //             break;
    //         }
    //     }
    // }


    /* 接收文件名 */
    Recv(client_socket, read_buffer, READ_BUF_SIZE, 0);

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

    Send_mymsg(fileSize, file, file_type, client_socket, read_buffer, write_buffer);

    /* 关闭文件和套接字 */
    fclose(file);
    close(client_socket);
    close(server_socket);

    printf("File sent successfully!\n");
    memset(read_buffer, 0, sizeof(read_buffer));
    memset(write_buffer, 0, sizeof(write_buffer));
}

