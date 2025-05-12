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

// 客户端：公钥加密
int encrypt_data(const unsigned char* plaintext, unsigned char* ciphertext, EVP_PKEY* pub_key) {
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pub_key, NULL);
    size_t ciphertext_len;
    EVP_PKEY_encrypt_init(ctx);
    EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING); // 设置填充方式
    EVP_PKEY_encrypt(ctx, ciphertext, &ciphertext_len, plaintext, 32);
    EVP_PKEY_CTX_free(ctx);
    return ciphertext_len;
}

// 服务器：私钥解密
int decrypt_data(const unsigned char* ciphertext, unsigned char* plaintext, EVP_PKEY* priv_key) {
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(priv_key, NULL);
    size_t plaintext_len;
    EVP_PKEY_decrypt_init(ctx);
    EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING); // 与客户端填充方式一致
    EVP_PKEY_decrypt(ctx, plaintext, &plaintext_len, ciphertext, 256); // 假设加密后数据为256字节
    EVP_PKEY_CTX_free(ctx);
    return plaintext_len;
}


#endif // CRYPTO_UTILS_H
