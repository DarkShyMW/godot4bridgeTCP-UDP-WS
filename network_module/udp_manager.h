#pragma once
#include "network_manager.h"
#include <ENet.hpp>

namespace godot {

class UDPManager : public NetworkManager {
    GDCLASS(UDPManager, NetworkManager)
    
public:
    enum QoSMode {
        UNRELIABLE,
        RELIABLE,
        SEQUENCED
    };
    
private:
    Ref<ENetConnection> enet;
    QoSMode qos_mode = RELIABLE;
    float resend_timeout = 0.5f;
    
    struct PacketData {
        uint32_t sequence;
        uint64_t timestamp;
        PacketPeer::PacketStatus status;
    };
    std::unordered_map<uint32_t, PacketData> outgoing_packets;
    uint32_t current_sequence = 0;
    
    void _process(float delta) override;
    void send_reliable(const PackedByteArray &data);
    
protected:
    static void _bind_methods();
    
public:
    void connect_to_host(String address, int port) override;
    void send_data(const Variant &data) override;
    void disconnect() override;
    bool is_connected() const override;
    
    void set_qos_mode(QoSMode mode);
    QoSMode get_qos_mode() const;
    
    UDPManager();
    ~UDPManager();
};

}

VARIANT_ENUM_CAST(UDPManager::QoSMode);