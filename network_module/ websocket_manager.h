#pragma once
#include "network_manager.h"
#include <WebSocketClient.hpp>

class WebSocketManager : public NetworkManager {
    GDCLASS(WebSocketManager, NetworkManager)
    
    godot::Ref<godot::WebSocketClient> client;
    
    void _process(float delta) override;
    static void _bind_methods();
    
public:
    void connect_to_host(godot::String address, int port) override;
    void send_data(godot::Variant data) override;
    void disconnect() override;
    
    WebSocketManager();
    ~WebSocketManager();
};