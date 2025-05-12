#ifndef CRYPTO_UTILS_H
#define CRYPTO_UTILS_H


#include <openssl/aes.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <iostream>
#include <cstring>
#include <string>
#include <random>

#define CLIENT_HELLO_MAX_CIPHERS 3  // 最大支持的密码算法数量

#define SSL_RSA 0
#define SSL_AES_ 1
#define SSL_HMAC 2


typedef struct{
    unsigned char clientRandom[32];  // 32字节的客户端随机数
    uint16_t cipherSuitesLength;     // 密码算法列表的长度
    unsigned short cipherSuites[CLIENT_HELLO_MAX_CIPHERS];  // 支持的密码算法列表
}ClientHello;

typedef struct{
    unsigned char serverRandom[32];  // 服务器随机数（32字节）
    uint16_t cipherSuitesLength;     // 密码算法列表的长度
    unsigned short cipherSuites[CLIENT_HELLO_MAX_CIPHERS];  // 选择密码算法列表
    unsigned char serverCertificate[2048];  // 服务器证书
    uint16_t serverCertificateLength;     // 服务器证书长度
}ServerHello;


void read_certificate(ServerHello* server_hello) {
    FILE* file = Fopen("./server_key/server.crt", "rb");

    // 获取文件大小
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    int n = Fread(server_hello->serverCertificate, 1, file_size, file);
    server_hello->serverCertificateLength = n;
    printf("    Read server certificate: %d bytes\n", n);
    
    // 关闭文件
    fclose(file);
}

void generate_random_bytes(unsigned char* buffer, int len){
    std::random_device rd;
    for (int i = 0; i < len; ++i) {
        buffer[i] = rd() & 0xFF;
    }
}

void encrypt_data(unsigned char* MS, unsigned char* encrypted, EVP_PKEY* publicKey) {
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(publicKey, NULL);
    if (!ctx) {
        perror("EVP_PKEY_CTX_new failed");
    }

    if (EVP_PKEY_encrypt_init(ctx) <= 0) {
        perror("EVP_PKEY_encrypt_init failed");
    }

    size_t encrypted_len = 0;
    if (EVP_PKEY_encrypt(ctx, encrypted, &encrypted_len, MS, sizeof(MS)) <= 0) {
        perror("EVP_PKEY_encrypt failed");
    }
}


#endif // CRYPTO_UTILS_H
