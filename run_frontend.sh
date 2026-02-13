BACKEND_DIR="./backend"
BACKEND_LIBRARY="${BACKEND_DIR}/libtetralath.so"
make --directory=${BACKEND_DIR} clean
make --directory=${BACKEND_DIR} lib

FRONTEND_DIR="./frontend"
VENV_DIR="${FRONTEND_DIR}/.venv"
python3 -m venv ${VENV_DIR}
source ${VENV_DIR}/bin/activate
pip3 install -r ${FRONTEND_DIR}/requirements.txt
python3 ${FRONTEND_DIR}/main.py