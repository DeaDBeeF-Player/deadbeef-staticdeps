#!/bin/sh

# run this script once to create the base container image,
# which would contain all necessary dependencies and build tools

docker build --platform linux/amd64 --progress plain --no-cache -f docker/x86_64/Dockerfile-bootstrap -t staticdeps-builder .
