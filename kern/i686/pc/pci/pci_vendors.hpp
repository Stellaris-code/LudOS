 
#include "../external/pci/pci_vendors.h"

#include <string.hpp>

inline std::string class_code_string(uint16_t base_class, uint16_t sub_class, uint16_t prog_if)
{
    for (const auto& el : PciClassCodeTable)
    {
        if (el.BaseClass == base_class && el.SubClass == sub_class && el.ProgIf == prog_if)
        {
            return std::string(el.BaseDesc) + ":" + el.SubDesc + ":" + el.ProgDesc;
        }
    }

    return "Unknown";
}

inline std::string dev_string(uint16_t ven_id, uint16_t dev_id)
{
    for (const auto& el : PciDevTable)
    {
        if (el.VenId == ven_id && el.DevId == dev_id)
        {
            return std::string(el.Chip) + " " + el.ChipDesc;
        }
    }

    return "Unknown";
}

inline std::string vendor_string(uint16_t ven_id)
{
    for (const auto& el : PciVenTable)
    {
        if (el.VenId == ven_id)
        {
            return std::string(el.VenFull) + "(" + el.VenShort + ")";
        }
    }

    return "Unknown";
}
