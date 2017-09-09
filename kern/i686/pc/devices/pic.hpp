/*
pic.hpp

Copyright (c) 26 Yann BOUCHER (yann)

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
#ifndef PIC_HPP
#define PIC_HPP

#include <stdint.h>

#define PIC_MASTER_CMD 0x20
#define PIC_MASTER_DATA 0x21
#define PIC_SLAVE_CMD 0xA0
#define PIC_SLAVE_DATA 0xA1

#define PIC_CMD_EOI 0x20

//-----------------------------------------------
// Controller Registers
//-----------------------------------------------

// PIC 1 register port addresses
#define PIC1_REG_COMMAND 0x20
#define PIC1_REG_STATUS 0x20
#define PIC1_REG_DATA 0x21
#define PIC1_REG_IMR 0x21

// PIC 2 register port addresses
#define PIC2_REG_COMMAND 0xA0
#define PIC2_REG_STATUS 0xA0
#define PIC2_REG_DATA 0xA1
#define PIC2_REG_IMR 0xA1

//-----------------------------------------------
// Initialization Command Bit Masks
//-----------------------------------------------

// Initialization Control Word 1 bit masks
#define PIC_ICW1_MASK_IC4 0x1 //00000001
#define PIC_ICW1_MASK_SNGL 0x2 //00000010
#define PIC_ICW1_MASK_ADI 0x4 //00000100
#define PIC_ICW1_MASK_LTIM 0x8 //00001000
#define PIC_ICW1_MASK_INIT 0x10 //00010000

// Initialization Control Words 2 and 3 do not require bit masks

// Initialization Control Word 4 bit masks
#define PIC_ICW4_MASK_UPM 0x1 //00000001
#define PIC_ICW4_MASK_AEOI 0x2 //00000010
#define PIC_ICW4_MASK_MS 0x4 //00000100
#define PIC_ICW4_MASK_BUF 0x8 //00001000
#define PIC_ICW4_MASK_SFNM 0x10 //00010000

//-----------------------------------------------
// Initialization Command 1 control bits
//-----------------------------------------------

#define PIC_ICW1_IC4_EXPECT 1 //1
#define PIC_ICW1_IC4_NO 0 //0
#define PIC_ICW1_SNGL_YES 2 //10
#define PIC_ICW1_SNGL_NO 0 //00
#define PIC_ICW1_ADI_CALLINTERVAL4 4 //100
#define PIC_ICW1_ADI_CALLINTERVAL8 0 //000
#define PIC_ICW1_LTIM_LEVELTRIGGERED 8 //1000
#define PIC_ICW1_LTIM_EDGETRIGGERED 0 //0000
#define PIC_ICW1_INIT_YES 0x10 //10000
#define PIC_ICW1_INIT_NO 0 //00000

//-----------------------------------------------
// Initialization Command 4 control bits
//-----------------------------------------------

#define PIC_ICW4_UPM_86MODE 1 //1
#define PIC_ICW4_UPM_MCSMODE 0 //0
#define PIC_ICW4_AEOI_AUTOEOI 2 //10
#define PIC_ICW4_AEOI_NOAUTOEOI 0 //0
#define PIC_ICW4_MS_BUFFERMASTER 4 //100
#define PIC_ICW4_MS_BUFFERSLAVE 0 //0
#define PIC_ICW4_BUF_MODEYES 8 //1000
#define PIC_ICW4_BUF_MODENO 0 //0
#define PIC_ICW4_SFNM_NESTEDMODE 0x10 //10000
#define PIC_ICW4_SFNM_NOTNESTED 0 //a binary 2

namespace pic
{
void init();
void send_eoi(uint8_t irq);
void set_mask(uint8_t irq);
void clear_mask(uint8_t irq);
}

#endif // PIC_HPP
