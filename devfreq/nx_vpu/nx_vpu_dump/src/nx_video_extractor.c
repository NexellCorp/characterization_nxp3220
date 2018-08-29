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

#ifdef __cplusplus
#define __STDC_CONSTANT_MACROS
#ifdef _STDINT_H
#undef _STDINT_H
#endif
#include <stdint.h>
#endif

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/pixdesc.h>
#include <libavdevice/avdevice.h>

#include <linux/videodev2.h>
#include <linux/videodev2_nxp_media.h>

#include "nx_video_extractor.h"

#ifndef INT64_MIN
#define INT64_MIN		0x8000000000000000LL
#endif

#ifndef INT64_MAX
#define INT64_MAX		0x7fffffffffffffffLL
#endif

#ifndef MKTAG
#define MKTAG(a,b,c,d) (a | (b << 8) | (c << 16) | (d << 24))
#endif

#define	NX_MAX_NUM_SPS		3
#define	NX_MAX_SPS_SIZE		1024
#define	NX_MAX_NUM_PPS		3
#define	NX_MAX_PPS_SIZE		1024

#define	NX_MAX_NUM_CFG		6
#define	NX_MAX_CFG_SIZE		1024

typedef struct {
	int				version;
	int				profile_indication;
	int				compatible_profile;
	int				level_indication;
	int				nal_length_size;
	int				num_sps;
	int				sps_length[NX_MAX_NUM_SPS];
	unsigned char	sps_data[NX_MAX_NUM_SPS][NX_MAX_SPS_SIZE];
	int				num_pps;
	int				pps_length[NX_MAX_NUM_PPS];
	unsigned char	pps_data[NX_MAX_NUM_PPS][NX_MAX_PPS_SIZE];
} NX_AVCC_TYPE;

typedef struct {
	int				profile;
	int				level;
	int				nal_length_size;
	int				num_cfg;
	int				cfg_length[NX_MAX_NUM_CFG];
	unsigned char	cfg_data[NX_MAX_NUM_CFG][NX_MAX_CFG_SIZE];
} NX_HVC1_TYPE;

typedef struct NX_VIDEO_INFO {
	AVFormatContext*	fmt_ctx;
	AVStream*			stream;
	AVCodec*			codec;
	int32_t				index;
	int32_t				nal_length_size;
} NX_VIDEO_INFO;

//------------------------------------------------------------------------------
static int32_t h264_make_sequence( unsigned char *extradata, int32_t extrasize, NX_AVCC_TYPE *info )
{
	int32_t pos = 0;
	int32_t i;
	int32_t length;

	if( 1 != extradata[0] || 11 > extrasize )
	{
		printf("fail, invalid \"avcC\" data\n");
		return -1;
	}

	// Parse "avcC" format data
	info->version            = (int32_t)extradata[pos];				pos++;
	info->profile_indication = (int32_t)extradata[pos];				pos++;
	info->compatible_profile = (int32_t)extradata[pos];				pos++;
	info->level_indication   = (int32_t)extradata[pos];				pos++;
	info->nal_length_size    = (int32_t)(extradata[pos]&0x03)+1;	pos++;

	// parser sps
	info->num_sps            = (int32_t)(extradata[pos]&0x1f);		pos++;
	for( i=0 ; i<info->num_sps ; i++)
	{
		length = info->sps_length[i] = (int32_t)(extradata[pos]<<8)|extradata[pos+1];
		pos += 2;
		if( (pos+length) > extrasize )
		{
			printf("fail, extradata size too small. (SPS)\n" );
			return -1;
		}
		memcpy( info->sps_data[i], extradata+pos, length );
		pos += length;
	}

	// parse pps
	info->num_pps            = (int32_t)extradata[pos];				pos++;
	for( i=0 ; i<info->num_pps ; i++ )
	{
		length = info->pps_length[i] = (int32_t)(extradata[pos]<<8)|extradata[pos+1];
		pos += 2;
		if( (pos+length) > extrasize )
		{
			printf( "fail, extradata size too small. (PPS)\n");
			return -1;
		}
		memcpy( info->pps_data[i], extradata+pos, length );
		pos += length;
	}

	return 0;
}

