AM_CFLAGS = \
	$(WARN_CFLAGS)	\
	-I./include		\
	-I${includedir}	\
	-I${includedir}/libdrm

libnx_video_api_la_LTLIBRARIES = libnx_video_api.la
libnx_video_api_ladir = ${libdir}
libnx_video_api_la_LDFLAGS = -L${libdir} -ldrm

libnx_video_api_la_SOURCES = \
	nx_video_enc.c		\
	nx_video_dec.c

libnx_video_apiincludedir = ${includedir}
libnx_video_apiinclude_HEADERS = \
	nx_video_api.h

nobase_include_HEADERS = \
	linux/videodev2_nxp_media.h
