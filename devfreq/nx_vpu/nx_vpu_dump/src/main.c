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

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <linux/videodev2.h>
#include <linux/videodev2_nxp_media.h>

#include "nx_video_api.h"
#include "nx_video_utils.h"
#include "nx_video_extractor.h"

//------------------------------------------------------------------------------
#define NX_IMAGE_FORMAT				V4L2_PIX_FMT_YUV420

//------------------------------------------------------------------------------
typedef struct NX_APP_INFO {
	char*		in_file;
	char*		out_dir;

	int32_t		start_sec;
	int32_t		end_frame;
} NX_APP_INFO;

//------------------------------------------------------------------------------
static void signal_handler( int32_t sig )
{
	printf("Aborted by signal %s (%d)..\n", (char*)strsignal(sig), sig);

	switch( sig )
	{
		case SIGINT :
			printf("SIGINT..\n"); 	break;
		case SIGTERM :
			printf("SIGTERM..\n");	break;
		case SIGABRT :
			printf("SIGABRT..\n");	break;
		default :
			break;
	}

	exit(EXIT_FAILURE);
}

//------------------------------------------------------------------------------
static void register_signal( void )
{
	signal( SIGINT,  signal_handler );
	signal( SIGTERM, signal_handler );
	signal( SIGABRT, signal_handler );
}

//------------------------------------------------------------------------------
static void make_directory( const char *fmt, ... )
{
	va_list args;
	char dir[1024];
	char *ptr = dir;
	memset( dir, 0x00, sizeof(dir) );

	va_start(args, fmt);
	vsnprintf(dir, sizeof(dir), fmt, args);
	va_end(args);

	while( *ptr )
	{
		if( '/' == *ptr )
		{
			*ptr = 0x00;
			if( 0 != access( dir, F_OK ) && (ptr != dir) )
			{
				printf("Make Directory. ( %s )\n", dir);
				mkdir( dir, 0777 );
			}
			*ptr = '/';
		}
		ptr++;
	}

	if( 0 != access( dir, F_OK) )
	{
		printf("Make Directory. ( %s )\n", dir);
		mkdir( dir, 0777 );
	}
}

//------------------------------------------------------------------------------
static void print_usage(const char *name)
{
	printf(
		"Usage : %s -i [input file] -o [output path] [options]\n"
		"   -i [input file]    [M] : input media file name\n"
		"   -o [output path]   [M] : output file name\n"
		"   -j [start time]    [O] : start time\n"
		"   -l [end frame]     [O] : end frame\n"
		"   -h : help\n"
		,name
	);
}

