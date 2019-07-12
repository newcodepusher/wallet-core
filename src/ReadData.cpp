#include "Data.h"
#include "ReadData.h"


TW::Data TW::readBytes(const TW::Data &from, int max, int initial_pos) {
    if (from.size() - initial_pos < max) {
        throw std::invalid_argument("Data::Cannot read enough bytes!");
    }
    return TW::Data(from.begin() + initial_pos, from.begin() + initial_pos + max);
}

TW::Data TW::readVarBytes(const Data &from, int initial_pos) {
    int32_t size = readNumber<int32_t>(from, initial_pos);
    return readBytes(from, size, initial_pos + 4);
}

template<> uint64_t TW::readVar(const TW::Data &from, int initial_pos, const uint64_t &max) {
    byte fb = from[initial_pos];
    uint64_t value;
    if (fb == 0xFD) {
        value = readNumber<uint16_t>(from, initial_pos + 1);
    } else if (fb == 0xFE) {
        value = readNumber<uint32_t>(from, initial_pos + 1);
    } else if (fb == 0xFF) {
        value = readNumber<uint64_t>(from, initial_pos + 1);
    } else {
        value = fb;
    }
    if (value > max) {
        // std::cout << "TOO HUGE VALUE: " << value << " max=" << max << std::endl;
        throw std::invalid_argument("ReadData::ReadVarInt error: Too huge value! FormatException");
        return -1;
    }
    return value;
}

template<> int64_t TW::readVar(const TW::Data &from, int initial_pos, const int64_t &max) {
    return (int64_t) readVar<uint64_t>(from, initial_pos, uint64_t(max));
}

template<> uint16_t TW::readNumber(const TW::Data &from, int initial_pos) {
    TW::Data bytes = readBytes(from, 2, initial_pos);
    return uint16_t(bytes[0] | (bytes[1] << 8));
}

template<> uint32_t TW::readNumber(const TW::Data &from, int initial_pos) {
    TW::Data bytes = readBytes(from, 4, initial_pos);
    uint32_t val = bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24);
    return val;
}

template<> uint64_t TW::readNumber(const TW::Data &from, int initial_pos) {
    TW::Data bytes = readBytes(from, 8, initial_pos);
    uint32_t val1 = bytes[0] | (bytes[1]<<8) | (bytes[2]<<16) | (bytes[3]<<24);
    uint64_t val2 = bytes[4] | (bytes[5]<<8) | (bytes[6]<<16) | (bytes[7]<<24);
    uint64_t val = val1 | val2 << 32;
    return val;
}

template<> int16_t TW::readNumber(const TW::Data &from, int initial_pos) {
    return (int16_t) readNumber<uint16_t>(from, initial_pos);
}

template<> int32_t TW::readNumber(const TW::Data &from, int initial_pos) {
    return (int32_t) readNumber<uint32_t>(from, initial_pos);
}

template<> int64_t TW::readNumber(const TW::Data &from, int initial_pos) {
    return (int64_t) readNumber<uint64_t>(from, initial_pos);
}


TW::Data TW::writeVarBytes(const Data &from, int initial_pos) {
    return concat(write<int32_t>(int32_t(from.size()) - initial_pos), TW::Data(from.begin() + initial_pos, from.end()));
}

template<> TW::Data TW::write(const uint16_t &v) {
    return TW::Data({(byte) (((v >> 0) << 8) >> 8), (byte) (((v >> 8) << 8) >> 8)});
}

template<> TW::Data TW::write(const uint32_t &v) {
    return Data({
        (byte) (((v >> 0) << 24) >> 24),
        (byte) (((v >> 8) << 24) >> 24),
        (byte) (((v >> 16) << 24) >> 24),
        (byte) (((v >> 24) << 24) >> 24),
    });
}

template<> TW::Data TW::write(const uint64_t &v) {
    return Data({
        (byte) (((v >> 0) << 56) >> 56),
        (byte) (((v >> 8) << 56) >> 56),
        (byte) (((v >> 16) << 56) >> 56),
        (byte) (((v >> 24) << 56) >> 56),
        (byte) (((v >> 32) << 56) >> 56),
        (byte) (((v >> 40) << 56) >> 56),
        (byte) (((v >> 48) << 56) >> 56),
        (byte) (((v >> 56) << 56) >> 56)
    });
}

template<> TW::Data TW::write(const int16_t &v) {
    return write(uint16_t(v));
}

template<> TW::Data TW::write(const int32_t &v) {
    return write(uint32_t(v));
}

template<> TW::Data TW::write(const int64_t &v) {
    return write(uint64_t(v));
}


template<> TW::Data TW::writeVar(const uint64_t & value) {
    if (value < 0) {
        throw std::invalid_argument("ReadData::WriteVarInt ArgumentOutOfRangeException");
    }
    if (value < 0xFD) {
        return Data({(byte) value});
    } else if (value <= 0xFFFF) {
        return concat(Data({(byte) 0xFD}), write((uint16_t) value));
    } else if (value <= 0xFFFFFFFF) {
        return concat(Data({(byte) 0xFE}), write((uint32_t) value));
    } else {
        return concat(Data({(byte) 0xFF}), write(value));
    }
}

template<> TW::Data TW::writeVar(const int64_t & value) {
    return writeVar((uint64_t) value);
}