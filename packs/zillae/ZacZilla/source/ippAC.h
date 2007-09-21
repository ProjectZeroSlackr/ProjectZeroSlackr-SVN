/******************************************************************************
//
//              INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//        Copyright (c) 2000 Intel Corporation. All Rights Reserved.
//
//
//  Description:
//      Intel(R) Integrated Performance Primitives - Audio Processing header file.
//                             Version 1.1
//
//      This file contains declarations specific to the audio coding domain.
******************************************************************************/
 
#ifndef _IPPAC_H_
#define _IPPAC_H_

#include "ippdefs.h"    

#ifdef __cplusplus
extern "C" {
#endif

/***** Data Structures and Macro ***********************************************/

/* Macro  */

#define IPP_MP3_GRANULE_LEN         576
#define IPP_MP3_SF_BUF_LEN          40  /* scalefactor buffer length */
#define IPP_MP3_V_BUF_LEN           512 /* V data buffer length */

/* Data Structures */
/* MPEG -1, -2 BC header, 32 bits. See ISO/IEC 11172-3, sect 2.4.1.3, 2.4.2.3, 2.4.2.4 */
typedef struct {
    int id;                     /* ID 1: MPEG-1, 0: MPEG-2 */
    int layer;                  /* layer index 0x3: Layer I  
                                //             0x2: Layer II
                                //             0x1: Layer III */
    int protectionBit;          /* CRC flag 0: CRC on, 1: CRC off */
    int bitRate;                /* bit rate index */
    int samplingFreq;           /* sampling frequency index */
    int paddingBit;             /* padding flag 0: no padding, 1 padding  */
    int privateBit;             /* private_bit, no use  */
    int mode;                   /* mono/stereo select information */
    int modeExt;                /* extension to mode */
    int copyright;              /* copyright or not, 0: no, 1: yes  */
    int originalCopy;           /* original bitstream or copy, 0: copy, 1: original */
    int emphasis;               /* flag indicates the type of de-emphasis that shall be used */
    int CRCWord;                /* CRC-check word */

} IppMP3FrameHeader;


/* MP3 side informatin structure , for each granule. Other info main_data_begin, 
// private_bits, scfsi are not included here.
// please refer to reference ISO/IEC 11172-3:1993, 2.4.1.7, 2.4.2.7. ISO/IEC 
// 13818-3:1998, 2.4.1.7 ).
*/
typedef struct {
    int  part23Len;             /* the number of bits for scale factors and Huffman data */ 
    int  bigVals;               /* the half number of Huffman data whose maximum 
                                // amplitudes are greater than 1 
                                */
    int  globGain;              /* the quantizer step size information */
    int  sfCompress;            /* information to select the number of bits used for 
                                // the transmission of the scale factors 
                                */
    int  winSwitch;             /* flag signals that the block uses an other than
                                //   normal window 
                                */
    int  blockType;             /* flag indicates the window type */
    int  mixedBlock;            /* flag 0: non mixed block, 1: mixed block */
    int  pTableSelect[3];       /* Huffman table index for the 3 regions in big-values field */
    int  pSubBlkGain[3];        /* gain offset from the global gain for one subblock */
    int  reg0Cnt;               /* the number of scale factor bands at the boundary 
                                // of the first region of the big-values field 
                                */

    int  reg1Cnt;               /* similar to reg0Cnt, but of the second region */
    int  preFlag;               /* flag of high frequency amplification */
    int  sfScale;               /* scale to the scale factors */
    int  cnt1TabSel;            /* Huffman table index for the count1 region of quadruples */
} IppMP3SideInfo;

/***** Audio Coding Functions **************************************************/

/***** MP3 Functions *****/

/* Frame Unpacking */
IPPAPI(IppStatus, ippsUnpackFrameHeader_MP3, 
       (Ipp8u **ppBitStream, IppMP3FrameHeader *pFrameHeader) )

IPPAPI(IppStatus, ippsUnpackSideInfo_MP3, 
        (Ipp8u **ppBitStream, IppMP3SideInfo *pDstSideInfo, int *pDstMainDataBegin, 
         int *pDstPrivateBits, int *pDstScfsi, IppMP3FrameHeader *pFrameHeader) )

IPPAPI(IppStatus, ippsUnpackScaleFactors_MP3_1u8s, 
       (Ipp8u **ppBitStream, int *pOffset, Ipp8s *pDstScaleFactor, 
        IppMP3SideInfo *pSideInfo, int *pScfsi,   
        IppMP3FrameHeader *pFrameHeader, int granule, int channel) )

/* Huffman Decoding */
IPPAPI(IppStatus, ippsHuffmanDecode_MP3_1u32s, 
       (Ipp8u **ppBitStream, int *pOffset, Ipp32s *pDstIs, int *pDstNonZeroBound, 
        IppMP3SideInfo *pSideInfo, IppMP3FrameHeader *pFrameHeader, int hufSize) )

/* Requantization */
IPPAPI(IppStatus, ippsReQuantize_MP3_32s_I, 
       (Ipp32s *pSrcDstIsXr, int *pNonZeroBound, Ipp8s *pScaleFactor, 
        IppMP3SideInfo *pSideInfo, IppMP3FrameHeader *pFrameHeader, Ipp32s *pBuffer) )

/* Hybrid Filtering */
IPPAPI(IppStatus, ippsMDCTInv_MP3_32s, 
       (Ipp32s *pSrcXr, Ipp32s *pDstY, Ipp32s *pSrcDstOverlapAdd, 
        int nonZeroBound, int *pPrevNumOfImdct, int blockType, int mixedBlock) )

/* Polyphase Filtering */
IPPAPI(IppStatus, ippsSynthPQMF_MP3_32s16s, 
       (Ipp32s *pSrcY, Ipp16s *pDstAudioOut, Ipp32s *pVBuffer, int *pVPosition, int mode) )

#ifdef __cplusplus
}
#endif

#endif /* end of #define _IPPAC_H_ */

/* EOF */

