#pragma once
#include <Godot.hpp>
#include <PacketPeer.hpp>

namespace godot {

class Compression {
public:
    static PackedByteArray compress(const PackedByteArray &data, const String &algorithm = "zstd");
    static PackedByteArray decompress(const PackedByteArray &compressed_data, const String &algorithm = "zstd");
};

}