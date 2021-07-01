#pragma once

#include "Events/Event.h"
#include "Elements/Element.h"

class WindowResizeEvent : public Event
{
public:
	WindowResizeEvent(float width, float height)
		: m_Width(width), m_Height(height) {}

	inline float GetWidth() const { return m_Width; }
	inline float GetHeight() const { return m_Height; }

	std::string ToString() const override
	{
		std::stringstream ss;
		ss << "WindowResizedEvent: " << m_Width << "," << m_Height;
		return ss.str();
	}

	EVENT_CLASS_TYPE(WindowResize)
	EVENT_CLASS_CATEGORY(EventCategoryApp)
private:
	float m_Width;
	float m_Height;
};

class WindowCloseEvent : public Event
{
public:
	WindowCloseEvent() {}

	EVENT_CLASS_TYPE(WindowClose)
	EVENT_CLASS_CATEGORY(EventCategoryApp)
};

class ContentsChangeEvent : public Event
{
public:
	ContentsChangeEvent(Ref<std::vector<Ref<Element>>> elements, bool append = false)
		: m_Elements(elements), m_Append(append) {}

	inline Ref<std::vector<Ref<Element>>> GetElements() const { return m_Elements; }
	inline bool Append() const { return m_Append; }

	EVENT_CLASS_TYPE(ContentsChange)
	EVENT_CLASS_CATEGORY(EventCategoryApp)
private:
	Ref<std::vector<Ref<Element>>> m_Elements;
	bool m_Append;
};
