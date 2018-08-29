//------------------------------------------------------------------------------
//
//  Copyright (C) 2018 Nexell Co. All Rights Reserved
//  Nexell Co. Proprietary & Confidential
//
//  NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//  AND	WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//  BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//  FOR A PARTICULAR PURPOSE.
//
//  Module	:
//  File	:
//  Description	:
//  Author	:
//  Export	:
//  History	:
//
//------------------------------------------------------------------------------

#ifndef __NX_VIDEO_EXTRACTOR_H__
#define __NX_VIDEO_EXTRACTOR_H__

typedef struct NX_VIDEO_INFO* NX_VIDEO_HANDLE;

NX_VIDEO_HANDLE nx_video_open( const char* file );
void nx_video_close( NX_VIDEO_HANDLE handle );

int32_t nx_video_read( NX_VIDEO_HANDLE handle, uint8_t *buf, int32_t *size, int32_t *key, int64_t *timestamp );
int32_t nx_video_seek( NX_VIDEO_HANDLE handle, int64_t seektime );

int32_t nx_video_get_seqinfo( NX_VIDEO_HANDLE handle, uint8_t *buf, int32_t *size );
int32_t nx_video_get_format( NX_VIDEO_HANDLE handle, uint32_t *format );
int32_t nx_video_get_resolution( NX_VIDEO_HANDLE handle, int32_t *width, int32_t *height );
int32_t nx_video_get_duration( NX_VIDEO_HANDLE handle, int64_t *duration );

#endif	// __NX_VIDEO_EXTRACTOR_H__
