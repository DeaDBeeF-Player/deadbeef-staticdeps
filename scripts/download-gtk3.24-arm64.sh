#!/bin/sh
set -e

DIST="questing"
ARCH="arm64"

GTK_VERSION="3.24"
BASE_DIR="lib-arm64/gtk-${GTK_VERSION}"
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

download_pkg libgtk-3-0t64
download_pkg libgtk-3-dev

download_pkg libatk1.0-0t64
download_pkg libatk1.0-dev

download_pkg libcairo2
download_pkg libcairo2-dev
download_pkg libcairo-gobject2

download_pkg libgdk-pixbuf-2.0-0
download_pkg libgdk-pixbuf-2.0-dev

download_pkg libglib2.0-0t64
download_pkg libglib2.0-dev

download_pkg libgio-2.0-dev
download_pkg libglib2.0-bin
download_pkg libglib2.0-dev-bin

download_pkg libpango-1.0-0
download_pkg libpangocairo-1.0-0
download_pkg libpangoft2-1.0-0
download_pkg libpango1.0-dev

download_pkg libfreetype6

download_pkg libffi8
download_pkg libffi-dev

download_pkg libatk-bridge2.0-0t64
download_pkg libatk-bridge2.0-dev

download_pkg libcairo-script-interpreter2
download_pkg libpangoxft-1.0-0

# --- extract ---

for i in *.deb; do
    ar x "$i"

    if [ -f data.tar.xz ]; then
        tar xf data.tar.xz
        rm data.tar.xz
    elif [ -f data.tar.zst ]; then
        tar --use-compress-program=unzstd -xf data.tar.zst
        rm data.tar.zst
    fi

    rm -f debian-binary control.tar.* || true
done