//------------------------------------------------------------------------------
static int32_t h264_make_stream( NX_AVCC_TYPE *avcc, unsigned char *buf, int32_t *size )
{
	int32_t i;
	int32_t pos = 0;

	for (i=0 ; i<avcc->num_sps ; i++)
	{
		buf[pos++] = 0x00;
		buf[pos++] = 0x00;
		buf[pos++] = 0x00;
		buf[pos++] = 0x01;
		memcpy( buf + pos, avcc->sps_data[i], avcc->sps_length[i] );
		pos += avcc->sps_length[i];
	}

	for (i=0 ; i<avcc->num_pps ; i++)
	{
		buf[pos++] = 0x00;
		buf[pos++] = 0x00;
		buf[pos++] = 0x00;
		buf[pos++] = 0x01;
		memcpy( buf + pos, avcc->pps_data[i], avcc->pps_length[i] );
		pos += avcc->pps_length[i];
	}
	*size = pos;
}

//------------------------------------------------------------------------------
static int32_t h264_parse_stream( AVPacket *packet, int32_t nal_length_size, unsigned char *outbuf, int32_t *outsize )
{
	int32_t nal_length;

	//	input
	unsigned char *inbuf = packet->data;
	int32_t insize = packet->size;
	int32_t pos = 0;

	//	'avcC' format
	do{
		nal_length = 0;

		if( nal_length_size == 2 )
		{
			nal_length = inbuf[0]<< 8 | inbuf[1];
		}
		else if( nal_length_size == 3 )
		{
			nal_length = inbuf[0]<<16 | inbuf[1]<<8  | inbuf[2];
		}
		else if( nal_length_size == 4 )
		{
			nal_length = inbuf[0]<<24 | inbuf[1]<<16 | inbuf[2]<<8 | inbuf[3];
		}
		else if( nal_length_size == 1 )
		{
			nal_length = inbuf[0];
		}

		inbuf  += nal_length_size;
		insize -= nal_length_size;

		if( 0==nal_length || insize<(int)nal_length )
		{
			printf("fail, avcC type nal length error (nal_length = %d, insize=%d, nal_length_size=%d)\n",
				nal_length, insize, nal_length_size);
			return -1;
		}

		//	put nal start code
		outbuf[pos + 0] = 0x00;
		outbuf[pos + 1] = 0x00;
		outbuf[pos + 2] = 0x00;
		outbuf[pos + 3] = 0x01;
		pos += 4;

		memcpy( outbuf + pos, inbuf, nal_length );
		pos += nal_length;

		insize -= nal_length;
		inbuf  += nal_length;
	}while( 2<insize );

	*outsize = pos;
	return 0;
}

//------------------------------------------------------------------------------
NX_VIDEO_HANDLE nx_video_open( const char* file )
{
	NX_VIDEO_HANDLE handle = NULL;
	AVFormatContext *fmt_ctx = NULL;
	AVInputFormat *fmt = NULL;
	AVStream *stream = NULL;
	AVCodec *codec = NULL;
	int32_t index = -1;
	int32_t nal_length_size=0;

	int32_t i;

	av_register_all();

	fmt_ctx = avformat_alloc_context();
	fmt_ctx->flags |= CODEC_FLAG_TRUNCATED;

	if( 0 > avformat_open_input(&fmt_ctx, file, fmt, NULL) )
	{
		printf("fail, avformat_open_input().\n");
		goto ERROR;
	}

	if( 0 > avformat_find_stream_info(fmt_ctx, NULL) )
	{
		printf("fail, avformat_find_stream_info().\n");
		goto ERROR;
	}

	av_dump_format( fmt_ctx, 0, file, 0 );

	for( i = 0; i < (int32_t)fmt_ctx->nb_streams; i++ )
	{
		AVStream *strm = fmt_ctx->streams[i];
		if( strm->codec->codec_type == AVMEDIA_TYPE_VIDEO )
		{
			if( !(codec=avcodec_find_decoder(strm->codec->codec_id)) )
			{
				printf("Unsupported codec (id=%d) for input stream %d\n", strm->codec->codec_id, strm->index);
				goto ERROR;
			}

			if( 0 > avcodec_open2(strm->codec, codec, NULL)<0 )
			{
				printf( "Error while opening codec for input stream %d\n", strm->index );
				goto ERROR;
			}
			else
			{
				if( index == -1 )
				{
					index = i;
					stream = strm;;
				}
				else
				{
					avcodec_close( strm->codec );
				}
			}
		}
	}

	if ((stream->codec->codec_id == AV_CODEC_ID_H264) && (stream->codec->extradata_size > 0))
	{
		if (stream->codec->extradata[0] = 0x01)
		{
			NX_AVCC_TYPE info;
			h264_make_sequence(stream->codec->extradata, stream->codec->extradata_size, &info);
			nal_length_size = info.nal_length_size;
		}
	}

	handle = (NX_VIDEO_HANDLE)malloc(sizeof(NX_VIDEO_INFO));
	memset( handle, 0x00, sizeof(NX_VIDEO_INFO) );
	handle->fmt_ctx         = fmt_ctx;
	handle->stream          = stream;
	handle->codec           = codec;
	handle->index           = index;
	handle->nal_length_size = nal_length_size;

	return handle;

ERROR:
	if( fmt_ctx )
	{
		avformat_close_input( &fmt_ctx );
	}

	return NULL;
}

