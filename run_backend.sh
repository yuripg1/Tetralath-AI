#!/usr/bin/env bash
set -eu -o pipefail
source ./config.sh

make --directory=${BACKEND_DIR} clean
make --directory=${BACKEND_DIR}

${BACKEND_EXECUTABLE}
