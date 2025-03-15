#include "network_manager.h"
#include <Godot.hpp>
#include <OS.hpp>
#include <Engine.hpp>
#include <Crypto.hpp>
#include <CryptoKey.hpp>
#include <JSON.hpp>

using namespace godot;

void NetworkManager::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_PROCESS:
            // Обработка сетевых событий
            break;
        case NOTIFICATION_EXIT_TREE:
            disconnect();
            break;
    }
}

void NetworkManager::_bind_methods() {
    ClassDB::bind_method(D_METHOD("connect_to_host", "address", "port"), &NetworkManager::connect_to_host);
    ClassDB::bind_method(D_METHOD("send_data", "data"), &NetworkManager::send_data);
    ClassDB::bind_method(D_METHOD("disconnect"), &NetworkManager::disconnect);
    ClassDB::bind_method(D_METHOD("is_connected"), &NetworkManager::is_connected);
    
    ClassDB::bind_method(D_METHOD("set_auto_reconnect", "enabled"), &NetworkManager::set_auto_reconnect);
    ClassDB::bind_method(D_METHOD("get_auto_reconnect"), &NetworkManager::get_auto_reconnect);
    
    ClassDB::bind_method(D_METHOD("set_encryption_mode", "mode"), &NetworkManager::set_encryption_mode);
    ClassDB::bind_method(D_METHOD("get_encryption_mode"), &NetworkManager::get_encryption_mode);
    
    ClassDB::bind_method(D_METHOD("set_encryption_key", "key"), &NetworkManager::set_encryption_key);
    ClassDB::bind_method(D_METHOD("get_encryption_key"), &NetworkManager::get_encryption_key);
    
    ClassDB::bind_method(D_METHOD("get_stats"), &NetworkManager::get_stats);
    
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "auto_reconnect"), "set_auto_reconnect", "get_auto_reconnect");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "encryption_mode", PROPERTY_HINT_ENUM, "None,AES256"), "set_encryption_mode", "get_encryption_mode");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "encryption_key"), "set_encryption_key", "get_encryption_key");
    
    ADD_SIGNAL(MethodInfo("network_connected"));
    ADD_SIGNAL(MethodInfo("network_disconnected"));
    ADD_SIGNAL(MethodInfo("network_data_received", PropertyInfo(Variant::OBJECT, "data")));
    ADD_SIGNAL(MethodInfo("network_error", PropertyInfo(Variant::STRING, "error_message")));
    ADD_SIGNAL(MethodInfo("network_reconnect_failed"));
    
    BIND_ENUM_CONSTANT(TCP);
    BIND_ENUM_CONSTANT(UDP);
    BIND_ENUM_CONSTANT(WEBSOCKET);
    BIND_ENUM_CONSTANT(NONE);
    BIND_ENUM_CONSTANT(AES256);
}

NetworkManager::NetworkManager() {
    reconnect_timer = nullptr;
    last_port = 0;
    current_reconnect_attempts = 0;
    stats = NetworkStats{};
}

NetworkManager::~NetworkManager() {
    if (reconnect_timer) {
        reconnect_timer->queue_free();
    }
}

void NetworkManager::connect_to_host(String address, int port) {
    last_address = address;
    last_port = port;
    current_reconnect_attempts = 0;
}

void NetworkManager::send_data(const Variant &data) {
    // Базовый метод, должен быть переопределен в наследниках
    ERR_PRINT("send_data() not implemented in base NetworkManager!");
}

void NetworkManager::disconnect() {
    stop_reconnect_timer();
}

bool NetworkManager::is_connected() const {
    return false;
}

void NetworkManager::start_reconnect_timer() {
    if (!reconnect_timer) {
        reconnect_timer = Timer::_new();
        reconnect_timer->set_name("ReconnectTimer");
        reconnect_timer->set_wait_time(reconnect_interval);
        reconnect_timer->set_one_shot(true);
        reconnect_timer->connect("timeout", this, "_on_reconnect_timeout");
        add_child(reconnect_timer);
    }
    reconnect_timer->start();
}

void NetworkManager::stop_reconnect_timer() {
    if (reconnect_timer && reconnect_timer->is_inside_tree()) {
        reconnect_timer->stop();
    }
}

void NetworkManager::_on_reconnect_timeout() {
    if (current_reconnect_attempts < max_reconnect_attempts) {
        current_reconnect_attempts++;
        connect_to_host(last_address, last_port);
    } else {
        emit_signal("network_reconnect_failed");
    }
}

// Шифрование данных
PackedByteArray NetworkManager::encrypt_data(const PackedByteArray &data) {
    if (encryption_mode == AES256 && !encryption_key.empty()) {
        Ref<Crypto> crypto = Crypto::create();
        Ref<CryptoKey> key = CryptoKey::create();
        key->set_secret(encryption_key);
        return crypto->encrypt(data, key);
    }
    return data;
}

PackedByteArray NetworkManager::decrypt_data(const PackedByteArray &data) {
    if (encryption_mode == AES256 && !encryption_key.empty()) {
        Ref<Crypto> crypto = Crypto::create();
        Ref<CryptoKey> key = CryptoKey::create();
        key->set_secret(encryption_key);
        return crypto->decrypt(data, key);
    }
    return data;
}

// Статистика
void NetworkManager::update_stats_sent(int bytes) {
    stats.bytes_sent += bytes;
    stats.connection_time = OS::get_singleton()->get_ticks_msec() / 1000.0;
}

void NetworkManager::update_stats_received(int bytes) {
    stats.bytes_received += bytes;
}

Dictionary NetworkManager::get_stats() const {
    Dictionary stats_dict;
    stats_dict["bytes_sent"] = stats.bytes_sent;
    stats_dict["bytes_received"] = stats.bytes_received;
    stats_dict["packets_lost"] = stats.packets_lost;
    stats_dict["avg_ping"] = stats.avg_ping;
    stats_dict["connection_time"] = stats.connection_time;
    return stats_dict;
}

// Логирование
void NetworkManager::log_message(const String &message, const String &level) {
    String log_msg = String("[Network] [{0}] {1}").format(Array::make(level, message));
    Engine::get_singleton()->print(log_msg);
}

// Свойства
void NetworkManager::set_auto_reconnect(bool enabled) {
    auto_reconnect = enabled;
}

bool NetworkManager::get_auto_reconnect() const {
    return auto_reconnect;
}

void NetworkManager::set_encryption_mode(EncryptionMode mode) {
    encryption_mode = mode;
}

NetworkManager::EncryptionMode NetworkManager::get_encryption_mode() const {
    return encryption_mode;
}

void NetworkManager::set_encryption_key(const String &key) {
    encryption_key = key;
}

String NetworkManager::get_encryption_key() const {
    return encryption_key;
}