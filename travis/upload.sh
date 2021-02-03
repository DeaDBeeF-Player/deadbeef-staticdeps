echo Decrypting id_rsa...

mkdir -p sshconfig

if [ ! -z $gh_rsa_key ]; then
    openssl aes-256-cbc -K $gh_rsa_key -iv $gh_rsa_iv -in .github/id_rsa.enc -out sshconfig/id_rsa -d || exit 1
elif [ ! -z $encrypted_b1899526f957_key ]; then
    openssl aes-256-cbc -K $encrypted_b1899526f957_key -iv $encrypted_b1899526f957_iv -in travis/id_rsa.enc -out sshconfig/id_rsa -d || exit 1
else
    echo "SSH key is not available, upload cancelled"
    exit 0
fi

eval "$(ssh-agent -s)"
chmod 600 sshconfig/id_rsa
ssh-add sshconfig/id_rsa || exit 1

SSHOPTS="ssh -o StrictHostKeyChecking=no"

case "$TRAVIS_OS_NAME" in
    linux)
        echo Uploading linux artifacts...
        rsync -e "$SSHOPTS" ${PWD}/_build/ddb-static-deps-latest.tar.bz2 waker,deadbeef@frs.sourceforge.net:/home/frs/project/d/de/deadbeef/staticdeps || exit 1
    ;;
esac
