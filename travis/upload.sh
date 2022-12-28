echo Decrypting id_rsa...

mkdir -p sshconfig

if [ ! -z $gh_ed25519_key ]; then
    openssl aes-256-cbc -K $gh_ed25519_key -iv $gh_ed25519_iv -in .github/id_ed25519.enc -out sshconfig/id_ed25519 -d || exit 1
else
    echo "SSH key is not available, upload cancelled"
    exit 0
fi

eval "$(ssh-agent -s)"
chmod 600 sshconfig/id_ed25519
ssh-add sshconfig/id_ed25519 || exit 1

SSHOPTS="ssh -o StrictHostKeyChecking=no"

case "$TRAVIS_OS_NAME" in
    linux)
        echo Uploading linux artifacts...
        rsync -e "$SSHOPTS" ${PWD}/_build/ddb-static-deps-latest.tar.bz2 waker,deadbeef@frs.sourceforge.net:/home/frs/project/d/de/deadbeef/staticdeps || exit 1
    ;;
esac
