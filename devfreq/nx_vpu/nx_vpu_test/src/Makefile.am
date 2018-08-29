AM_CFLAGS = \
	$(WARN_CFLAGS)	\
	-I.				\
	-I${includedir}	\
	-I${includedir}/libdrm	\
	-I${top_builddir}/src/include

nx_vpu_test_LDADD = \
	-L${libdir}			\
	-ldrm				\
	-lnx_video_alloc	\
	-lnx_video_api

nx_vpu_test_SOURCES = \
	nx_video_utils.c	\
	nx_list.c			\
	main.c

bin_PROGRAMS = nx_vpu_test

