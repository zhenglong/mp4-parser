//
//  main.cpp
//  mp4-parser
//
//  Created by HJ on 2019/12/5.
//  Copyright © 2019 HJ. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

using namespace std;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;

#define GF_4CC(a,b,c,d) (((a)<<24)|((b)<<16)|((c)<<8)|(d))

enum {
    GF_ISOM_BOX_TYPE_FTYP    = GF_4CC( 'f', 't', 'y', 'p' ),
};

#define GF_ISOM_BOX            \
u32 type;            \
u64 size;


struct Box {
    // 长度是Box大小+具体Box大小
    // 如果size为1，则长度为box_length64；
    // 如果size为0，则为最后一个box，文件结尾即为该box结尾。
    GF_ISOM_BOX
};

struct FileTypeBox {
    GF_ISOM_BOX
    u32 major_brand;
    u32 minor_version;
    u32 brand_count;
    u32 *compatible_brands;
};

void printU32WithStr(u32 val) {
    char str[5];
    memset(str, 0, sizeof(str));
    str[0] = val >> 24 & 0xFF;
    str[1] = val >> 16 & 0xFF;
    str[2] = val >> 8 & 0xFF;
    str[3] = val & 0xFF;
    cout << str;
}

unsigned int currentIndex = 0;

unsigned int readU32(char *buf) {
    unsigned int num = buf[currentIndex++];
    num <<= 8;
    num |= buf[currentIndex++];
    num <<= 8;
    num |= buf[currentIndex++];
    num <<= 8;
    num |= buf[currentIndex++];
    return num;
}

unsigned long readU64(char *buf) {
    unsigned long num = buf[currentIndex++];
    num <<= 8;
    num |= buf[currentIndex++];
    num <<= 8;
    num |= buf[currentIndex++];
    num <<= 8;
    num |= buf[currentIndex++];
    num <<= 8;
    
    num |= buf[currentIndex++];
    num <<= 8;
    num |= buf[currentIndex++];
    num <<= 8;
    num |= buf[currentIndex++];
    num <<= 8;
    num |= buf[currentIndex++];
    
    return num;
}

void readCharArray(char *buf, unsigned int len, char *dest) {
    memcpy(dest, buf + currentIndex, len);
    currentIndex += len;
}

int main(int argc, const char * argv[]) {
    
    ifstream file;
    file.open("/Users/hujiang/Downloads/1.mp4");
    const static int BUF_LEN = 4096;
    char buf[BUF_LEN];
    memset(buf, 0,  BUF_LEN);
    if (file.is_open()) {
        file.read(buf, BUF_LEN);
        auto readLength = file.gcount();
        struct FileTypeBox fty;
        struct Box box;
        
        while (currentIndex < readLength) {
            auto boxStartIndex = currentIndex;
            box.size = readU32(buf);
            box.type = readU32(buf);
            if (box.size == 1) {
                box.size  = readU64(buf);
            }
            cout << "Found box type: ";
            printU32WithStr(box.type);
            cout << endl;
            switch (box.type) {
                case GF_ISOM_BOX_TYPE_FTYP:
                {
                    memcpy(&fty, &box, sizeof(box));
                    fty.major_brand = readU32(buf);
                    fty.minor_version = readU32(buf);
                    
                    u64 compatibleBrandsLen = box.size - (currentIndex - boxStartIndex);
                    fty.brand_count = (u32)(compatibleBrandsLen >> 2);
                    fty.compatible_brands = (u32*)malloc(compatibleBrandsLen);
                    memcpy(fty.compatible_brands, buf + currentIndex, compatibleBrandsLen);
                    cout << "compatible brands: ";
                    for (auto i = 0; i < fty.brand_count; i++) {
                        printU32WithStr(fty.compatible_brands[i]);
                        cout << ' ';
                    }
                    cout << endl;
                }
                    break;
                default:
                    currentIndex++;
                    break;
            }
        }
        file.close();
    }
    return 0;
}
