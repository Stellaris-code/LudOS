/*
gfxcommands.cpp

Copyright (c) 30 Yann BOUCHER (yann)

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

#include "gfxcommands.hpp"

#include "shell/shell.hpp"
#include "fs/vfs.hpp"
#include "graphics/drawing/image_loader.hpp"
#include "graphics/video.hpp"
#include "graphics/drawing/display_draw.hpp"
#include "graphics/text/graphicterm.hpp"
#include "graphics/fonts/font.hpp"
#include "graphics/fonts/psf.hpp"
#include "terminal/terminal.hpp"
#include "utils/messagebus.hpp"
#include "utils/nop.hpp"
#include "utils/stlutils.hpp"
#include "utils/crc32.hpp"
#include "drivers/kbd/kbd_mappings.hpp"

void install_gfx_commands(Shell &sh)
{
    sh.register_command(
    {"display", "Display a specified image",
     "Usage : 'display <file>'",
     [&sh](const std::vector<std::string>& args)
     {
         if (args.size() != 1)
         {
             sh.error("display needs one argument\n");
             return -1;
         }

         auto img = graphics::load_image(sh.get_path(args[0]));
         if (!img)
         {
             sh.error("Can't load '%s'\n", args[0].c_str());
             return -2;
         }

         img->resize(graphics::screen()->width(), graphics::screen()->height());

         term().disable();
         graphics::screen()->blit(*img, {0, 0});
         graphics::draw_to_display(*graphics::screen());

         bool escape { false };

         auto handl = MessageBus::register_handler<kbd::KeyEvent>([&escape](const kbd::KeyEvent& e)
         {
             if (e.state == kbd::KeyEvent::Pressed && e.key == kbd::Escape)
             {
                 escape = true;
             }
         });

         while (!escape)
         {
             nop();
         }

         MessageBus::remove_handler(handl);

         graphics::clear_display(graphics::color_black);

         term().enable();
         term().force_redraw();

         return 0;
     }});

    sh.register_command(
    {"setbckg", "Use a certain image as a terminal background",
     "Usage : 'setbckg <file>'",
     [&sh](const std::vector<std::string>& args)
     {
         if (args.size() != 1)
         {
             sh.error("setbckg needs one argument\n");
             return -1;
         }

         auto img = graphics::load_image(sh.get_path(args[0]));
         if (!img)
         {
             sh.error("Can't load '%s'\n", args[0].c_str());
             return -2;
         }

         if (!MessageBus::send<SetBackgroundMessage>({*img}))
         {
             // Nobody received the message
             sh.error("Current terminal doesn't support setting background image !\n");
         }

         return 0;
     }});

    sh.register_command(
    {"setmode", "Set a graphical mode",
     "Usage : 'setmode <width>x<height>(x<depth>)'",
     [&sh](const std::vector<std::string>& args)
     {
         if (args.size() != 1)
         {
             sh.error("setmode needs one argument\n");
             return -1;
         }

         auto toks = tokenize(args[0], "x");
         if (toks.size() == 2)
         {
             toks.emplace_back("32");
         }
         if (toks.size() != 3)
         {
             sh.error("invalid argument\n");
             return -2;
         }

         if (!graphics::change_mode(std::stoul(toks[0]), std::stoul(toks[1]), std::stoul(toks[2])))
         {
             sh.error("Can't set mode %s\n", args[0].c_str());
             return -2;
         }

         graphics::clear_display(graphics::color_black);

         create_term<graphics::GraphicTerm>(*graphics::screen(), term_data());

         return 0;
     }});

    sh.register_command(
    {"listmodes", "List available video modes",
     "Usage : 'listmodes'",
     [](const std::vector<std::string>&)
     {
         for (const auto& mode : graphics::list_video_modes())
         {
             kprintf("\t%dx%dx%d\n", mode.width, mode.height, mode.depth);
         }
         return 0;
     }});

    sh.register_command(
    {"setfont", "Set terminal font",
     "Usage : 'setmode <font>'",
     [&sh](const std::vector<std::string>& args)
     {
         if (args.size() != 1)
         {
             sh.error("setmode needs one argument\n");
             return -1;
         }
         static std::vector<graphics::psf::PSFFont> fonts;
         fonts.emplace_back();
         if (!fonts.back().load(sh.get_path(args[0])))
         {
             sh.error("can't load %s\n", args[0].c_str());
             return -2;
         }

         graphics::clear_display(graphics::color_black);
         create_term<graphics::GraphicTerm>(*graphics::screen(), term_data(), fonts.back());

         return 0;
     }});
}
