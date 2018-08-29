NX_FFMPEG_INC  = ${top_builddir}/src/ffmpeg/ffmpeg-2.8.6-arm32/include
NX_FFMPEG_LIB  = ${top_builddir}/src/ffmpeg/ffmpeg-2.8.6-arm32/lib

AM_CFLAGS = \
	$(WARN_CFLAGS)	\
	-I.				\
	-I./include		\
	-I${includedir}	\
	-I${includedir}/libdrm	\
	-I${top_builddir}/src/include	\
	-I$(NX_FFMPEG_INC)

nx_vpu_dump_LDADD = \
	-L${libdir}			\
	-ldrm				\
	-lnx_video_alloc	\
	-lnx_video_api		\
	-L$(NX_FFMPEG_LIB)	\
	-lavcodec			\
	-lavformat			\
	-lavutil			\
	-lswresample

nx_vpu_dump_SOURCES = \
	nx_video_utils.c	\
	nx_video_extractor.c\
	main.c

bin_PROGRAMS = nx_vpu_dump
