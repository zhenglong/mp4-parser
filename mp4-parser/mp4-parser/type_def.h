//
//  type_def.h
//  mp4-parser
//
//  Created by HJ on 2019/12/10.
//  Copyright © 2019 HJ. All rights reserved.
//

#ifndef type_def_h
#define type_def_h

#include <cstdlib>

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;

#define GF_ISOM_BOX            \
u32 type;            \
u64 size;


#define GF_ISOM_FULL_BOX        \
GF_ISOM_BOX            \
u8 version;            \
u32 flags;            \

#define GF_ISOM_DATAENTRY_FIELDS    \
char *location;


#define GF_ISOM_SAMPLE_ENTRY_FIELDS        \
GF_ISOM_BOX                              \
char reserved[ 6 ];                      \
u16 data_reference_index;                \

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

struct TrackBox {
    GF_ISOM_BOX
};

struct TrackHeaderBox {
    GF_ISOM_FULL_BOX
    
    u64 creation_time;
    u64 modification_time;
    u32 track_id;
    u32 reserved;
    u64 duration;
    
    u32 reserved1[2];
    u16 layer;
    u16 alternate_group;
    u16 volume;
    u16 reserved2;
    u32 matrix[9];
    u32 width;
    u32 height;
};

struct EditListEntryBox {
    u64 segment_duration;
    u64 media_time;
    u16 media_rate_interger;
    u16 media_rate_fraction;
};

// 容器Box
struct EditBox {
    GF_ISOM_BOX
};

struct EditListBox {
    GF_ISOM_FULL_BOX
    
    u32 entry_count;
    struct EditListEntryBox *list;
};

struct MediaBox {
    GF_ISOM_BOX
};

struct MediaHeaderBox {
    GF_ISOM_FULL_BOX
    u64 creation_time;
    u64 modification_time;
    u32 time_scale;
    u64 duration;
    u16 language;
    u16 quality;
};

struct HandlerBox {
    GF_ISOM_FULL_BOX
    u32 pre_defined;
    u32 handler_type;
    u32 reserved[3];
    u32 name_length;
    char *name;
};

// container box
struct MediaInformationBox {
    GF_ISOM_BOX
};

struct VideoMediaHeaderBox {
    GF_ISOM_FULL_BOX
    u16 graphics_mode;
    u16 opcolor[3];
};

struct SoundMediaHeaderBox {
    GF_ISOM_FULL_BOX
    u16 balance;
    u16 reserved;
};

struct HintMediaHeaderBox {
    GF_ISOM_FULL_BOX
    u16 max_pdu_size;
    u16 avg_pdu_size;
    u32 max_bitrate;
    u32 avg_bitrate;
    u32 reserved;
};

struct DataInformationBox {
    GF_ISOM_BOX
};

struct DataReferenceEntryBox {
    GF_ISOM_FULL_BOX
    GF_ISOM_DATAENTRY_FIELDS
};

struct DataReferenceUrlEntryBox {
    GF_ISOM_FULL_BOX
    GF_ISOM_DATAENTRY_FIELDS
};

struct DataReferenceAliasEntryBox {
    GF_ISOM_FULL_BOX
    GF_ISOM_DATAENTRY_FIELDS
};

//  deprecated
struct DataReferenceUrnEntryBox {
    GF_ISOM_FULL_BOX
    GF_ISOM_DATAENTRY_FIELDS
    char *name_urn;
};

struct DataReferenceBox {
    GF_ISOM_FULL_BOX
    u32 entry_count;
    struct DataReferenceEntryBox **list;
};

struct SampleTableBox {
    GF_ISOM_BOX
};

struct SampleDescriptionEntryBox {
    GF_ISOM_SAMPLE_ENTRY_FIELDS
};

struct VideoSampleDescriptionEntryBox {
    GF_ISOM_SAMPLE_ENTRY_FIELDS
    u16 version;
    u16 revision_level;
    u32 vendor;
    u32 temporal_quality;
    u32 spatial_quality;
    u16 width;
    u16 height;
    u32 horizontal_resolution;
    u32 vertical_resolution;
    u32 data_size;
    u16 frame_count;
    u32 compressor_name[8];
    u16 depth;
    u16 color_table_id;
};

// has three versions
struct MpegAudioSampleDescriptionEntryBox {
    GF_ISOM_SAMPLE_ENTRY_FIELDS
    u16 version;
    u16 revision;
    u32 vendor;
    u16 channel_count;
    u16 bits_per_sample;
    u16 compression_id;
    u16 packet_size;
    u16 sample_rate_hi;
    u16 sample_rate_lo;
    u8 extensions[36];
};

struct AvcDecoderConfigurationBox {
    GF_ISOM_BOX
    
};

struct AvccDecoderConfigurationBox {
    GF_ISOM_BOX
    // TODO:
};

struct SampleDescriptionBox {
    GF_ISOM_FULL_BOX
    u32 entry_count;
    struct SampleDescriptionEntryBox **list;
};

struct TimeToSampleEntryBox {
    u32 sample_count;
    u32 sample_delta;
};

struct TimeToSampleBox {
    GF_ISOM_FULL_BOX
    u32 entry_count;
    TimeToSampleEntryBox *list;
};

struct SyncSampleBox {
    GF_ISOM_FULL_BOX
    u32 entry_count;
    u32 *sample_number;
};

struct SampleToChunkEntryBox {
    u32 first_chunk;
    u32 sample_per_chunk;
    u32 sample_description_index;
};

struct SampleToChunkBox {
    GF_ISOM_FULL_BOX
    u32 entry_count;
    struct SampleToChunkEntryBox *list;
};

struct SampleSizeBox {
    GF_ISOM_FULL_BOX
    u32 sample_size;
    u32 sample_count;
    u32 *sample_size_list;
};

struct ChunkOffsetBox {
    GF_ISOM_FULL_BOX
    u32 entry_count;
    u32 *chunk_offset_list;
};


#endif /* type_def_h */
