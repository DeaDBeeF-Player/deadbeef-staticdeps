#!/bin/sh

# run this script once to create the base container image,
# which would contain all necessary dependencies and build tools

docker build --no-cache -f docker/Dockerfile-builder -t staticdeps-builder .
