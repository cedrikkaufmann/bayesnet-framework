#!./bayescli-env/bin/python

import sys
import argparse
import json
import websockets
import asyncio

async def main():
    # prepare argument parser
    parser = argparse.ArgumentParser()
    parser.add_argument('HOST', type=str, help='websocket host to connect to')
    parser.add_argument('PORT', type=int, help='websocket port to connect to')
    parser.add_argument('-s', '--script', action='store', dest='SCRIPT', type=str, help='script file to load')
    parser.add_argument('-c', '--command', action='store', dest='COMMAND', type=str, help='execute single command and disconnect')

    # parse arguments
    args = parser.parse_args()
    # read arguments
    port = args.PORT
    host = args.HOST
    script = args.SCRIPT
    command = args.COMMAND
    interactiveShell = False

    # check if interactive shell should be spawned
    if script == None:
        interactiveShell = True

    try:
        # connect to websocket
        async with websockets.connect(f"ws://{host}:{port}") as ws:
        
            # execute single command
            if command != None:
                await execute(ws, command)
                sys.exit(1)

            # interactive shell
            while interactiveShell:
                # read command
                command = input("> ")
                
                # check if we shall leave
                if command == "exit":
                    sys.exit(0)

                # execute command
                await execute(ws, command)

            # interpret script
            with open(script, "r") as file:
                # read file
                lines = file.readlines()

                # process file
                for index, line in enumerate(lines):
                    await execute(ws, line, index + 1)

    except ConnectionRefusedError:
        print(f"Error connecting to websocket (ws://{host}:{port}). Make sure the BayesServer is running.")
    
    except KeyboardInterrupt:
        sys.exit(0)

    except Exception as e:
        print(e)
        sys.exit(1)
    


def print_error(cmd, index = None):
    if index == None:
        print(f"unknown command: \"{cmd.strip()}\"")
    else:
        print(f"unknown command at line {index}: \"{cmd.strip()}\"")
    
async def execute(ws, cmd, index = None):
    # ignore empty lines
    if cmd.strip() == "":
        return

    # ignore comments
    if cmd.strip()[0] == ';':
        return

    # split cmd by whitespace
    s = cmd.strip().split(' ')

    # check if we have a print command
    if s[0] == "print":
        print(' '.join(s[1:]))
        return

    # process commands with single argument
    if len(s) == 2:
        if s[0] == "load_network":
            await load_network(ws, s[1])
            return

        if s[0] == "clear_evidence":
            await clear_evidence(ws, s[1])
            return

        if s[0] == "get_belief":
            await get_belief(ws, s[1])
            return

        # unknown command
        print_error(cmd, index)

    # process commands with 2 arguments
    elif len(s) == 3:
        if s[0] == "set_evidence":
            await set_evidence(ws, s[1], int(s[2]))
            return

        if s[0] == "observe":
            await observe(ws, s[1], float(s[2]))
            return

        # unknown command
        print_error(cmd, index)

    else:
        # unknown command
        print_error(cmd, index)


async def load_network(ws, file):
    data = {
        "action": "load_network",
        "payload": {
            "file": file
        }
    }
    
    await ws.send(json.dumps(data))
    result = await ws.recv()
    
    data = json.loads(result)
    
    if data['payload']['status'] != 'success':
        print(f"error: {data['payload']['error']}")

async def set_evidence(ws, node, state):
    data = {
        "action": "set_evidence",
        "payload": {
            "node": node,
            "state": state
        }
    }

    await ws.send(json.dumps(data))
    result = await ws.recv()
    
    data = json.loads(result)
    
    if data['payload']['status'] != 'success':
        print(f"error: {data['payload']['error']}")


async def clear_evidence(ws, node):
    data = {
        "action": "clear_evidence",
        "payload": {
            "node": node
        }
    }

    await ws.send(json.dumps(data))
    result = await ws.recv()

    data = json.loads(result)
    
    if data['payload']['status'] != 'success':
        print(f"error: {data['payload']['error']}")


async def observe(ws, node, value):
    data = {
        "action": "observe",
        "payload": {
            "node": node,
            "value": value
        }
    }

    await ws.send(json.dumps(data))
    result = await ws.recv()
    
    data = json.loads(result)
    
    if data['payload']['status'] != 'success':
        print(f"error: {data['payload']['error']}")


async def get_belief(ws, node):
    data = {
        "action": "get_belief",
        "payload": {
            "node": node
        }
    }

    await ws.send(json.dumps(data))
    result = await ws.recv()

    data = json.loads(result)
    
    if data['payload']['status'] != 'success':
        print(f"error: {data['payload']['error']}")
    else:
        print(data['payload'][node])


if __name__ == '__main__':
    asyncio.run(main())