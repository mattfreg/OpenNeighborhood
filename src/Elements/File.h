#pragma once

#include <XBDM.h>

#include "Elements/Element.h"

class File : public Element
{
public:
    File(const XBDM::File &data);

    virtual void OnClick() override;

private:
    XBDM::File m_Data;
    bool m_ShowPropertiesWindow = false;

    void OpenDirectory();
    void LaunchXex();
    void Download();
    void Delete();
    void Rename();
    void UpdateContents();

    void DisplayProperties();

    virtual void DisplayContextMenu() override;
};
