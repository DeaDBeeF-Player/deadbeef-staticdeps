echo Decrtypting id_rsa...
openssl aes-256-cbc -K $encrypted_2d29dbf2147c_key -iv $encrypted_2d29dbf2147c_iv -in travis/id_rsa.enc -out travis/id_rsa -d || exit -1
eval "$(ssh-agent -s)"
chmod 600 travis/id_rsa
ssh-add travis/id_rsa || exit 1

SSHOPTS="ssh -o StrictHostKeyChecking=no"

case "$TRAVIS_OS_NAME" in
    linux)
        echo Uploading linux artifacts...
        rsync -e "$SSHOPTS" docker_artifacts/ddb-static-deps-latest.tar.bz2 waker,deadbeef@frs.sourceforge.net:/home/frs/project/d/de/deadbeef/staticdeps || exit 1
    ;;
esac
