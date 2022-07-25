#!/bin/bash
python3 -m venv bayescli-env
source bayescli-env/bin/activate
pip install websockets
deactivate