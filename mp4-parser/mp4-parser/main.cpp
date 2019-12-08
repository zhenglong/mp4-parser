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
    GF_ISOM_BOX_TYPE_FREE    = GF_4CC( 'f', 'r', 'e', 'e' ),
    GF_ISOM_BOX_TYPE_MDAT    = GF_4CC( 'm', 'd', 'a', 't' ),
    GF_ISOM_BOX_TYPE_MOOV    = GF_4CC( 'm', 'o', 'o', 'v' )
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

/*
 * 内容直接忽略
 */
struct FreeSpaceBox {
    GF_ISOM_BOX
};

struct MediaDataBox {
    GF_ISOM_BOX
    u64 data_size;
    u8 *data;
};

struct MovieBox {
    GF_ISOM_BOX
    
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

// 环形缓存
const static int BUF_LEN = 4096;
char buf[BUF_LEN];
u32 rangeStart = 0; // 可用数据的起始索引
u32 rangeEnd = BUF_LEN - 1; // 可用数据的结束索引
ifstream file;

u8 readNextByte() {
    u8 res = (u8)buf[rangeStart];
    rangeStart = (rangeStart + 1) % BUF_LEN;
    return  res;
}

void makeSureBufReady(u32 requestedBytes) {
    // 如果buf不足以完成本次读取，则填满buf的可用空间
    // TODO: 暂时不处理：1. 到达文件结尾的情况 2. 请求的字节数大于BUF_LEN
    if (((rangeStart + requestedBytes - 1) % BUF_LEN) > rangeEnd) {
        auto availableStartIndex = (rangeEnd + 1) % BUF_LEN;
        
        file.read(buf + availableStartIndex, BUF_LEN - rangeEnd);
        file.read(buf, rangeStart);
        rangeEnd = (rangeStart - 1 + BUF_LEN) % BUF_LEN;
    }
}

u32 readU32(char *buf) {
    makeSureBufReady(sizeof(u32));
    u32 num = readNextByte();
    num <<= 8;
    num |= readNextByte();
    num <<= 8;
    num |= readNextByte();
    num <<= 8;
    num |= readNextByte();
    return num;
}

u64 readU64(char *buf) {
    makeSureBufReady(sizeof(u64));
    
    u64 num = readNextByte();
    num <<= 8;
    num |= readNextByte();
    num <<= 8;
    num |= readNextByte();
    num <<= 8;
    num |= readNextByte();
    num <<= 8;
    
    num |= readNextByte();
    num <<= 8;
    num |= readNextByte();
    num <<= 8;
    num |= readNextByte();
    num <<= 8;
    num |= readNextByte();
    
    return num;
}

void skip(u32 bytes) {
    // 如果跳过的字节数大于BUF_LEN，则直接清空缓存，然后seek到指定位置
    if (bytes > BUF_LEN) {
        memset(buf, 0, BUF_LEN);
        auto jumpLen = bytes - ((rangeEnd + 1 + BUF_LEN - rangeStart) % BUF_LEN);
        file.seekg(jumpLen, ios_base::seek_dir::cur);
        rangeStart = 0;
        if (!file.eof()) {
            file.read(buf, BUF_LEN);
            rangeEnd = (u32)file.gcount() - 1;
        }
    } else {
        makeSureBufReady(bytes);
        rangeStart = (rangeStart + bytes) % BUF_LEN;
    }
}

void readCharArray(char *buf, u32 len, u8 *dest) {
    // TODO: 暂不处理len大于BUF_LEN的情况
    makeSureBufReady(len);
    auto expectedRangStart = (rangeStart + len) % BUF_LEN;
    if (expectedRangStart > rangeStart) {
        memcpy(dest, buf + rangeStart, len);
        rangeStart = expectedRangStart;
    } else {
        auto firstPartLen =BUF_LEN - rangeStart;
        memcpy(dest, buf + rangeStart, firstPartLen);
        memcpy(dest + firstPartLen, buf, len - firstPartLen);
    }
}

int main(int argc, const char * argv[]) {
    file.open("/Users/hujiang/Downloads/1.mp4");
    memset(buf, 0,  BUF_LEN);
    if (file.is_open()) {
        file.read(buf, BUF_LEN);
        // 读取的字节数肯定小于等于BUF_LEN
        rangeEnd = (u32)file.gcount() - 1;
        struct FileTypeBox fty;
        struct Box box;
        struct MediaDataBox mdat;
        struct MovieBox moov;
        
        while (((rangeStart + 1) % BUF_LEN) < rangeEnd) {
            auto boxStartIndex = rangeStart;
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
                    
                    cout << "major brand: ";
                    printU32WithStr(fty.major_brand);
                    cout << endl;
                    
                    auto readBoxLen = (rangeStart + BUF_LEN - boxStartIndex) % BUF_LEN;
                    u64 compatibleBrandsLen = box.size - readBoxLen;
                    fty.brand_count = (u32)(compatibleBrandsLen >> 2);
                    fty.compatible_brands = (u32*)malloc(compatibleBrandsLen);
                    cout << "compatible brands: ";
                    for (auto i = 0; i < fty.brand_count; i++) {
                        fty.compatible_brands[i] = readU32(buf);
                        printU32WithStr(fty.compatible_brands[i]);
                        cout << ' ';
                    }
                    cout << endl;
                }
                    break;
                case GF_ISOM_BOX_TYPE_FREE:
                    skip((u32)box.size - ((rangeStart + BUF_LEN - boxStartIndex) % BUF_LEN));
                    break;
                case GF_ISOM_BOX_TYPE_MDAT:
                {
                    memcpy(&mdat, &box, sizeof(box));
                    skip((u32)box.size - ((rangeStart + BUF_LEN - boxStartIndex) % BUF_LEN));
                }
                    break;
                case GF_ISOM_BOX_TYPE_MOOV:
                    memcpy(&moov, &box, sizeof(box));
                    skip((u32)box.size - ((rangeStart + BUF_LEN - boxStartIndex) % BUF_LEN));
                    break;
                default:
                    readNextByte();
                    break;
            }
        }
        file.close();
    }
    return 0;
}
