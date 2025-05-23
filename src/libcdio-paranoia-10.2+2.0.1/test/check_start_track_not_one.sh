#/bin/sh
# Check that a digital audio CD with a start track number of >1 works.

if test ! -d "$abs_top_builddir" ; then
  abs_top_builddir=/src/external-vcs/github/rocky/libcdio-paranoia
fi

if test ! -d "$abs_top_srcdir" ; then
  abs_top_srcdir=/src/external-vcs/github/rocky/libcdio-paranoia
fi

if test ! -d "abs_top_builddir" ; then
  abs_top_builddir=/src/external-vcs/github/rocky/libcdio-paranoia
fi

test_dir=$abs_top_srcdir/test
test_build_dir=$abs_top_builddir/test

set -e

if [ $($abs_top_builddir/test/get_libcdio_version) -le 20000 ] ; then
    exit 77
fi

cd_paranoia=$abs_top_builddir/src/cd-paranoia
base=start_track_not_one
orig_cdda=$test_dir/data/cdda.bin

# First we make a longer CDDA image.
cdda=$test_build_dir/$base.bin
cat $orig_cdda $orig_cdda $orig_cdda > $cdda

# And a CUE file with a non-one start track.
cue=$test_build_dir/$base.cue
cat << EOF > $cue
FILE "$cdda" BINARY
  TRACK 07 AUDIO
    FLAGS DCP
    INDEX 01 00:00:00
  TRACK 08 AUDIO
    FLAGS DCP
    INDEX 01 00:04:02
  TRACK 09 AUDIO
    FLAGS DCP
    INDEX 01 00:08:04
EOF

# Use an awk program to parse/verify the TOC.
# We check that we see only tracks 7, 8 and 9.
awk_prog='
BEGIN           { expect_track = 7; in_toc = 0; err = 0; num_tracks = 0 }
/^TOTAL/        { in_toc = 0 }
in_toc == 1  {
                    if ($1 != expect_track ".") {
                        print "Expected track " expect_track ", got " $1;
                        err = 1
                    }
                    expect_track ++;
                    num_tracks ++;
                }
/^===/          { in_toc = 1 }
END             {
                    if (num_tracks != 3) {
                        print "Number of tracks was not 3.";
                        err = 1
                    }
                    exit err;
                }
'

$cd_paranoia -Q -d $cue 2>&1 | gawk "$awk_prog"
