#ifndef PCI_VENDORS_HPP
#define PCI_VENDORS_HPP

#include <string.hpp>

namespace pci
{

std::string class_code_string(uint16_t base_class, uint16_t sub_class, uint16_t prog_if);

std::string dev_string(uint16_t ven_id, uint16_t dev_id);

std::string vendor_string(uint16_t ven_id);

}

#endif
