#!/bin/sh
#
# Perform a round-trip test between big-endian and little-endian
# BIN/CUE to see if we can read both files correctly.
#
set -e
: ${CMP:="/usr/bin/cmp"}
: ${WC:="/usr/bin/wc"}
: ${abs_builddir:="/src/external-vcs/github/rocky/libcdio-paranoia/test"}
: ${abs_srcdir:="/src/external-vcs/github/rocky/libcdio-paranoia/test"}
: ${abs_top_builddir:="/src/external-vcs/github/rocky/libcdio-paranoia"}

cd_paranoia="${abs_top_builddir}/src/cd-paranoia"

orig_le_cue_file="${abs_srcdir}/data/cdda.cue"
orig_le_bin_file="${abs_srcdir}/data/cdda.bin"

be_cue_file="${abs_builddir}/cdda-be.cue"
be_bin_file="${abs_builddir}/cdda-be.bin"

le_cue_file="${abs_builddir}/cdda-le.cue"
le_bin_file="${abs_builddir}/cdda-le.bin"

if test "${CMP}" = no; then
    echo "Don't see 'cmp' program. Test skipped."
    exit 77

elif test "${WC}" = no; then
    echo "Don't see 'wc' program. Test skipped."
    exit 77
fi

# Convert from little-endian to big-endian.
cat "${orig_le_cue_file}" > "${be_cue_file}"
${cd_paranoia} -d "${orig_le_cue_file}" -v -R -- "1-" "${be_bin_file}"

# The contents of these two .bin files should differ, but they should
# have the same length.
if test `${WC} -c < "${orig_le_bin_file}"` -ne `${WC} -c < "${be_bin_file}"`; then
    echo "** File sizes differ after byte swap"
    exit 3

elif ${CMP} -s "${orig_le_bin_file}" "${be_bin_file}"; then
    echo "** Identical raw file after byte swap"
    exit 3
fi

# Convert it back to little-endian.
cat "${be_cue_file}" > "${le_cue_file}"
${cd_paranoia} -d "${be_cue_file}" -v -r -- "1-" "${le_bin_file}"

# It should round-trip to the original file.
if ${CMP} "${le_bin_file}" "${orig_le_bin_file}"; then
    echo "** Identical raw file after round-trip"
    exit 0
else
    echo "** Raw files differ after round-trip"
    exit 3
fi
