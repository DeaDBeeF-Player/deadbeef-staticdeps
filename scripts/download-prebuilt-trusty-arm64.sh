#!/bin/sh
set -e

DIST="trusty"
ARCH="arm64"

BASE_DIR="lib-arm64/prebuilt"
mkdir -p "${BASE_DIR}"
cd "${BASE_DIR}"

BASE_URL="http://ports.ubuntu.com/ubuntu-ports"
PKG_INDEX="Packages.gz"

# --- download package index ---

if [ ! -f "$PKG_INDEX" ]; then
    echo "Downloading package index..."
    curl -L -o "$PKG_INDEX" \
        "$BASE_URL/dists/$DIST/main/binary-$ARCH/Packages.gz"
fi

# --- resolve package to exact .deb path ---

resolve_pkg() {
    pkg="$1"

    gzip -dc "$PKG_INDEX" \
    | awk -v pkg="$pkg" '
        $1 == "Package:" && $2 == pkg { found=1 }
        found && $1 == "Filename:" { print $2; exit }
    '
}

download_pkg() {
    pkg="$1"

    file_path="$(resolve_pkg "$pkg")"

    if [ -z "$file_path" ]; then
        echo "ERROR: package not found: $pkg"
        exit 1
    fi

    file="$(basename "$file_path")"

    if [ ! -f "$file" ]; then
        echo "Downloading $file ..."
        curl -L -C - -o "$file" "$BASE_URL/$file_path"
    else
        echo "Skipping $file"
    fi
}

# --- packages (same list you had) ---

download_pkg libx11-6
download_pkg libx11-dev
download_pkg x11proto-core-dev
download_pkg libxcursor1
download_pkg libxcursor-dev
download_pkg libxext6
download_pkg libxext-dev
download_pkg libxfixes3
download_pkg libxfixes-dev
download_pkg libxrandr2
download_pkg libxrandr-dev
download_pkg libxrender1
download_pkg libxrender-dev
download_pkg libjack-jackd2-0
download_pkg libjack-jackd2-dev
download_pkg libpulse0
download_pkg libpulse-mainloop-glib0
download_pkg libpulse-dev
download_pkg libstdc++6
download_pkg libstdc++6-4.7-dev
download_pkg libgcc1
download_pkg libgcc-4.7-dev
# pipewire is not available in trusty
#download_pkg libpipewire-0.3-0t64
#download_pkg libpipewire-0.3-dev
#download_pkg libspa-0.2-dev
#download_pkg libspa-0.2-bluetooth
#download_pkg libspa-0.2-libcamera
#download_pkg libspa-0.2-modules

# --- extract ---

for i in *.deb; do
    ar x "$i"

    if [ -f data.tar.xz ]; then
        tar xf data.tar.xz
        rm data.tar.xz
    elif [ -f data.tar.gz ]; then
        tar zxf data.tar.gz
        rm data.tar.gz
    elif [ -f data.tar.zst ]; then
        tar --use-compress-program=unzstd -xf data.tar.zst
        rm data.tar.zst
    fi

    rm -f debian-binary control.tar.* || true
done
