#include "websocket_manager.h"

using namespace godot;

void WebSocketManager::_bind_methods() {
    ClassDB::bind_method(D_METHOD("connect_to_host", "address", "port"), &WebSocketManager::connect_to_host);
    ClassDB::bind_method(D_METHOD("send_data", "data"), &WebSocketManager::send_data);
    ClassDB::bind_method(D_METHOD("disconnect"), &WebSocketManager::disconnect);
}

WebSocketManager::WebSocketManager() {
    client.instance();
    client->connect("data_received", this, "_on_data_received");
}

void WebSocketManager::connect_to_host(String address, int port) {
    String url = String("ws://{0}:{1}").format(Array::make(address, port));
    client->connect_to_url(url);
}

void WebSocketManager::send_data(Variant data) {
    if(data.get_type() == Variant::STRING) {
        client->put_string(data);
    } else {
        client->put_packet(data);
    }
}

void WebSocketManager::_process(float delta) {
    client->poll();
}