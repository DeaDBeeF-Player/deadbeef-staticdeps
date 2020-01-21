FROM staticdeps-builder

WORKDIR /usr/src/staticdeps
COPY ./src /usr/src/staticdeps/src
COPY ./tools /usr/src/staticdeps/tools
COPY ./travis /usr/src/staticdeps/travis
COPY ./libgtk-x86_64 /usr/src/staticdeps/libgtk-x86_64
ENV TRAVIS_OS_NAME=linux

CMD [ "bash", "travis/build.sh" ]
