#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 函数声明
void encryptFile(const char* inputPath, const char* outputPath, const char* key);
void decryptFile(const char* inputPath, const char* outputPath, const char* key);
int compareFiles(const char* file1, const char* file2);
void readPath(char* buffer, size_t size, const char* prompt);

int main() {
    int choice;
    char inputPath[256], outputPath[256], key[256];
    char originalFile[256], decryptedFile[256];
    
    printf("文件加解密工具\n");
    printf("1. 文件加密\n");
    printf("2. 文件解密\n");
    printf("3. 校验文件\n");
    printf("请选择操作 (1-3): ");
    scanf("%d", &choice);
    getchar(); // 消除缓冲区的换行符
    
    switch (choice) {
        case 1: // 文件加密
            readPath(inputPath, sizeof(inputPath), "请输入要加密的文件路径");
            readPath(outputPath, sizeof(outputPath), "请输入加密后的文件保存路径");
            
            do {
                printf("请输入加密密钥: ");
                fgets(key, sizeof(key), stdin);
                key[strcspn(key, "\n")] = 0;
                if(strlen(key) == 0) {
                    printf("错误：密钥不能为空，请重新输入\n");
                }
            } while(strlen(key) == 0);
            
            encryptFile(inputPath, outputPath, key);
            printf("文件加密完成，已保存到: %s\n", outputPath);
            break;
            
        case 2: // 文件解密
            readPath(inputPath, sizeof(inputPath), "请输入要解密的文件路径");
            readPath(outputPath, sizeof(outputPath), "请输入解密后的文件保存路径");
            
            do {
                printf("请输入解密密钥: ");
                fgets(key, sizeof(key), stdin);
                key[strcspn(key, "\n")] = 0;
                if(strlen(key) == 0) {
                    printf("错误：密钥不能为空，请重新输入\n");
                }
            } while(strlen(key) == 0);
            
            decryptFile(inputPath, outputPath, key);
            printf("文件解密完成，已保存到: %s\n", outputPath);
            break;
            
        case 3: // 校验文件
            readPath(originalFile, sizeof(originalFile), "请输入原始文件路径");
            readPath(decryptedFile, sizeof(decryptedFile), "请输入解密后的文件路径");
            
            if (compareFiles(originalFile, decryptedFile)) {
                printf("校验成功: 文件内容一致\n");
            } else {
                printf("校验失败: 文件内容不一致\n");
            }
            break;
            
        default:
            printf("无效选择\n");
            break;
    }
    
    return 0;
}

// 帮助用户更安全地输入文件路径
void readPath(char* buffer, size_t size, const char* prompt) {
    printf("%s (直接输入路径，无需引号): ", prompt);
    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = 0; // 去除换行符
    
    // 输出确认信息
    printf("您输入的路径是: %s\n", buffer);
}

// 使用异或加密算法对文件进行加密
void encryptFile(const char* inputPath, const char* outputPath, const char* key) {
    FILE *inFile, *outFile;
    unsigned char buffer[1024];
    size_t bytesRead;
    size_t keyLen = strlen(key);
    size_t i, j;
    
    // 打开输入文件
    inFile = fopen(inputPath, "rb");
    if (inFile == NULL) {
        printf("无法打开输入文件: %s\n", inputPath);
        return;
    }
    
    // 创建输出文件
    outFile = fopen(outputPath, "wb");
    if (outFile == NULL) {
        printf("无法创建输出文件: %s\n", outputPath);
        fclose(inFile);
        return;
    }
    
    // 读取输入文件，进行加密，并写入输出文件
    j = 0; // 密钥索引
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), inFile)) > 0) {
        for (i = 0; i < bytesRead; i++) {
            // 使用异或加密
            buffer[i] = buffer[i] ^ key[j];
            j = (j + 1) % keyLen; // 循环使用密钥
        }
        
        fwrite(buffer, 1, bytesRead, outFile);
    }
    
    fclose(inFile);
    fclose(outFile);
}

// 使用异或加密算法对文件进行解密（解密过程与加密过程相同）
void decryptFile(const char* inputPath, const char* outputPath, const char* key) {
    // 由于异或加密的可逆性，解密过程和加密过程相同
    encryptFile(inputPath, outputPath, key);
}

// 比较两个文件内容是否相同
int compareFiles(const char* file1, const char* file2) {
    FILE *f1, *f2;
    unsigned char buffer1[1024], buffer2[1024];
    size_t bytesRead1, bytesRead2;
    int result = 1; // 默认文件内容相同
    
    f1 = fopen(file1, "rb");
    if (f1 == NULL) {
        printf("无法打开文件: %s\n", file1);
        return 0;
    }
    
    f2 = fopen(file2, "rb");
    if (f2 == NULL) {
        printf("无法打开文件: %s\n", file2);
        fclose(f1);
        return 0;
    }
    
    // 逐块比较文件内容
    while (1) {
        bytesRead1 = fread(buffer1, 1, sizeof(buffer1), f1);
        bytesRead2 = fread(buffer2, 1, sizeof(buffer2), f2);
        
        // 检查读取的字节数是否相同
        if (bytesRead1 != bytesRead2) {
            result = 0; // 文件长度不同
            break;
        }
        
        // 如果已经读到文件末尾，退出循环
        if (bytesRead1 == 0) {
            break;
        }
        
        // 比较内容
        if (memcmp(buffer1, buffer2, bytesRead1) != 0) {
            result = 0; // 内容不同
            break;
        }
    }
    
    fclose(f1);
    fclose(f2);
    return result;
}
