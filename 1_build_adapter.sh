#!/bin/bash

# This script builds the Docker image for the adapter_pabx service.
# NOTE: The build is set to target the amd64 architecture, building PJSIP for arm64 will not work

docker build --platform linux/amd64 -t adapter_pabx:dev -f Dockerfile .