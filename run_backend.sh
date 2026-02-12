BACKEND_DIR="./backend"
BACKEND_EXECUTABLE="${BACKEND_DIR}/tetralath"

make --directory=${BACKEND_DIR} clean
make --directory=${BACKEND_DIR}

${BACKEND_EXECUTABLE}
