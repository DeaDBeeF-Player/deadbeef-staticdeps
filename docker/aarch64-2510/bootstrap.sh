#!/bin/bash

# run this script once to create the base container image,
# which would contain all necessary dependencies and build tools

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
docker build --no-cache --progress plain -f "$DIR/Dockerfile-bootstrap" -t staticdeps-builder-arm64 .
