#!/bin/sh

docker build -t staticdeps-travis -f docker/Dockerfile-travis .
rm -rf docker-artifacts
mkdir -p docker-artifacts
docker run --rm -v ${PWD}/docker-artifacts:/usr/src/staticdeps/_build staticdeps-travis
