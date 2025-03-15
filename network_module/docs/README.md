# Network Module Documentation

## Features
- Multi-protocol support (TCP/UDP/WebSocket)
- Automatic reconnection
- QoS for UDP
- AES-256 encryption
- Data compression
- NAT Punchthrough
- Network statistics

## Quick Start

```gdscript
extends Node

var network

func _ready():
    network = UDPManager.new()
    network.auto_reconnect = true
    network.qos_mode = UDPManager.RELIABLE
    network.connect("network_data_received", self, "_on_data")
    network.connect_to_host("127.0.0.1", 9080)

func _on_data(data):
    print("Received:", data)

# API Reference
## UDPManager
Properties:

qos_mode (int) - Quality of Service mode

resend_timeout (float) - Packet resend timeout

Methods:

set_qos_mode(mode: QoSMode)

get_stats() -> Dictionary


# Как собрать?
`scons platform=windows target=release custom_modules=addons/network_module`