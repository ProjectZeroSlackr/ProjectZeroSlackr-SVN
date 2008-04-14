/* /////////////////////////////// "ippdefs.h" /////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 1999 Intel Corporation. All Rights Reserved.
//
//
//      Intel(R) Integrated Performance Primitives Common Types and Macro Definitions.
//                               Version 1.1
//
//  Created: 11-Jun-1999 19:18
//
*/
#ifndef __IPPDEFS_H__
#define __IPPDEFS_H__

#ifdef __cplusplus
extern "C" {
#endif


#if defined( _WIN32 ) || defined ( _WIN64 )
  #define __STDCALL  __stdcall
  #define __CDECL    __cdecl
  #define __INT64    __int64
  #define __UINT64    unsigned __int64
#else
  #define __STDCALL
  #define __CDECL
  #define __INT64    long long
  #define __UINT64    unsigned long long
#endif


#if !defined( IPPAPI )

  #if defined( _IPP_W32DLL ) && (defined( _WIN32 ) || defined( _WIN64 ))
    #if defined( __MSC_VER__ ) || defined( __ICL )
      #define IPPAPI( type,name,arg ) \
                     extern __declspec(dllimport) type __STDCALL name arg;
    #else
      #define IPPAPI( type,name,arg )        extern type __STDCALL name arg;
    #endif
  #else
    #define   IPPAPI( type,name,arg )        extern type __STDCALL name arg;
  #endif

#endif


#define IPP_PI    ( 3.14159265358979323846 )  /* ANSI C does not support M_PI */
#define IPP_2PI   ( 6.28318530717958647692 )  /* 2*pi                         */
#define IPP_PI2   ( 1.57079632679489661923 )  /* pi/2                         */
#define IPP_PI4   ( 0.78539816339744830961 )  /* pi/4                         */
#define IPP_PI180 ( 0.01745329251994329577 )  /* pi/180                       */
#define IPP_RPI   ( 0.31830988618379067154 )  /* 1/pi                         */
#define IPP_SQRT2 ( 1.41421356237309504880 )  /* sqrt(2)                      */
#define IPP_SQRT3 ( 1.73205080756887729353 )  /* sqrt(3)                      */
#define IPP_LN2   ( 0.69314718055994530942 )  /* ln(2)                        */
#define IPP_LN3   ( 1.09861228866810969139 )  /* ln(3)                        */
#define IPP_E     ( 2.71828182845904523536 )  /* e                            */
#define IPP_RE    ( 0.36787944117144232159 )  /* 1/e                          */
#define IPP_EPS23 ( 1.19209289e-07f )
#define IPP_EPS52 ( 2.2204460492503131e-016 )

#define IPP_MAX_8U     ( 0xFF )
#define IPP_MAX_16U    ( 0xFFFF )
#define IPP_MAX_32U    ( 0xFFFFFFFF )
#define IPP_MIN_8S     (-128 )
#define IPP_MAX_8S     ( 127 )
#define IPP_MIN_16S    (-32768 )
#define IPP_MAX_16S    ( 32767 )
#define IPP_MIN_32S    (-2147483647 - 1 )
#define IPP_MAX_32S    ( 2147483647 )

#if defined( _WIN32 ) || defined ( _WIN64 )
  #define IPP_MAX_64S  ( 9223372036854775807i64 )
  #define IPP_MIN_64S  (-9223372036854775807i64 - 1 )
#else
  #define IPP_MAX_64S  ( 9223372036854775807LL )
  #define IPP_MIN_64S  (-9223372036854775807LL - 1 )
#endif

#define IPP_MINABS_32F ( 1.175494351e-38f )
#define IPP_MAXABS_32F ( 3.402823466e+38f )
#define IPP_EPS_32F    ( 1.192092890e-07f )
#define IPP_MINABS_64F ( 2.2250738585072014e-308 )
#define IPP_MAXABS_64F ( 1.7976931348623158e+308 )
#define IPP_EPS_64F    ( 2.2204460492503131e-016 )

#define IPP_DEG_TO_RAD( deg ) ( (deg)/180.0 * IPP_PI )
#define IPP_COUNT_OF( obj )  (sizeof(obj)/sizeof(obj[0]))

#define IPP_MAX( a, b ) ( ((a) > (b)) ? (a) : (b) )
#define IPP_MIN( a, b ) ( ((a) < (b)) ? (a) : (b) )


#if !defined( _OWN_BLDPCS )

typedef struct {
    int    major;                     /* e.g. 1                               */
    int    minor;                     /* e.g. 2                               */
    int    majorBuild;                /* e.g. 3                               */
    int    build;                     /* e.g. 10, always >= majorBuild        */

    char  targetCpu[4];               /* corresponding to Intel(R) processor  */
    const char* Name;                 /* e.g. "ippsm6"                        */
    const char* Version;              /* e.g. "v1.2 Beta"                     */
    const char* BuildDate;            /* e.g. "Jul 20 99"                     */
} IppLibraryVersion;


typedef unsigned char   Ipp8u;
typedef unsigned short  Ipp16u;
typedef unsigned int    Ipp32u;

typedef signed char    Ipp8s;
typedef signed short   Ipp16s;
typedef signed int     Ipp32s;
typedef float   Ipp32f;
typedef __INT64 Ipp64s;
typedef __UINT64 Ipp64u;
typedef double  Ipp64f;

typedef struct {
    Ipp8s  re;
    Ipp8s  im;
} Ipp8sc;

typedef struct {
    Ipp16s  re;
    Ipp16s  im;
} Ipp16sc;

typedef struct {
    Ipp32s  re;
    Ipp32s  im;
} Ipp32sc;

typedef struct {
    Ipp32f  re;
    Ipp32f  im;
} Ipp32fc;

typedef struct {
    Ipp64s  re;
    Ipp64s  im;
} Ipp64sc;

typedef struct {
    Ipp64f  re;
    Ipp64f  im;
} Ipp64fc;

typedef enum {
    ippRndZero,
    ippRndNear
} IppRoundMode;


typedef enum {
    ippAlgHintNone,
    ippAlgHintFast,
    ippAlgHintAccurate
} IppHintAlgorithm;

typedef enum {
    ippCmpLess,
    ippCmpLessEq,
    ippCmpEq,
    ippCmpGreaterEq,
    ippCmpGreater
} IppCmpOp;


enum {
    IPP_FFT_DIV_FWD_BY_N = 1,
    IPP_FFT_DIV_INV_BY_N = 2,
    IPP_FFT_DIV_BY_SQRTN = 4,
    IPP_FFT_NODIV_BY_ANY = 8
};

typedef enum {
   ipp1u,
   ipp8u,  ipp8s,
   ipp16u, ipp16s, ipp16sc,
   ipp32u, ipp32s, ipp32sc,
   ipp32f, ipp32fc,
   ipp64u, ipp64s, ipp64sc,
   ipp64f, ipp64fc
} IppDataType;


typedef struct {
    int x;
    int y;
    int width;
    int height;
} IppiRect;

typedef struct {
    int x;
    int y;
} IppiPoint;

typedef struct {
    int width;
    int height;
} IppiSize;


/* /////////////////////////////////////////////////////////////////////////////
//        The following enumerator defines a status of IPP operations
//                     negative value means error
*/
typedef enum {
     /* errors */
    ippStsNotSupportedModeErr = -9999, 
    ippStsZeroMaskValuesErr, 
    ippStsQuadErr          ,
    ippStsRectErr          ,
    ippStsCoeffErr         ,
    ippStsNoiseValErr      , 
    ippStsDitherLevelsErr  , 
    ippStsNumChannelsErr   , 
    ippStsCOIErr           , 
    ippStsDivisorErr       ,
    ippStsAlphaTypeErr     ,
    ippStsGammaRangeErr    ,
    ippStsGrayCoefSumErr   ,
    ippStsChannelErr       ,

    ippStsToneMagnErr      ,
    ippStsToneFreqErr      ,
    ippStsTonePhaseErr     ,
    ippStsTrnglMagnErr     ,
    ippStsTrnglFreqErr     ,
    ippStsTrnglPhaseErr    ,
    ippStsTrnglAsymErr     ,

    ippStsHugeWinErr       ,
    ippStsJaehneErr        ,
    ippStsStrideErr        ,
    ippStsEpsValErr        ,
    ippStsWtOffsetErr      ,
    ippStsAnchorErr        ,
    ippStsMaskSizeErr      ,

    ippStsShiftErr         ,
    ippStsSampleFactorErr  ,
    ippStsSamplePhaseErr   ,

    ippStsFIRMRFactorErr   ,
    ippStsFIRMRPhaseErr    ,
    ippStsRelFreqErr       ,
    ippStsFIRLenErr        ,
    ippStsIIROrderErr      ,
    ippStsDlyLineIndexErr  ,

    ippStsResizeFactorErr  ,
    ippStsInterpolationErr ,
    ippStsMirrorFlipErr    ,
    ippStsMoment00ZeroErr  ,
    ippStsThreshNegLevelErr,
    ippStsThresholdErr     ,

    ippStsContextMatchErr  ,
    ippStsFftFlagErr       ,
    ippStsFftOrderErr      ,

    ippStsStepErr          ,
    ippStsScaleRangeErr    ,
    ippStsDataTypeErr      ,
    ippStsOutOfRangeErr    ,
    ippStsDivByZeroErr     ,
    ippStsMemAllocErr      ,
    ippStsNullPtrErr       ,
    ippStsRangeErr         ,
    ippStsSizeErr          , 


    ippStsBadArgErr        =   -5,
    ippStsNoMemErr         =   -4,
    ippStsSAReservedErr3   =   -3,
    ippStsErr              =   -2,
    ippStsSAReservedErr1   =   -1,

     /* no errors */
    ippStsNoErr             =   0,

     /* warnings */
    ippStsNoOperation       ,
    ippStsMisalignedBuf     ,
    ippStsSqrtNegArg        ,
    ippStsInvZero           ,
    ippStsEvenMedianMaskSize,
    ippStsDivByZero         ,
    ippStsLnZeroArg         ,
    ippStsLnNegArg          ,
    ippStsNanArg            

} IppStatus;

#define ippStsOk ippStsNoErr 

#endif /* _OWN_BLDPCS */


#ifdef __cplusplus
}
#endif

#endif /* __IPPDEFS_H__ */
/* ///////////////////////// End of file "ippdefs.h" //////////////////////// */
