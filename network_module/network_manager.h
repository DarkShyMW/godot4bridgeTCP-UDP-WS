#pragma once
#include <Godot.hpp>
#include <Node.hpp>
#include <Timer.hpp>
#include <vector>

namespace godot {

class NetworkManager : public Node {
    GDCLASS(NetworkManager, Node)

public:
    enum Protocol { TCP, UDP, WEBSOCKET };
    enum EncryptionMode { NONE, AES256 };
    
protected:
    String last_address;
    int last_port = 0;
    int max_reconnect_attempts = 5;
    float reconnect_interval = 2.0f;
    int current_reconnect_attempts = 0;
    bool auto_reconnect = true;
    Timer* reconnect_timer = nullptr;
    
    EncryptionMode encryption_mode = NONE;
    String encryption_key;
    
    struct NetworkStats {
        uint64_t bytes_sent = 0;
        uint64_t bytes_received = 0;
        uint32_t packets_lost = 0;
        float avg_ping = 0.0f;
        float connection_time = 0.0f;
    } stats;
    
    static void _bind_methods();
    void _notification(int p_what);
    
    void start_reconnect_timer();
    void stop_reconnect_timer();
    void log_message(const String& message, const String& level);

public:
    // Base methods
    virtual void connect_to_host(String address, int port);
    virtual void send_data(const Variant& data);
    virtual void disconnect();
    virtual bool is_connected() const;
    
    // Configuration
    void set_auto_reconnect(bool enabled);
    bool get_auto_reconnect() const;
    
    void set_encryption_mode(EncryptionMode mode);
    EncryptionMode get_encryption_mode() const;
    
    void set_encryption_key(const String& key);
    String get_encryption_key() const;
    
    // Stats
    Dictionary get_stats() const;
    
    // Signals
    SIGNAL(NETWORK_CONNECTED)
    SIGNAL(NETWORK_DISCONNECTED)
    SIGNAL(NETWORK_DATA_RECEIVED, "data")
    SIGNAL(NETWORK_ERROR, "error_message")
    SIGNAL(NETWORK_RECONNECT_FAILED)
    
    NetworkManager();
    virtual ~NetworkManager();
};

}

VARIANT_ENUM_CAST(NetworkManager::Protocol);
VARIANT_ENUM_CAST(NetworkManager::EncryptionMode);