//
//  enum_def.h
//  mp4-parser
//
//  Created by HJ on 2019/12/10.
//  Copyright © 2019 HJ. All rights reserved.
//

#ifndef enum_def_h
#define enum_def_h


#define GF_4CC(a,b,c,d) (((a)<<24)|((b)<<16)|((c)<<8)|(d))

enum {
    GF_ISOM_BOX_TYPE_FTYP    = GF_4CC( 'f', 't', 'y', 'p' ),
    GF_ISOM_BOX_TYPE_FREE    = GF_4CC( 'f', 'r', 'e', 'e' ),
    GF_ISOM_BOX_TYPE_MDAT    = GF_4CC( 'm', 'd', 'a', 't' ),
    GF_ISOM_BOX_TYPE_MOOV    = GF_4CC( 'm', 'o', 'o', 'v' ),
    GF_ISOM_BOX_TYPE_MVHD    = GF_4CC( 'm', 'v', 'h', 'd' ),
    GF_ISOM_BOX_TYPE_TRAK    = GF_4CC( 't', 'r', 'a', 'k' ),
    GF_ISOM_BOX_TYPE_TKHD    = GF_4CC( 't', 'k', 'h', 'd' ),
    GF_ISOM_BOX_TYPE_EDTS    = GF_4CC( 'e', 'd', 't', 's' ),
    GF_ISOM_BOX_TYPE_ELST    = GF_4CC( 'e', 'l', 's', 't' ),
    GF_ISOM_BOX_TYPE_MDIA    = GF_4CC( 'm', 'd', 'i', 'a' ),
    GF_ISOM_BOX_TYPE_MDHD    = GF_4CC( 'm', 'd', 'h', 'd' ),
    GF_ISOM_BOX_TYPE_HDLR    = GF_4CC( 'h', 'd', 'l', 'r' ),
    GF_ISOM_BOX_TYPE_MINF    = GF_4CC( 'm', 'i', 'n', 'f' ),
    GF_ISOM_BOX_TYPE_VMHD    = GF_4CC( 'v', 'm', 'h', 'd' ),
    GF_ISOM_BOX_TYPE_SMHD    = GF_4CC( 's', 'm', 'h', 'd' ),
    GF_ISOM_BOX_TYPE_HMHD    = GF_4CC( 'h', 'm', 'h', 'd' ),
    GF_ISOM_BOX_TYPE_DINF    = GF_4CC( 'd', 'i', 'n', 'f' ),
    GF_ISOM_BOX_TYPE_DREF    = GF_4CC( 'd', 'r', 'e', 'f' ),
    GF_ISOM_BOX_TYPE_STBL    = GF_4CC( 's', 't', 'b', 'l' ),
    GF_ISOM_BOX_TYPE_STSD    = GF_4CC( 's', 't', 's', 'd' ),
    GF_ISOM_BOX_TYPE_AVC1    = GF_4CC( 'a', 'v', 'c', '1' ),
    GF_ISOM_BOX_TYPE_STTS    = GF_4CC( 's', 't', 't', 's' ),
    GF_ISOM_BOX_TYPE_STSS    = GF_4CC( 's', 't', 's', 's' ),
    GF_ISOM_BOX_TYPE_STSC    = GF_4CC( 's', 't', 's', 'c' ),
    GF_ISOM_BOX_TYPE_STSZ    = GF_4CC( 's', 't', 's', 'z' ),
    GF_ISOM_BOX_TYPE_STZ2    = GF_4CC( 's', 't', 'z', '2' ), // TODO: unknown
    GF_ISOM_BOX_TYPE_CO64    = GF_4CC( 'c', 'o', '6', '4' ), // TODO: unknown
    GF_ISOM_BOX_TYPE_STCO    = GF_4CC( 's', 't', 'c', 'o' ),
    GF_ISOM_BOX_TYPE_SGPD    = GF_4CC( 's', 'g', 'p', 'd' ),
    GF_ISOM_BOX_TYPE_SBGP    = GF_4CC( 's', 'b', 'g', 'p' ),
    GF_ISOM_BOX_TYPE_UDTA    = GF_4CC( 'u', 'd', 't', 'a' ),
};

// video sample description extensions
enum {
    GF_ISOM_BOX_TYPE_GAMA    = GF_4CC( 'g', 'a', 'm', 'a' ),
    GF_ISOM_BOX_TYPE_FIEL    = GF_4CC( 'f', 'i', 'e', 'l' ),
    GF_ISOM_BOX_TYPE_MJQT    = GF_4CC( 'm', 'j', 'q', 't' ),
    GF_ISOM_BOX_TYPE_MJHT    = GF_4CC( 'm', 'j', 'h', 't' ),
    GF_ISOM_BOX_TYPE_AVCC    = GF_4CC( 'a', 'v', 'c', 'C' ),
    GF_ISOM_BOX_TYPE_PASP    = GF_4CC( 'p', 'a', 's', 'p' ),
    GF_ISOM_BOX_TYPE_COLR    = GF_4CC( 'c', 'o', 'l', 'r' ),
    GF_ISOM_BOX_TYPE_CLAP    = GF_4CC( 'c', 'l', 'a', 'p' ),
};

// supported QuickTime audio formats
enum {
    GF_ISOM_BOX_TYPE_MP4A    = GF_4CC( 'm', 'p', '4', 'a' ),
    GF_ISOM_BOX_TYPE_ESDS    = GF_4CC( 'e', 's', 'd', 's' ),
};

enum {
    GF_ISOM_ENTRY_TYPE_ALIS  = GF_4CC( 'a', 'l', 'i', 's' ),
    GF_ISOM_ENTRY_TYPE_RSRC  = GF_4CC( 'r', 's', 'r', 'c' ),
    GF_ISOM_ENTRY_TYPE_URL   = GF_4CC( 'u', 'r', 'l', ' ' ),
};


#endif /* enum_def_h */
