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
    GF_ISOM_BOX_TYPE_MOOV    = GF_4CC( 'm', 'o', 'o', 'v' ),
    GF_ISOM_BOX_TYPE_MVHD    = GF_4CC( 'm', 'v', 'h', 'd' ),
};

#define GF_ISOM_BOX            \
u32 type;            \
u64 size;


#define GF_ISOM_FULL_BOX        \
GF_ISOM_BOX            \
u8 version;            \
u32 flags;            \

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

struct MovieHeaderBox {
    GF_ISOM_FULL_BOX
    u64 creation_time;
    u64 modification_time;
    u32 time_scale;
    u64 duration;
    
    u32 rate;
    u16 volume;
    u8 reserved[10];
    u32 matrix[9];
    u32 pre_defined[6];
    u32 next_track_id;
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

u32 readU16() {
    makeSureBufReady(sizeof(u16));
    u32 num = readNextByte();
    num <<= 8;
    num |= readNextByte();
    return num;
}

u32 readU24() {
    makeSureBufReady(sizeof(u8) * 3);
    u32 num = readNextByte();
    num <<= 8;
    num |= readNextByte();
    num <<= 8;
    num |= readNextByte();
    return num;
}

u32 readU32() {
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

void readU32Array(u32 *arr, u32 len) {
    makeSureBufReady(sizeof(u32) * len);
    for (auto i = 0; i < len; i++) {
        arr[i] = readU32();
    }
}

u64 readU64() {
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

void readU8Array(u8 *dest, u32 len) {
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
        struct MovieHeaderBox mvhd;
        
        while (((rangeStart + 1) % BUF_LEN) < rangeEnd) {
            auto boxStartIndex = rangeStart;
            box.size = readU32();
            box.type = readU32();
            if (box.size == 1) {
                box.size  = readU64();
            }
            cout << "Found box type: ";
            printU32WithStr(box.type);
            cout << endl;
            u8 version = 0;
            u32 flags = 0;
            if (box.type == GF_ISOM_BOX_TYPE_MVHD) {
                version = readNextByte();
                flags = readU24();
            }
            switch (box.type) {
                case GF_ISOM_BOX_TYPE_FTYP:
                {
                    memcpy(&fty, &box, sizeof(box));
                    fty.major_brand = readU32();
                    fty.minor_version = readU32();
                    
                    cout << "major brand: ";
                    printU32WithStr(fty.major_brand);
                    cout << endl;
                    
                    auto readBoxLen = (rangeStart + BUF_LEN - boxStartIndex) % BUF_LEN;
                    u64 compatibleBrandsLen = box.size - readBoxLen;
                    fty.brand_count = (u32)(compatibleBrandsLen >> 2);
                    fty.compatible_brands = (u32*)malloc(compatibleBrandsLen);
                    cout << "compatible brands: ";
                    for (auto i = 0; i < fty.brand_count; i++) {
                        fty.compatible_brands[i] = readU32();
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
                {
                    memcpy(&moov, &box, sizeof(box));
//                    skip((u32)box.size - ((rangeStart + BUF_LEN - boxStartIndex) % BUF_LEN));
                }
                    break;
                case GF_ISOM_BOX_TYPE_MVHD:
                {
                    memcpy(&mvhd, &box, sizeof(box));
                    if (version == 1) {
                        mvhd.creation_time = readU64();
                        mvhd.modification_time = readU64();
                        mvhd.time_scale = readU32();
                        mvhd.duration = readU64();
                    } else {
                        mvhd.creation_time = readU32();
                        mvhd.modification_time = readU32();
                        mvhd.time_scale = readU32();
                        mvhd.duration = readU32();
                    }
                    mvhd.flags = flags;
                    mvhd.version = version;
                    mvhd.rate = readU32();
                    mvhd.volume = readU16();
                    readU8Array(mvhd.reserved, sizeof(mvhd.reserved));
                    readU32Array(mvhd.matrix, sizeof(mvhd.matrix) / sizeof(u32));
                    readU32Array(mvhd.pre_defined, sizeof(mvhd.pre_defined) / sizeof(u32));
                    mvhd.next_track_id = readU32();
                }
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
