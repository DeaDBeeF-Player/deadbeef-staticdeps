#!/bin/sh

docker build --platform linux/amd64 --progress plain -t staticdeps -f docker/x86_64/Dockerfile-build .
rm -rf docker-artifacts
mkdir -p docker-artifacts
docker run -t -i --rm -v ${PWD}/docker-artifacts:/usr/src/staticdeps/_build staticdeps
