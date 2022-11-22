#pragma once

#include "Elements/Element.h"

class Xbox : public Element
{
public:
    Xbox(const std::string &label, const std::string &ipAddress);

    virtual void OnClick() override;

    inline const std::string &GetIpAddress() const { return m_IpAddress; }

private:
    std::string m_IpAddress;
    std::string m_ActiveTitle;
    std::string m_ConsoleType;
    bool m_ShowPropertiesWindow = false;

    bool FetchConsoleInfo();

    void DisplayProperties();

    virtual void DisplayContextMenu() override;
};
