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
};
