./configure --extra-cflags="-fPIC -I$PREFIX/include/opus $CFLAGS"\
    --enable-pic --enable-static --enable-gpl --disable-doc --disable-ffplay\
    --disable-ffprobe --disable-ffserver --disable-avdevice --disable-ffmpeg\
    --disable-postproc --disable-swresample --disable-avfilter\
    --disable-swscale --enable-network --disable-swscale-alpha --disable-vdpau\
    --disable-videotoolbox\
    --disable-dxva2 --enable-hwaccels\
    --disable-encoders --disable-muxers --disable-indevs --disable-outdevs\
    --disable-devices --disable-filters --disable-parsers --enable-parser=ac3\
    --enable-demuxer=ac3 --disable-bsfs --disable-bzlib --disable-protocols\
    --disable-decoders --enable-decoder=libopus --enable-libopus\
    --enable-decoder=wmapro --disable-decoder=wmav1 --disable-decoder=wmav2\
    --enable-decoder=wmavoice --disable-decoder=alac --enable-decoder=ac3\
    --enable-decoder=amrnb --disable-demuxers --enable-demuxer=asf\
    --disable-demuxer=mov --enable-demuxer=oma --enable-demuxer=ac3\
    --enable-demuxer=amr --enable-demuxer=ogg\
    --enable-demuxer=tak --enable-decoder=tak\
    --enable-decoder=dsd_lsbf --enable-decoder=dsd_lsbf_planar\
    --enable-decoder=dsd_msbf --enable-decoder=dsd_msbf_planar\
    --enable-demuxer=dsf --enable-demuxer=iff\
    --enable-libopencore-amrnb --enable-libopencore-amrwb\
    --enable-version3 --disable-mmx\
    --disable-yasm\
    --enable-protocol=file --enable-protocol=http --prefix="$PREFIX"

make clean
make V=1 CC=$CC

