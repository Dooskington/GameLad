#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <type_traits>
#include <vector>

class StateSerializer
{
public:
    explicit StateSerializer(std::vector<byte>& output) :
        m_output(&output),
        m_input(nullptr),
        m_inputSize(0),
        m_offset(0),
        m_errorOffset(0),
        m_valid(true)
    {
        m_output->clear();
    }

    StateSerializer(const void* input, size_t inputSize) :
        m_output(nullptr),
        m_input(static_cast<const byte*>(input)),
        m_inputSize(inputSize),
        m_offset(0),
        m_errorOffset(0),
        m_valid(input != nullptr || inputSize == 0)
    {
    }

    bool IsReading() const { return m_output == nullptr; }
    bool IsValid() const { return m_valid; }
    size_t Size() const { return m_offset; }
    size_t ErrorOffset() const { return m_errorOffset; }
    size_t Remaining() const
    {
        return IsReading() && m_offset <= m_inputSize ? m_inputSize - m_offset : 0;
    }

    void Invalidate()
    {
        if (m_valid)
        {
            m_errorOffset = m_offset;
        }
        m_valid = false;
    }

    void Sync(byte& value)
    {
        SyncUnsigned(value, 1);
    }

    void Sync(sbyte& value)
    {
        byte encoded = static_cast<byte>(value);
        Sync(encoded);
        if (IsReading() && m_valid)
        {
            value = static_cast<sbyte>(encoded);
        }
    }

    void Sync(bool& value)
    {
        byte encoded = value ? 1 : 0;
        Sync(encoded);
        if (IsReading() && m_valid)
        {
            if (encoded > 1)
            {
                Invalidate();
                return;
            }
            value = encoded != 0;
        }
    }

    void Sync(ushort& value)
    {
        SyncUnsigned(value, 2);
    }

    void Sync(unsigned int& value)
    {
        SyncUnsigned(value, 4);
    }

    void Sync(int& value)
    {
        std::uint32_t encoded = static_cast<std::uint32_t>(value);
        SyncFixedUnsigned(encoded, 4);
        if (IsReading() && m_valid)
        {
            std::int32_t signedValue = 0;
            std::memcpy(&signedValue, &encoded, sizeof(signedValue));
            value = static_cast<int>(signedValue);
        }
    }

    void Sync(unsigned long& value)
    {
        std::uint64_t encoded = static_cast<std::uint64_t>(value);
        SyncFixedUnsigned(encoded, 8);
        if (IsReading() && m_valid)
        {
            if (encoded > static_cast<std::uint64_t>((std::numeric_limits<unsigned long>::max)()))
            {
                Invalidate();
                return;
            }
            value = static_cast<unsigned long>(encoded);
        }
    }

    void Sync(unsigned long long& value)
    {
        std::uint64_t encoded = static_cast<std::uint64_t>(value);
        SyncFixedUnsigned(encoded, 8);
        if (IsReading() && m_valid)
        {
            value = static_cast<unsigned long long>(encoded);
        }
    }

    void Sync(long long& value)
    {
        std::uint64_t encoded = 0;
        std::memcpy(&encoded, &value, sizeof(encoded));
        SyncFixedUnsigned(encoded, 8);
        if (IsReading() && m_valid)
        {
            std::memcpy(&value, &encoded, sizeof(value));
        }
    }

    void Sync(float& value)
    {
        std::uint32_t encoded = 0;
        if (!IsReading())
        {
            std::memcpy(&encoded, &value, sizeof(encoded));
        }
        SyncFixedUnsigned(encoded, 4);
        if (IsReading() && m_valid)
        {
            std::memcpy(&value, &encoded, sizeof(value));
        }
    }

    void Sync(double& value)
    {
        std::uint64_t encoded = 0;
        if (!IsReading())
        {
            std::memcpy(&encoded, &value, sizeof(encoded));
        }
        SyncFixedUnsigned(encoded, 8);
        if (IsReading() && m_valid)
        {
            std::memcpy(&value, &encoded, sizeof(value));
        }
    }

    template <typename Enum>
    void SyncEnum(Enum& value)
    {
        static_assert(std::is_enum<Enum>::value, "SyncEnum requires an enum type");
        unsigned int encoded = static_cast<unsigned int>(value);
        Sync(encoded);
        if (IsReading() && m_valid)
        {
            value = static_cast<Enum>(encoded);
        }
    }

    void SyncBytes(void* data, size_t size)
    {
        if (!m_valid || size == 0)
        {
            return;
        }

        if (IsReading())
        {
            if (data == nullptr || m_offset > m_inputSize || size > m_inputSize - m_offset)
            {
                Invalidate();
                return;
            }
            std::memcpy(data, m_input + m_offset, size);
        }
        else
        {
            if (data == nullptr)
            {
                Invalidate();
                return;
            }
            const byte* bytes = static_cast<const byte*>(data);
            m_output->insert(m_output->end(), bytes, bytes + size);
        }

        m_offset += size;
    }

private:
    template <typename T>
    void SyncUnsigned(T& value, size_t bytes)
    {
        std::uint64_t encoded = static_cast<std::uint64_t>(value);
        SyncFixedUnsigned(encoded, bytes);
        if (IsReading() && m_valid)
        {
            value = static_cast<T>(encoded);
        }
    }

    template <typename T>
    void SyncFixedUnsigned(T& value, size_t bytes)
    {
        static_assert(std::is_unsigned<T>::value, "State values must be unsigned");
        if (!m_valid || bytes > sizeof(T))
        {
            Invalidate();
            return;
        }

        if (IsReading())
        {
            if (m_offset > m_inputSize || bytes > m_inputSize - m_offset)
            {
                Invalidate();
                return;
            }

            value = 0;
            for (size_t index = 0; index < bytes; ++index)
            {
                value |= static_cast<T>(m_input[m_offset + index]) << (index * 8);
            }
        }
        else
        {
            for (size_t index = 0; index < bytes; ++index)
            {
                m_output->push_back(static_cast<byte>((value >> (index * 8)) & 0xFF));
            }
        }

        m_offset += bytes;
    }

    std::vector<byte>* m_output;
    const byte* m_input;
    size_t m_inputSize;
    size_t m_offset;
    size_t m_errorOffset;
    bool m_valid;
};
