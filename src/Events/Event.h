#pragma once
#include "pch.h"

#include "Helpers/Utils.h"

enum class EventType
{
    None = 0,
    WindowClose,
    WindowResize,
    ContentsChange,
    DirectoryChange,
    KeyPressed,
    KeyReleased,
    KeyTyped,
    MouseButtonPressed,
    MouseButtonReleased,
    MouseMoved,
    MouseScrolled,
};

enum EventCategory
{
    None = 0,
    EventCategoryApp = BIT(0),
    EventCategoryInput = BIT(1),
    EventCategoryKeyboard = BIT(2),
    EventCategoryMouse = BIT(3),
    EventCategoryMouseButton = BIT(4),
};

#define EVENT_CLASS_TYPE(type) \
    inline static EventType GetStaticType() \
    { \
        return EventType::type; \
    } \
    inline virtual EventType GetEventType() const override \
    { \
        return GetStaticType(); \
    } \
    inline virtual const char *GetName() const override \
    { \
        return #type; \
    }

#define EVENT_CLASS_CATEGORY(category) \
    inline virtual int GetCategoryFlags() const override \
    { \
        return category; \
    }

#define BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

class Event
{
    friend class EventDispatcher;

public:
    virtual ~Event() = default;

    virtual EventType GetEventType() const = 0;
    virtual const char *GetName() const = 0;
    virtual int GetCategoryFlags() const = 0;

    inline bool IsInCategory(EventCategory category) { return GetCategoryFlags() & category; }

    inline bool IsHandled() const { return m_Handled; }

private:
    bool m_Handled = false;
};

class EventDispatcher
{
    template<typename T>
    using EventFn = std::function<bool(T &)>;

public:
    EventDispatcher(Event &event)
        : m_Event(event) {}

    template<typename T>
    bool Dispatch(EventFn<T> func)
    {
        if (m_Event.GetEventType() == T::GetStaticType())
        {
            m_Event.m_Handled = func(*reinterpret_cast<T *>(&m_Event));
            return true;
        }
        return false;
    }

private:
    Event &m_Event;
};
