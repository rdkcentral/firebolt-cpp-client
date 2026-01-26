#include "displayDemo.h"
#include <iostream>
#include <string>
#include <vector>

#include "json_types/jsondata_device_types.h"

#include "outputstream.h"
extern OutputStream gOutput;

using namespace Firebolt;
using namespace Firebolt::Display;

DisplayDemo::DisplayDemo()
    : IFireboltDemo()
{
    methodsFromRpc("Display");
}

void DisplayDemo::runOption(const int index)
{
    std::string key = names_[index];

    outStream_ << "Running Display method: " << key << std::endl;
    if (key == "Display.maxResolution")
    {
        Result<DisplaySize> r = Firebolt::IFireboltAccessor::Instance().DisplayInterface().maxResolution();
        if (validateResult(r))
        {
            DisplaySize res = r.value();
            gOutput << "Max Resolution - Width: " << res.width << ", Height: " << res.height << std::endl;
        }
    }
    else if (key == "Display.size")
    {
        Result<DisplaySize> r = Firebolt::IFireboltAccessor::Instance().DisplayInterface().size();
        if (validateResult(r))
        {
            DisplaySize size = r.value();
            gOutput << "Display Size - Width: " << size.width << ", Height: " << size.height << std::endl;
        }
    }
}
