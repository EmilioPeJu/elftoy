#!/bin/bash
# Generate a binary out of a shellcode
if [[ -z "$1" ]]; then
    echo "Please pass a shellcode path as a first argument"
    exit 1
fi
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PAYLOAD="$1"
PAYLOADSIZE="$(wc -c $PAYLOAD)"
cat "${DIR}/../elf/tinyelfheader" "$PAYLOAD" > "${PAYLOAD}.out"
r2 -n -w "${PAYLOAD}.out" <<EOF
s 0x60
w8+ $PAYLOADSIZE
s 0x68
w8+ $PAYLOADSIZE
EOF
chmod +x "${PAYLOAD}.out"
