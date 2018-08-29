AM_CFLAGS = \
	$(WARN_CFLAGS)	\
	-I./include		\
	-I${includedir}	\
	-I${includedir}/libdrm

libnx_video_alloc_la_LTLIBRARIES = libnx_video_alloc.la
libnx_video_alloc_ladir = ${libdir}
libnx_video_alloc_la_LDFLAGS = -L${libdir} -ldrm

libnx_video_alloc_la_SOURCES = \
	nx_video_alloc.c

libnx_video_allocincludedir = ${includedir}
libnx_video_allocinclude_HEADERS = \
	nx_video_alloc.h

nobase_include_HEADERS = \
	linux/videodev2_nxp_media.h
