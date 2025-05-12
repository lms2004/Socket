#include "myfunc.h"
#include "utils.h"
#include "crypto_utils.h"

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

    // SSL 握手
    printf("Server started with SSL handshake...\n");

    if(strcmp(argv[1], "ssl") == 0){
        /* (1) 接收 Client Hello 并发送 Server Hello */
        ClientHello rev_client_hello;
        int recv_len = Recv(client_socket, &rev_client_hello, sizeof(rev_client_hello), 0);
        printf("    Received ClientHello: %d\n", recv_len);

        // 构造 Server Hello
        ServerHello server_hello;
        server_hello.cipherSuitesLength = rev_client_hello.cipherSuitesLength;
        memcpy(server_hello.cipherSuites, rev_client_hello.cipherSuites, sizeof(rev_client_hello.cipherSuites));

        generate_random_bytes(server_hello.serverRandom, 32); // 生成随机数

        
        read_certificate(&server_hello); // 填充公钥证书

        send(client_socket, &server_hello, sizeof(server_hello), 0);
        printf("    Sent ServerHello\n");



        /* 加载公钥 */
        FILE *fp2 = popen("openssl x509 -in ./server_key/server.crt -pubkey -noout ", "r");
        fread(read_buffer, sizeof(char), READ_BUF_SIZE, fp2);
        BIO* bio = BIO_new_mem_buf(read_buffer, -1);
        EVP_PKEY* publicKey = PEM_read_bio_PUBKEY(bio, NULL, NULL, NULL);
        if (!publicKey) {
            std::cerr << "Error loading public key." << std::endl;
            return 1;
        }
        
        printf("    Loaded public key.\n");



        return 0;
    }


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
    memset(read_buffer, 0, READ_BUF_SIZE);
    memset(write_buffer, 0, WRITE_BUF_SIZE);
}

