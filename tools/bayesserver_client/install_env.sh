#!/bin/bash
python3 -m venv bayesserver-env
source bayesserver-env/bin/activate
pip install websockets
deactivate