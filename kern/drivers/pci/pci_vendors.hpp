#ifndef PCI_VENDORS_HPP
#define PCI_VENDORS_HPP

#include <stdint.h>

#include <kstring/kstrfwd.hpp>

namespace pci
{

kpp::string class_code_string(uint16_t base_class, uint16_t sub_class, uint16_t prog_if);

kpp::string dev_string(uint16_t ven_id, uint16_t dev_id);

kpp::string vendor_string(uint16_t ven_id);

}

#endif
