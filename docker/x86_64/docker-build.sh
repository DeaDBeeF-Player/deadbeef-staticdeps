#!/bin/sh

docker build --progress plain -t staticdeps -f docker/Dockerfile .
rm -rf docker-artifacts
mkdir -p docker-artifacts
docker run -t -i --rm -v ${PWD}/docker-artifacts:/usr/src/staticdeps/_build staticdeps
