#!/bin/sh

docker build --platform linux/amd64 --progress plain -t staticdeps-travis -f docker/x86_64/Dockerfile-travis .
rm -rf docker-artifacts
mkdir -p docker-artifacts
docker run --rm -v ${PWD}/docker-artifacts:/usr/src/staticdeps/_build staticdeps-travis