//------------------------------------------------------------------------------
void nx_video_close( NX_VIDEO_HANDLE handle )
{
	if( handle && handle->fmt_ctx )
	{
		if( handle->fmt_ctx ) avformat_close_input( &handle->fmt_ctx );
		free( handle );
		handle = NULL;
	}
}

//------------------------------------------------------------------------------
int32_t nx_video_read( NX_VIDEO_HANDLE handle, uint8_t *buf, int32_t *size, int32_t *key, int64_t *timestamp )
{
	AVPacket packet;
	AVFormatContext *fmt_ctx;
	AVStream *stream;
	int32_t nal_length_size=0;
	enum AVCodecID codec_id;
	double timestamp_ratio;

	if( !handle )
	{
		return -1;
	}

	fmt_ctx         = handle->fmt_ctx;
	stream          = handle->stream;
	codec_id        = handle->stream->codec->codec_id;
	nal_length_size = handle->nal_length_size;
	timestamp_ratio = (double)handle->stream->time_base.num*1000./(double)handle->stream->time_base.den;

	do {
		if( 0 > av_read_frame( fmt_ctx, &packet ) )
			return -1;

		if( packet.stream_index == stream->index )
		{
			if( codec_id == AV_CODEC_ID_H264 && stream->codec->extradata_size > 0 && stream->codec->extradata[0] == 0x01 )
			{
				if( 0 > h264_parse_stream( &packet, nal_length_size, buf, size ) )
				{
					printf("fail, h264_parse_stream().\n");
					return -1;
				}

				if (key)		*key = (packet.flags & AV_PKT_FLAG_KEY) ? 1 : 0;
				if (timestamp)	*timestamp = (packet.pts != AV_NOPTS_VALUE ) ? packet.pts*timestamp_ratio : -1;

				av_free_packet( &packet );
				return 0;
			}
			else
			{
				memcpy(buf, packet.data, packet.size );

				if (size)		*size = packet.size;
				if (key)		*key = (packet.flags & AV_PKT_FLAG_KEY) ? 1 : 0;
				if (timestamp)	*timestamp = (packet.pts != AV_NOPTS_VALUE ) ? packet.pts*timestamp_ratio : -1;

				av_free_packet( &packet );
				return 0;
			}
		}
	} while(1);

	return -1;
}

//------------------------------------------------------------------------------
int32_t nx_video_seek( NX_VIDEO_HANDLE handle, int64_t seektime )
{
	int32_t ret;
	int32_t flags = AVSEEK_FLAG_FRAME;

	flags &= ~AVSEEK_FLAG_BYTE;

	if( !handle && !handle->fmt_ctx )
		return -1;

	ret = avformat_seek_file( handle->fmt_ctx, -1, INT64_MIN, seektime * 1000, INT64_MAX, flags );
	if( 0 > ret )
	{
		printf("fail, avformat_seek_file().\n");
	}

	return ret;
}

//------------------------------------------------------------------------------
int32_t nx_video_get_seqinfo( NX_VIDEO_HANDLE handle, uint8_t* buf, int32_t *size )
{
	AVStream *stream;
	if( !handle )
		return -1;

	stream = handle->stream;

	if( (stream->codec->codec_id == AV_CODEC_ID_H264) && (stream->codec->extradata_size > 0) )
	{
		if( stream->codec->extradata[0] == 0x01 )
		{
			NX_AVCC_TYPE info;
			h264_make_sequence( stream->codec->extradata, stream->codec->extradata_size, &info );
			h264_make_stream( &info, buf, size );
			return 0;
		}
	}

	memcpy( buf, stream->codec->extradata, stream->codec->extradata_size );
	*size = stream->codec->extradata_size;
	return 0;
}

