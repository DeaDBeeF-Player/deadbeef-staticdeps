#!/bin/bash

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
docker build --progress plain -t staticdeps -f $DIR/Dockerfile-build .
rm -rf docker-artifacts
mkdir -p docker-artifacts
docker run -t -i --rm -v ${PWD}/docker-artifacts:/usr/src/staticdeps/_build staticdeps
