#ifndef FILE_HPP
#define FILE_HPP

#include <cassert>
#include <cstddef>
#include <type_traits>

class ReadRAMFile
{
    typedef std::make_signed<size_t>::type offset_t;

    public:
        ReadRAMFile() = default;
        ReadRAMFile(const void* const lpBuffer, const size_t bufferSize);
        ReadRAMFile(const ReadRAMFile&) = default;
        ReadRAMFile(ReadRAMFile&&) = default;
        ~ReadRAMFile() = default;

        ReadRAMFile& operator=(const ReadRAMFile&) = default;
        ReadRAMFile& operator=(ReadRAMFile&&) = default;

        void ClearBuffer();
        void SetBuffer(const void* const lpBuffer, const size_t bufferSize);

        const void* GetBuffer() const;
        size_t GetBufferSize() const;
        size_t GetCursor() const;

        bool CanRead(size_t bytes) const;

        bool IsGoodPosition(const size_t offset) const;
        bool IsGoodOffset(const offset_t offset) const;
        bool IsEOF() const;

        bool SetCursor(const size_t newPos);
        void ForceSetCursor(const size_t newPos);

        bool MoveCursor(const offset_t offset);
        void ForceMoveCursor(const offset_t offset);

        template<typename T>
        const T& ForceRead(const bool moveCursor = true);

        template<typename T, std::size_t N>
        const T (&ForceRead(const bool moveCursor = true))[N];

        template<typename T>
        bool Read(T& val, const bool moveCursor = true)
        {
            if (!CanRead(sizeof(T))) return false;

            val = *reinterpret_cast<const T*>(m_lpData + m_curPos);
            if (moveCursor) m_curPos += sizeof(T);

            return true;
        }

        template<typename T, typename T2>
        bool ReadAs(T2& val, const bool moveCursor = true)
        {
            if (!CanRead(sizeof(T))) return false;

            val = *reinterpret_cast<const T*>(m_lpData + m_curPos);
            if (moveCursor) m_curPos += sizeof(T);

            return true;
        }

        template<typename T>
        bool Read(T* val, std::size_t N, const bool moveCursor = true)
        {
            if (!CanRead(sizeof(T)*N)) return false;

            for (std::size_t i = 0; i < N; ++i)
                val[i] = reinterpret_cast<const T*>(m_lpData + m_curPos)[i];

            if (moveCursor) m_curPos += sizeof(T)*N;

            return true;
        }
        template<typename T, std::size_t N> bool Read(T (&val)[N], const bool moveCursor = true) { return Read(val, N, moveCursor); }

        template<typename T, typename T2>
        bool ReadAs(T2* val, std::size_t N, const bool moveCursor = true)
        {
            if (!CanRead(sizeof(T)*N)) return false;

            for (std::size_t i = 0; i < N; ++i)
                val[i] = reinterpret_cast<const T*>(m_lpData + m_curPos)[i];

            if (moveCursor) m_curPos += sizeof(T)*N;

            return true;
        }
        template<typename T, typename T2, std::size_t N>
        bool ReadAs(T2 (&val)[N], const bool moveCursor = true) { return ReadAs<T>(val, N, moveCursor); }
    private:
        const uint8_t* m_lpData = nullptr;
        size_t m_dataSize = 0;
        size_t m_curPos = 0;
};

inline ReadRAMFile::ReadRAMFile(const void* const lpBuffer, const size_t bufferSize) :
    m_lpData(reinterpret_cast<const uint8_t*>(lpBuffer)),
    m_dataSize(bufferSize) {}

inline void ReadRAMFile::ClearBuffer()
{
    m_lpData = nullptr;
    m_dataSize = 0;
    m_curPos = 0;
}
inline void ReadRAMFile::SetBuffer(const void* const lpBuffer, const size_t bufferSize)
{
    m_lpData = reinterpret_cast<const uint8_t*>(lpBuffer);
    m_dataSize = bufferSize;
    m_curPos = 0;
}

inline const void* ReadRAMFile::GetBuffer() const { return m_lpData; }
inline size_t ReadRAMFile::GetBufferSize() const { return m_dataSize; }
inline size_t ReadRAMFile::GetCursor() const { return m_curPos; }

inline bool ReadRAMFile::CanRead(size_t bytes) const { return (m_curPos + bytes <= m_dataSize); }

inline bool ReadRAMFile::IsGoodPosition(const size_t offset) const { return offset <= m_dataSize; }
inline bool ReadRAMFile::IsGoodOffset(const offset_t offset) const
{
    if (offset > 0)
        return IsGoodPosition(m_curPos + offset);

    return (static_cast<size_t>(-offset) <= m_curPos);
}
inline bool ReadRAMFile::IsEOF() const { return (m_curPos >= m_dataSize); }

inline bool ReadRAMFile::SetCursor(const size_t newPos)
{
    if (!IsGoodPosition(newPos))
        return false;

    m_curPos = newPos;
    return true;
}
inline void ReadRAMFile::ForceSetCursor(const size_t newPos) { assert(IsGoodPosition(newPos)); m_curPos = newPos; }

inline bool ReadRAMFile::MoveCursor(const offset_t offset)
{
    if (!IsGoodOffset(offset))
        return false;

    m_curPos += offset;
    return true;
}
inline void ReadRAMFile::ForceMoveCursor(const offset_t offset) { assert(IsGoodOffset(offset)); m_curPos += offset; }

template<typename T>
inline const T& ReadRAMFile::ForceRead(const bool moveCursor)
{
    assert(CanRead(sizeof(T)));
    register const T& tmp = *reinterpret_cast<const T*>(m_lpData + m_curPos);
    if (moveCursor)
        m_curPos += sizeof(T);
    return tmp;
}

template<typename T, std::size_t N>
inline const T (&ReadRAMFile::ForceRead(const bool moveCursor))[N]
{
    assert(CanRead(sizeof(T) * N));
    register const T (&tmp)[N] = *reinterpret_cast<const T(*)[N]>(m_lpData + m_curPos);
    if (moveCursor)
        m_curPos += sizeof(T) * N;
    return tmp;
}

#endif // FILE_HPP
