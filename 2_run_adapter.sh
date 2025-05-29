#!/bin/bash

# This script run the Docker container for the adapter_pabx service.

docker run \
    -it \
    --rm \
    --platform linux/amd64 \
    --name adapter_pabx \
    --net host \
    -v ./accounts.json:/home/workspace/src/adapter_pabx/config/accounts.json \
    adapter_pabx:dev