//------------------------------------------------------------------------------
static int32_t vpu_dec_dump( NX_APP_INFO* info )
{
	uint8_t strm_buf[4*1024*1024];
	NX_V4L2DEC_HANDLE dec = NULL;
	NX_VIDEO_HANDLE stream = NULL;

	int32_t ret;
	int32_t init = false;
	int32_t size, additional;
	int32_t frm_cnt = 0, strm_cnt = 0, img_cnt = 0;
	int32_t prv_index = -1;
	int32_t key;
	int64_t timestamp;

	int32_t width, height;
	uint32_t fmt;

	NX_V4L2DEC_SEQ_IN seq_in;
	NX_V4L2DEC_SEQ_OUT seq_out;

	NX_V4L2DEC_IN dec_in;
	NX_V4L2DEC_OUT dec_out;

	NX_STREAM_INFO strm_info;

	register_signal();

	stream = nx_video_open( info->in_file );
	if( !stream )
	{
		printf("fail, nx_video_open().\n");
		goto TERMINATE;
	}

	if( 0 > nx_video_get_format( stream, &fmt ) )
	{
		printf("fail, nx_video_get_format().\n");
		goto TERMINATE;
	}

	if( 0 > nx_video_get_resolution( stream, &width, &height ) )
	{
		printf("fail, nx_video_get_resolution().\n");
		goto TERMINATE;
	}

	printf("file: %s ( resolution: %d x %d, format: 0x%08X )\n",
		info->in_file, width, height, fmt );

	dec = NX_V4l2DecOpen( fmt );
	if( NULL == dec )
	{
		return -1;
	}

	if( 0 != info->start_sec )
	{
		int32_t seektime;
		int64_t duration;
		nx_video_get_duration( stream, &duration );

		seektime = (0 > info->start_sec) ? duration/1000000 + info->start_sec : info->start_sec;
		printf("jump to %d sec.\n", seektime);

		nx_video_seek( stream, seektime * 1000 );
	}

	do {
		if( !init )
		{
			nx_video_get_seqinfo( stream, strm_buf, &additional );
		}

		if( 0 > nx_video_read( stream, strm_buf + additional, &size, &key, &timestamp ) )
		{
			size = 0;
		}

		if( !init && !key )
		{
			continue;
		}

		if( !init )
		{
			memset( &seq_in, 0x00, sizeof(seq_in) );
			seq_in.width    = width;
			seq_in.height   = height;
			seq_in.strmBuf  = strm_buf + additional;
			seq_in.strmSize = size;
			seq_in.seqBuf   = strm_buf;
			seq_in.seqSize  = additional;
			seq_in.timeStamp = 0;

			strm_info.buf  = seq_in.seqBuf;
			strm_info.size = seq_in.seqSize;
			nx_stream_dump( &strm_info, "%s/bitstream/bitstream_%04d.bit",info->out_dir, strm_cnt );
			strm_cnt++;

			strm_info.buf  = seq_in.strmBuf;
			strm_info.size = seq_in.strmSize;
			nx_stream_dump( &strm_info, "%s/bitstream/bitstream_%04d.bit",info->out_dir, strm_cnt );
			strm_cnt++;

			ret = NX_V4l2DecParseVideoCfg( dec, &seq_in, &seq_out );
			if( 0 > ret )
			{
				printf("fail, NX_V4l2DecParseVideoCfg().\n");
				break;
			}

			seq_in.width       = seq_out.width;
			seq_in.height      = seq_out.height;
			seq_in.imgPlaneNum = nx_get_plane_num(NX_IMAGE_FORMAT);
			seq_in.imgFormat   = NX_IMAGE_FORMAT;
			seq_in.numBuffers  = seq_out.minBuffers + 3;

			printf("[ Sequence] width( %d ), height( %d ), plane( %d ), format( 0x%08x ), reqiured buffer( %d ), current buffer( %d )\n",
				seq_in.width, seq_in.height, seq_in.imgPlaneNum, seq_in.imgFormat, seq_out.minBuffers, seq_in.numBuffers );

			ret = NX_V4l2DecInit(dec, &seq_in);
			if( 0 > ret )
			{
				printf("fail, NX_V4l2DecInit().\n");
				break;
			}

			init = true;
			additional = 0;
			continue;
		}

		do {
			memset(&dec_in, 0, sizeof(NX_V4L2DEC_IN));
			dec_in.strmBuf   = (size > 0) ? strm_buf : NULL;
			dec_in.strmSize  = (size > 0) ? size + additional : 0;
			dec_in.timeStamp = (size > 0) ? timestamp : 0;
			dec_in.eos       = (size > 0) ? 0 : 1;

			strm_info.buf  = dec_in.strmBuf;
			strm_info.size = dec_in.strmSize;
			nx_stream_dump( &strm_info, "%s/bitstream/bitstream_%04d.bit",info->out_dir, strm_cnt );
			strm_cnt++;

			ret = NX_V4l2DecDecodeFrame(dec, &dec_in, &dec_out);
			if( 0 < ret )
			{
				printf("[%5d Frm] need more frame.\n", frm_cnt);
				continue;
			}

			printf("[%5d Frm] Size=%6d, DecIdx=%2d, DispIdx=%2d, InTimeStamp=%7lld, outTimeStamp=%7lld, interlace=%1d %1d, Reliable=%3d, %3d, type =%3d, %3d, UsedByte=%6d, RemainByte=%6d\n",
				frm_cnt, dec_in.strmSize, dec_out.decIdx, dec_out.dispIdx, dec_in.timeStamp, dec_out.timeStamp[DISPLAY_FRAME], dec_out.interlace[DECODED_FRAME], dec_out.interlace[DISPLAY_FRAME],
				dec_out.outFrmReliable_0_100[DECODED_FRAME], dec_out.outFrmReliable_0_100[DISPLAY_FRAME],
				dec_out.picType[DECODED_FRAME], dec_out.picType[DISPLAY_FRAME],
				dec_out.usedByte, dec_out.remainByte
			);

			if( 0 <= dec_out.dispIdx )
			{
				nx_memory_dump( &dec_out.hImg, "%s/golden/golden_%04d.yuv", info->out_dir, img_cnt );
				img_cnt++;

				if( 0 <= prv_index )
				{
					ret = NX_V4l2DecClrDspFlag(dec, NULL, prv_index);
					if( 0 > ret )
					{
						printf("fail, NX_V4l2DecClrDspFlag().\n");
						break;
					}
				}
				prv_index = dec_out.dispIdx;
			}
		} while( size == 0 && dec_out.remainByte != 0 );

		if( (0 > ret) || (size == 0 && dec_out.remainByte == 0) )
		{
			break;
		}

		if( info->end_frame && (img_cnt >= info->end_frame) )
		{
			break;
		}

		frm_cnt++;
	} while(1);

TERMINATE:
	if( dec )
	{
		if( 0 <= prv_index )
		{
			NX_V4l2DecClrDspFlag(dec, NULL, prv_index);
			prv_index = -1;
		}

		NX_V4l2DecClose(dec);
	}

	return 0;
}

//------------------------------------------------------------------------------
int32_t main( int32_t argc, char *argv[] )
{
	int32_t ret = 0;
	int32_t opt;

	NX_APP_INFO info;
	memset( &info, 0x00, sizeof(info) );

	register_signal();

	while (-1 != (opt = getopt(argc, argv, "i:o:j:l:h")))
	{
		switch(opt)
		{
		case 'i':	info.in_file = strdup(optarg);	break;
		case 'o':	info.out_dir = strdup(optarg);	break;
		case 'j':	info.start_sec = atoi(optarg);	break;
		case 'l':	info.end_frame = atoi(optarg);	break;
		case 'h':	print_usage(argv[0]);			return 0;
		default:	break;
		}
	}

	if( !info.in_file || !info.out_dir )
	{
		print_usage(argv[0]);
		return -1;
	}

	make_directory( "%s/bitstream/", info.out_dir );
	make_directory( "%s/golden/", info.out_dir );

	ret = vpu_dec_dump( &info );

	if( info.in_file ) free( info.in_file );
	if( info.out_dir ) free( info.out_dir );

	return ret;
}
