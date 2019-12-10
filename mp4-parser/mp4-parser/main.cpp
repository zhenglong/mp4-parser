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

#include "enum_def.h"
#include "type_def.h"

using namespace std;

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

void makeSureBufReady(u32 requestedBytes) {
    // 如果buf不足以完成本次读取，则填满buf的可用空间
    // TODO: 暂时不处理：1. 到达文件结尾的情况 2. 请求的字节数大于BUF_LEN
    cout << rangeStart << ' ' << rangeEnd << endl;
    if (((rangeEnd + BUF_LEN - rangeStart) % BUF_LEN) < requestedBytes) {
        auto availableStartIndex = (rangeEnd + 1) % BUF_LEN;
        
        if (availableStartIndex > rangeStart) {
            file.read(buf + availableStartIndex, BUF_LEN - availableStartIndex);
            if (rangeStart > 0) {
                file.read(buf, rangeStart);
            }
        } else {
            file.read(buf + availableStartIndex, rangeStart - availableStartIndex);
        }
        rangeEnd = (rangeStart - 1 + BUF_LEN) % BUF_LEN;
    }
}

u8 readNextByte() {
    makeSureBufReady(sizeof(u8));
    u8 res = (u8)buf[rangeStart];
    rangeStart = (rangeStart + 1) % BUF_LEN;
    return  res;
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
void readU16Array(u16 *arr, u32 len) {
    makeSureBufReady(sizeof(u16) * len);
    for (auto i = 0; i < len; i++) {
        arr[i] = readU16();
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
    makeSureBufReady(len * sizeof(u8));
    auto expectedRangStart = (rangeStart + len) % BUF_LEN;
    if (expectedRangStart > rangeStart) {
        memcpy(dest, buf + rangeStart, len);
    } else {
        auto firstPartLen =BUF_LEN - rangeStart;
        memcpy(dest, buf + rangeStart, firstPartLen);
        memcpy(dest + firstPartLen, buf, len - firstPartLen);
    }
    rangeStart = expectedRangStart;
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
        struct TrackBox trak;
        struct TrackHeaderBox tkhd;
        struct EditBox edts;
        struct EditListBox elst;
        struct MediaBox mdia;
        struct MediaHeaderBox mdhd;
        struct HandlerBox hdlr;
        struct MediaInformationBox minf;
        struct VideoMediaHeaderBox vmhd;
        struct DataInformationBox dinf;
        struct DataReferenceBox dref;
        struct SampleTableBox stbl;
        struct SampleDescriptionBox stsd;
        struct TimeToSampleBox stts;
        struct SyncSampleBox stss;
        struct SampleToChunkBox stsc;
        struct SampleSizeBox stsz;
        struct ChunkOffsetBox stco;
        
        while (((rangeStart + 1) % BUF_LEN) != rangeEnd) {
            auto boxStartIndex = rangeStart;
            box.size = readU32();
            box.type = readU32();
            if (box.size == 1) {
                box.size  = readU64();
            }
            cout << "Found box type: ";
            printU32WithStr(box.type);
            cout << endl;
            // 继承自FULLBOX的BOX会多出2个字段
            u8 version = 0;
            u32 flags = 0;
            if (box.type == GF_ISOM_BOX_TYPE_MVHD ||
                box.type == GF_ISOM_BOX_TYPE_TKHD ||
                box.type == GF_ISOM_BOX_TYPE_ELST ||
                box.type == GF_ISOM_BOX_TYPE_MDHD ||
                box.type == GF_ISOM_BOX_TYPE_HDLR ||
                box.type == GF_ISOM_BOX_TYPE_VMHD ||
                box.type == GF_ISOM_BOX_TYPE_DREF ||
                box.type == GF_ISOM_BOX_TYPE_STSD ||
                box.type == GF_ISOM_BOX_TYPE_STTS ||
                box.type == GF_ISOM_BOX_TYPE_STSS ||
                box.type == GF_ISOM_BOX_TYPE_STSC ||
                box.type == GF_ISOM_BOX_TYPE_STSZ ||
                box.type == GF_ISOM_BOX_TYPE_STCO) {
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
                case GF_ISOM_BOX_TYPE_TRAK:
                {
                    memcpy(&trak, &box, sizeof(box));
                }
                    break;
                case GF_ISOM_BOX_TYPE_TKHD:
                {
                    memcpy(&tkhd, &box, sizeof(box));
                    tkhd.version = version;
                    tkhd.flags = flags;
                    if (version == 1) {
                        tkhd.creation_time = readU64();
                        tkhd.modification_time = readU64();
                        tkhd.track_id = readU32();
                        tkhd.reserved = readU32();
                        tkhd.duration = readU64();
                    } else {
                        tkhd.creation_time = readU32();
                        tkhd.modification_time = readU32();
                        tkhd.track_id = readU32();
                        tkhd.reserved = readU32();
                        tkhd.duration = readU32();
                    }
                    
                    readU32Array(tkhd.reserved1, sizeof(tkhd.reserved1) / sizeof(u32));
                    tkhd.layer = readU16();
                    tkhd.alternate_group = readU16();
                    tkhd.volume = readU16();
                    tkhd.reserved2 = readU16();
                    readU32Array(tkhd.matrix, sizeof(tkhd.matrix) / sizeof(u32));
                    tkhd.width = readU32();
                    tkhd.height = readU32();
                }
                    break;
                case GF_ISOM_BOX_TYPE_EDTS:
                {
                    memcpy(&edts, &box, sizeof(box));
                }
                    break;
                case GF_ISOM_BOX_TYPE_ELST:
                {
                    memcpy(&elst, &box, sizeof(box));
                    elst.version = version;
                    elst.flags = flags;
                    elst.entry_count = readU32();
                    elst.list = (EditListEntryBox*)malloc(sizeof(EditListEntryBox) * elst.entry_count);
                    if (elst.version == 1) {
                        for (auto i = 0; i < elst.entry_count; i++) {
                            elst.list[i].segment_duration = readU64();
                            elst.list[i].media_time = readU64();
                            
                            elst.list[i].media_rate_interger = readU16();
                            elst.list[i].media_rate_fraction = readU16();
                        }
                    } else {
                        for (auto i = 0; i < elst.entry_count; i++) {
                            elst.list[i].segment_duration = readU32();
                            elst.list[i].media_time = readU32();
                            
                            elst.list[i].media_rate_interger = readU16();
                            elst.list[i].media_rate_fraction = readU16();
                        }
                    }
                }
                    break;
                case GF_ISOM_BOX_TYPE_MDIA:
                {
                    memcpy(&mdia, &box, sizeof(box));
                }
                    break;
                case GF_ISOM_BOX_TYPE_MDHD:
                {
                    memcpy(&mdhd, &box, sizeof(box));
                    mdhd.version = version;
                    mdhd.flags = flags;
                    if (version == 1) {
                        mdhd.creation_time = readU64();
                        mdhd.modification_time = readU64();
                        mdhd.time_scale = readU32();
                        mdhd.duration = readU64();
                    } else {
                        mdhd.creation_time = readU32();
                        mdhd.modification_time = readU32();
                        mdhd.time_scale = readU32();
                        mdhd.duration = readU32();
                    }
                    mdhd.language = readU16();
                    mdhd.quality = readU16();
                }
                    break;
                case GF_ISOM_BOX_TYPE_HDLR:
                {
                    memcpy(&hdlr, &box, sizeof(box));
                    hdlr.version = version;
                    hdlr.flags = flags;
                    hdlr.pre_defined = readU32();
                    hdlr.handler_type = readU32();
                    readU32Array(hdlr.reserved, sizeof(hdlr.reserved) / sizeof(u32));
                    hdlr.name_length = (u32)hdlr.size - ((rangeStart + BUF_LEN - boxStartIndex) % BUF_LEN);
                    hdlr.name = (char*)malloc(sizeof(char) * hdlr.name_length);
                    readU8Array((u8*)hdlr.name, hdlr.name_length);
                    
                    cout << "handler_type: ";
                    printU32WithStr(hdlr.handler_type);
                    cout << endl;
                }
                    break;
                case GF_ISOM_BOX_TYPE_MINF:
                {
                    memcpy(&minf, &box, sizeof(box));
                }
                    break;
                case GF_ISOM_BOX_TYPE_VMHD:
                {
                    memcpy(&vmhd, &box, sizeof(box));
                    vmhd.version = version;
                    vmhd.flags = flags;
                    vmhd.graphics_mode = readU16();
                    readU16Array(vmhd.opcolor, sizeof(vmhd.opcolor) / sizeof(u16));
                }
                    break;
                case GF_ISOM_BOX_TYPE_DINF:
                {
                    memcpy(&dinf, &box, sizeof(box));
                }
                    break;
                case GF_ISOM_BOX_TYPE_DREF:
                {
                    memcpy(&dref, &box, sizeof(box));
                    dref.version = version;
                    dref.flags = flags;
                    dref.entry_count = readU32();
                    dref.list = (DataReferenceEntryBox**)malloc(sizeof(DataReferenceEntryBox*) * dref.entry_count);
                    for (auto i = 0; i < dref.entry_count; i++) {
                        auto savedRangStart = rangeStart;
                        auto entrySize = readU32();
                        auto entryType = readU32();
                        auto entryVersion = readNextByte();
                        auto entryFlags = readU24();
                        switch (entryType) {
                            case GF_ISOM_ENTRY_TYPE_URL:
                            case GF_ISOM_ENTRY_TYPE_ALIS:
                            {
                                auto tempEntry = (DataReferenceEntryBox*)malloc(sizeof(DataReferenceEntryBox));
                                tempEntry->size = entrySize;
                                tempEntry->type = entryType;
                                tempEntry->version = entryVersion;
                                tempEntry->flags = entryFlags;
                                auto locationLength = (entrySize - ((rangeStart + BUF_LEN - savedRangStart) % BUF_LEN));
                                if (locationLength > 0) {
                                    tempEntry->location = (char *)malloc(sizeof(char) * locationLength);
                                    readU8Array((u8*)tempEntry->location, locationLength);
                                } else {
                                    tempEntry->location = nullptr;
                                }
                                
                                dref.list[i] = tempEntry;
                            }
                                break;
                            case GF_ISOM_ENTRY_TYPE_RSRC:
                            {
                                cout << "warning: deprecated rsrc data reference entry" << endl;
                                
                            }
                            default:
                                break;
                        }
                    }
                }
                    break;
                case GF_ISOM_BOX_TYPE_STBL:
                {
                    memcpy(&stbl, &box, sizeof(box));
                }
                    break;
                case GF_ISOM_BOX_TYPE_STSD:
                {
                    memcpy(&stsd, &box, sizeof(box));
                    stsd.version = version;
                    stsd.flags = flags;
                    stsd.entry_count = readU32();
                    stsd.list = (SampleDescriptionEntryBox**)malloc(sizeof(SampleDescriptionEntryBox*) * stsd.entry_count);
                    for (auto i = 0; i < stsd.entry_count; i++) {
                        struct SampleDescriptionEntryBox *descriptionEntry = (struct SampleDescriptionEntryBox*)malloc(sizeof(struct SampleDescriptionEntryBox));
                        descriptionEntry->size = readU32();
                        descriptionEntry->type = readU32();
                        readU8Array((u8*)descriptionEntry->reserved, sizeof(descriptionEntry->reserved) / sizeof(u8));
                        descriptionEntry->data_reference_index = readU16();
                        switch (descriptionEntry->type) {
                            case GF_ISOM_BOX_TYPE_AVC1:
                            {
                                auto videoDescriptionEntry = (struct VideoSampleDescriptionEntryBox*)realloc(descriptionEntry, sizeof(VideoSampleDescriptionEntryBox));
                                videoDescriptionEntry->version = readU16();
                                videoDescriptionEntry->revision_level = readU16();
                                videoDescriptionEntry->vendor = readU32();
                                videoDescriptionEntry->temporal_quality = readU32();
                                videoDescriptionEntry->spatial_quality = readU32();
                                videoDescriptionEntry->width = readU16();
                                videoDescriptionEntry->height = readU16();
                                videoDescriptionEntry->horizontal_resolution = readU32();
                                videoDescriptionEntry->vertical_resolution = readU32();
                                videoDescriptionEntry->data_size = readU32();
                                videoDescriptionEntry->frame_count = readU16();
                                readU32Array(videoDescriptionEntry->compressor_name, sizeof(videoDescriptionEntry->compressor_name) / sizeof(u32));
                                videoDescriptionEntry->depth = readU16();
                                videoDescriptionEntry->color_table_id = readU16();
                                stsd.list[i] = (SampleDescriptionEntryBox*)videoDescriptionEntry;
                                
                                // avc1 box必然有一个avcC box
                                auto tempRangStart = rangeStart;
                                struct AvccDecoderConfigurationBox avcc;
                                avcc.size = readU32();
                                avcc.type = readU32();
                                cout << "warn: skip box ";
                                printU32WithStr(avcc.type);
                                cout << endl;
                                skip((u32)avcc.size - ((rangeStart + BUF_LEN - tempRangStart) % BUF_LEN));
                            }
                                break;
                                
                            default:
                                break;
                        }
                    }
                }
                    break;
                case GF_ISOM_BOX_TYPE_STTS:
                {
                    memcpy(&stts, &box, sizeof(box));
                    stts.version = version;
                    stts.flags = flags;
                    stts.entry_count = readU32();
                    stts.list = (struct TimeToSampleEntryBox*)malloc(sizeof(struct TimeToSampleEntryBox) * stts.entry_count);
                    for (auto i = 0; i < stts.entry_count; i++) {
                        stts.list[i].sample_count = readU32();
                        stts.list[i].sample_delta = readU32();
                    }
                }
                    break;
                case GF_ISOM_BOX_TYPE_STSS:
                {
                    memcpy(&stss, &box, sizeof(box));
                    stss.version = version;
                    stss.flags = flags;
                    stss.entry_count = readU32();
                    stss.sample_number = (u32*)malloc(sizeof(u32) * stss.entry_count);
                    for (auto i = 0; i < stss.entry_count; i++) {
                        stss.sample_number[i] = readU32();
                    }
                }
                    break;
                case GF_ISOM_BOX_TYPE_STSC:
                {
                    memcpy(&stsc, &box, sizeof(box));
                    stsc.version = version;
                    stsc.flags = flags;
                    stsc.entry_count = readU32();
                    stsc.list = (struct SampleToChunkEntryBox*)malloc(sizeof(struct SampleToChunkEntryBox) * stsc.entry_count);
                    for (auto i = 0; i < stsc.entry_count; i++) {
                        stsc.list[i].first_chunk = readU32();
                        stsc.list[i].sample_per_chunk = readU32();
                        stsc.list[i].sample_description_index = readU32();
                    }
                }
                    break;
                case GF_ISOM_BOX_TYPE_STSZ:
                {
                    memcpy(&stsz, &box, sizeof(box));
                    stsz.version = version;
                    stsz.flags = flags;
                    stsz.sample_size = readU32();
                    stsz.sample_count = readU32();
                    stsz.sample_size_list = nullptr;
                    if (stsz.sample_size == 0) {
                        stsz.sample_size_list = (u32*)malloc(sizeof(u32) * stsz.sample_count);
                        for (auto i = 0; i < stsz.sample_count; i++) {
                            stsz.sample_size_list[i] = readU32();
                        }
                    }
                }
                    break;
                case GF_ISOM_BOX_TYPE_STCO:
                {
                    memcpy(&stco, &box, sizeof(box));
                    stco.version = version;
                    stco.flags = flags;
                    stco.entry_count = readU32();
                    stco.chunk_offset_list = (u32*)malloc(sizeof(u32) * stco.entry_count);
                    for (auto i = 0; i < stco.entry_count; i++) {
                        stco.chunk_offset_list[i] = readU32();
                    }
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
