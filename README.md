# ROS2 PABX Adapter

A modern, open-source server for managing PABX (Private Automatic Branch Exchange) announcement actions via SIP. This project enables automated announcements to be triggered on PABX systems, making it ideal for telephony automation, robotics, and smart office integrations.


> NOTE: This server is expected to be deployed for a single PABX network.

## Features

- Trigger announcements to PBX extensions programmatically
- SIP account management via simple JSON config
- Dockerized for easy deployment and development
- ROS action interface for integration with robotics systems

## Getting Started

### Prerequisites
- Docker

### Quick Start

Clone the repository and start the server using Docker:

```
git clone https://github.com/KABAM-Robotics/ros2-pabx-adapter.git
cd ros2-pabx-adapter
docker compose build adapter_pabx
docker compose up --remove-orphans adapter_pabx
```

### SIP Account Configuration

> NOTE: Current implementation only supports a single caller account, we plan to expand it to handle multiple accounts

SIP accounts are configured in workspace/src/adapter_pabx/config/accounts.json:

```JSON
{
    "<account_name>": {
        "username": "<extension>",
        "password": "<passwd>",
        "domain": "<ip>"
    }
}
```

You can mount your own SIP account configuration file using the compose configuration.
The compose configuration that comes with the project assumes that your config file is named `accounts.json` and is located at the root of the project.

### Placing a call
In another terminal, trigger an announcement (edit customer_extension in the script as needed):

> NOTE: Remember to update the extensions in the file first

```
bash request_announcement.sh
```



## ROS Action Definition

``` YAML
# Goal definition
uint32 customer_extension
uint32 announcement_device_extension
string requester_name
string robot_id
---
# Result definition
string announcement_result
---
# Feedback definition
string call_status

```


## Maintainers

- cardboardcode
- ibrahiminfinite