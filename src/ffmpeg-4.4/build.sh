./configure --extra-cflags="-fPIC $CFLAGS"\
    --enable-pic --enable-static --enable-gpl --disable-doc --disable-ffplay\
    --disable-ffprobe --disable-avdevice --disable-ffmpeg\
    --disable-postproc --disable-swresample --disable-avfilter\
    --disable-swscale --enable-network --disable-swscale-alpha --disable-vdpau\
    --disable-videotoolbox\
    --disable-dxva2 --enable-hwaccels\
    --disable-encoders --disable-muxers --disable-indevs --disable-outdevs\
    --disable-devices --disable-filters\
    --disable-bsfs --disable-bzlib --disable-protocols\
    --disable-libopus\
    --disable-decoders  --disable-demuxers --disable-parsers\
    --enable-decoder=wmapro\
    --enable-decoder=wmavoice\
    --enable-parser=ac3 --enable-demuxer=ac3 --enable-decoder=ac3\
    --enable-demuxer=eac3 --enable-decoder=eac3\
    --enable-decoder=amrnb --enable-demuxer=asf\
    --enable-demuxer=oma\
    --enable-demuxer=amr\
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

