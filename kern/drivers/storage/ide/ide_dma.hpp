/*
ide_dma.hpp

Copyright (c) 23 Yann BOUCHER (yann)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
#ifndef IDE_DMA_HPP
#define IDE_DMA_HPP

#include <kstring/kstring_view.hpp>

#include "drivers/storage/disk.hpp"
#include "drivers/pci/pcidriver.hpp"

#include "ide_common.hpp"

#include "i686/interrupts/isr.hpp"

namespace ide::dma
{

class Controller : public PciDriver
{
    friend class Disk;

public:
    static bool accept(const pci::PciDevice& dev);

    virtual void init() override;

    virtual kpp::string_view driver_name() const override { return "PCI IDE Controller"; }
    virtual DriverType  type() const override { return DriverType::IDEController; }

private:
    ata_device mk_dev(BusPort bus, DriveType type)
    { return {bus, type, io_base(bus), control_io_base(bus)}; }

    bool int14_handler(const registers* regs);
    bool int15_handler(const registers* regs);
    bool common_handler(const ata_device& dev);

    std::vector<std::pair<uint16_t, uint8_t> > scan();

    void send_command(const ata_device& dev, uint8_t command, bool read, size_t block, size_t count, gsl::span<const uint8_t> data);
    
    uint16_t io_base(BusPort bus);
    uint16_t control_io_base(BusPort bus);
    
    void prepare_prdt(BusPort bus, gsl::span<const uint8_t> data);
    
    uint8_t status_byte(BusPort bus);
    void send_status_byte(BusPort bus, uint8_t val);
    void send_command_byte(BusPort bus, uint8_t val);
    void send_prdt(BusPort bus);

private:
    bool m_primary_compatibility { false };
    bool m_secondary_compatibility { false };
};

class Disk : public IDEDisk
{
public:
    Disk(Controller& controller, BusPort port, DriveType type);

    template <typename... Args>
    static Disk& create_disk(Args&&... args)
    {
        return DiskImpl<Disk>::create_disk(std::forward<Args>(args)...);
    }

protected:
    [[nodiscard]]
    virtual kpp::expected<MemBuffer, DiskError> read_sector(size_t sector, size_t count) const override;
    [[nodiscard]]
    virtual kpp::expected<kpp::dummy_t, DiskError> write_sector(size_t sector, gsl::span<const uint8_t> data) override;

private:
    enum class RWAction
    {
        Read,
        Write
    };
    [[nodiscard]]
    kpp::expected<kpp::dummy_t, DiskError> do_read_write(size_t sector, gsl::span<const uint8_t> data, RWAction action) const;

private:
    Controller& m_cont;
};

}

#endif // IDE_DMA_HPP
