#include "myfunc.h"
#include "utils.h"
#include "crypto_utils.h"

#define MESSAGE_MAX 4096
#define SSL_RSA 0
#define SSL_AES 1
#define SSL_HMAC 2

int main(int argc, char** argv) {
    if(argc < 4){
        printf("Usage: %s <IP> <File> <mode>\n", argv[0]);
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
    printf("Client started with SSL handshake...\n");
    if(strcmp(argv[3], "ssl") == 0){
        // (1) 发送客户端 Hello
        ClientHello client_hello;
        client_hello.cipherSuitesLength = 3;
        client_hello.cipherSuites[0] = SSL_RSA;
        client_hello.cipherSuites[1] = SSL_AES;
        client_hello.cipherSuites[2] = SSL_HMAC;
        generate_random_bytes(client_hello.clientRandom, 32);
        send(client_socket, &client_hello, sizeof(client_hello), 0);
        printf("    Sent ClientHello\n");

        // (2) 接收服务器 Hello
        ServerHello server_hello;
        recv(client_socket, &server_hello, sizeof(server_hello), 0);
        printf("    Received ServerHello\n");


        /* 验证服务器证书 */
        int file = Open("./client_key/server.crt", O_WRONLY | O_CREAT | O_TRUNC, 0666); 
        Write(file, server_hello.serverCertificate, server_hello.serverCertificateLength);
        printf("    Received ServerCertificate: %d btyes\n", server_hello.serverCertificateLength);
        close(file);

        FILE *fp = popen("openssl verify -CAfile ./client_key/root.crt ./client_key/server.crt", "r");
        fread(read_buffer , sizeof(char), READ_BUF_SIZE, fp);

        if(strstr(read_buffer , "OK") != NULL){
            printf("    Server certificate is successfully verified.\n");
        }else{
            printf("    Server certificate verification failed.\n");
            return 1;
        }
        memset(read_buffer, 0, READ_BUF_SIZE);
        pclose(fp);

        /* 加载公钥 */
        FILE *fp2 = popen("openssl x509 -in ./client_key/server.crt -pubkey -noout ", "r");
        fread(read_buffer, sizeof(char), READ_BUF_SIZE, fp2);
        BIO* bio = BIO_new_mem_buf(read_buffer, -1);
        EVP_PKEY* publicKey = PEM_read_bio_PUBKEY(bio, NULL, NULL, NULL);
        
        printf("    Loaded public key.\n");


        if(fork() == 0){ // 删除临时对比的crt文件
            const char *args[] = { "rm", "./client_key/server.crt", nullptr };
            execv("/bin/rm", const_cast<char *const *>(args));
            // 如果 execv 返回，说明出错
            std::cerr << "Exec failed.\n";
            return 1;
        }
        printf("    delete server.crt\n");


        // (3) 生成 MS ，发送 EMS
        unsigned char MS[32] = {0};
        generate_random_bytes(MS, 32);

        unsigned char* encrypted = (unsigned char*)malloc(256);
        encrypt_data(MS, encrypted, publicKey);
        send(client_socket, encrypted, 256, 0);
        printf("    Sent encrypted MS\n");

        printf("SSL handshake completed.\n");
    }
    

    /* SSL 握手完成 */

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