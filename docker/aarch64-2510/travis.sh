#!/bin/sh

DIR="$(dirname "${BASH_SOURCE[0]})"
ARTIFACTS_DIR=$DIR/../../docker-artifacts
docker build -t staticdeps-travis -f $DIR/Dockerfile-travis .
rm -rf docker-artifacts
mkdir -p docker-artifacts
docker run --rm -v ${ARTIFACTS_DIR}:/usr/src/staticdeps/_build staticdeps-travis
