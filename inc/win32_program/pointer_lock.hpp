#pragma once

#include <cstdint>
#include <functional>
#include <windows.h>

namespace win32_program {

    enum class PointerType : uint8_t {
        Mouse,
        Touch,
        Pen,

        Count
    };

    enum class PointerBehaviour
    {
        None = -1,
        Panning,
        Painting
    };

    struct Pointer
    {
        PointerType Type = PointerType::Mouse;
        uint8_t Button = 0;
        uint32_t Id = 0;

        bool operator==(const Pointer& other) const = default;
    };

    struct PointerMapKey
    {
        PointerType Type;
        uint8_t Button;

        bool operator==(const PointerMapKey& other) const = default;
        bool operator==(const Pointer& other) const
        {
            return this->Type == other.Type && this->Button == other.Button;
        };
        PointerMapKey& operator=(const Pointer& other)
        {
            Type = other.Type;
            Button = other.Button;
            return *this;
        }
    };

    inline std::vector<Pointer> BuildPointersDown(PointerType type, uint32_t id, uint32_t flags)
    {
        std::vector<Pointer> pointers;

        if(flags & POINTER_FLAG_FIRSTBUTTON)
            pointers.push_back({ type, 0, id });

        if(flags & POINTER_FLAG_SECONDBUTTON)
            pointers.push_back({ type, 1, id });

        if(flags & POINTER_FLAG_THIRDBUTTON)
            pointers.push_back({ type, 2, id });

        if(flags & POINTER_FLAG_FOURTHBUTTON)
            pointers.push_back({ type, 3, id });

        if(flags & POINTER_FLAG_FIFTHBUTTON)
            pointers.push_back({ type, 4, id });

        return pointers;
    }

    inline std::vector<Pointer> BuildPointersUp(PointerType type, uint32_t id, uint32_t flags)
    {
        std::vector<Pointer> pointers;

        if(!(flags & POINTER_FLAG_FIRSTBUTTON))
            pointers.push_back({ type, 0, id });

        if(!(flags & POINTER_FLAG_SECONDBUTTON))
            pointers.push_back({ type, 1, id });

        if(!(flags & POINTER_FLAG_THIRDBUTTON))
            pointers.push_back({ type, 2, id });

        if(!(flags & POINTER_FLAG_FOURTHBUTTON))
            pointers.push_back({ type, 3, id });

        if(!(flags & POINTER_FLAG_FIFTHBUTTON))
            pointers.push_back({ type, 4, id });

        return pointers;
    }

    inline std::vector<Pointer> BuildAllPointers(PointerType type, uint32_t id)
    {
        std::vector<Pointer> pointers;

        for(uint8_t i=0; i<5; i++)
            pointers.push_back({ type, i, id });

        return pointers;
    }

    class PointerLock 
    {
        private:
            Pointer m_pointer;
            bool m_isLocked = false;

        public:
            bool IsLocked() const;
            bool IsLockedBy(Pointer pointer) const;

            PointerType GetPointerType() const;
            uint32_t GetPointerId() const;

            bool Acquire(Pointer pointer);
            bool Release(Pointer pointer);
    };
}

template <>
struct std::hash<win32_program::PointerMapKey>
{
    std::size_t operator()(const win32_program::PointerMapKey& k) const
    {
        return (static_cast<std::size_t>(k.Type) << 32) ^ static_cast<std::size_t>(k.Button);
    }
};