//------------------------------------------------------------------------------
int32_t nx_video_get_format( NX_VIDEO_HANDLE handle, uint32_t *format )
{
	uint32_t fmt = -1;
	if( !handle )
		return -1;

	switch( handle->stream->codec->codec_id )
	{
	case AV_CODEC_ID_H264:				fmt = V4L2_PIX_FMT_H264;	break;
	case AV_CODEC_ID_MPEG2VIDEO:		fmt = V4L2_PIX_FMT_MPEG2;	break;
	case AV_CODEC_ID_MPEG4:
	case AV_CODEC_ID_MSMPEG4V3:
		switch( handle->stream->codec->codec_tag )
		{
		case MKTAG('D','I','V','3'):
		case MKTAG('M','P','4','3'):
		case MKTAG('M','P','G','3'):
		case MKTAG('D','V','X','3'):
		case MKTAG('A','P','4','1'):	fmt = V4L2_PIX_FMT_DIV3;	break;
		case MKTAG('X','V','I','D'):	fmt = V4L2_PIX_FMT_XVID;	break;
		case MKTAG('D','I','V','X'):	fmt = V4L2_PIX_FMT_DIVX;	break;
		case MKTAG('D','I','V','4'):	fmt = V4L2_PIX_FMT_DIV4;	break;
		case MKTAG('D','X','5','0'):
		case MKTAG('D','I','V','5'):	fmt = V4L2_PIX_FMT_DIV5;	break;
		case MKTAG('D','I','V','6'):	fmt = V4L2_PIX_FMT_DIV6;	break;
		default:						fmt = V4L2_PIX_FMT_MPEG4;	break;
		}
	case AV_CODEC_ID_H263:
	case AV_CODEC_ID_H263P:
	case AV_CODEC_ID_H263I:				fmt = V4L2_PIX_FMT_H263;	break;

	case AV_CODEC_ID_WMV3:				fmt = V4L2_PIX_FMT_WMV9;	break;
	case AV_CODEC_ID_VC1:				fmt = V4L2_PIX_FMT_WVC1;	break;
	case AV_CODEC_ID_RV30:				fmt = V4L2_PIX_FMT_RV8;		break;
	case AV_CODEC_ID_RV40:				fmt = V4L2_PIX_FMT_RV9;		break;
	case AV_CODEC_ID_VP8:				fmt = V4L2_PIX_FMT_VP8;		break;
	case AV_CODEC_ID_FLV1:				fmt = V4L2_PIX_FMT_FLV1;	break;
	case AV_CODEC_ID_THEORA:			fmt = V4L2_PIX_FMT_THEORA;	break;
	case AV_CODEC_ID_MJPEG:				fmt = V4L2_PIX_FMT_MJPEG;	break;
	default:
		printf("Cannot support codec( %d, 0x%08X )\n",
			handle->stream->codec->codec_id, handle->stream->codec->codec_tag );
		return -1;
	}

	*format = fmt;
	return 0;
}

//------------------------------------------------------------------------------
int32_t nx_video_get_resolution( NX_VIDEO_HANDLE handle, int32_t *width, int32_t *height )
{
	if( handle && handle->stream )
	{
		*width  = handle->stream->codec->width;
		*height = handle->stream->codec->height;

		if( handle->stream->codec->coded_width != handle->stream->codec->width ||
			handle->stream->codec->coded_height != handle->stream->codec->height )
		{
			printf("resolution( %d x %d ), coded resolution( %d x %d )\n",
				handle->stream->codec->width,
				handle->stream->codec->height,
				handle->stream->codec->coded_width,
				handle->stream->codec->coded_height
			);
		}

		return 0;
	}

	return -1;
}

//------------------------------------------------------------------------------
int32_t nx_video_get_duration( NX_VIDEO_HANDLE handle, int64_t *duration )
{
	if( handle && handle->fmt_ctx )
	{
		*duration = handle->fmt_ctx->duration;
		return 0;
	}

	return -1;
}
