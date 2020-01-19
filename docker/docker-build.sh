#!/bin/sh

docker build -t staticdeps -f docker/Dockerfile .
rm -rf docker-artifacts
mkdir -p docker-artifacts
docker run -v ${PWD}/docker-artifacts:/usr/src/staticdeps/_build staticdeps
