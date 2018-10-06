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

#include "utils/kmsgbus.hpp"

#include "shell/shell.hpp"
#include "fs/vfs.hpp"
#include "graphics/drawing/image_loader.hpp"
#include "graphics/video.hpp"
#include "graphics/drawing/display_draw.hpp"
#include "graphics/text/graphicterm.hpp"
#include "graphics/fonts/font.hpp"
#include "graphics/fonts/psf.hpp"
#include "terminal/terminal.hpp"

#include "time/time.hpp"

#include "utils/nop.hpp"
#include "utils/stlutils.hpp"
#include "utils/crc32.hpp"
#include "drivers/kbd/kbd_mappings.hpp"

void console_perf_test()
{
   static const char letters[] =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

   const int iters = 10;
   char *buf;

   size_t width = term().width();
   size_t height = term().height();

   buf = (char*)kmalloc(width * height);

   if (!buf) {
      kprintf("Out of memory\n");
      return;
   }

   for (int i = 0; i < width * height; i++) {
      buf[i] = letters[i % (sizeof(letters) - 1)];
   }
   buf[width * height - 1] = '\n';

   uint64_t start = Time::total_ticks();

   for (int i = 0; i < iters; i++) {
      term().write(buf, width * height);
   }

   uint64_t end = Time::total_ticks();
   unsigned long long c = (end - start) / iters;

   kprintf("Term size: %d rows x %d cols\n", width, height);
   kprintf("Screen redraw:       %10llu cycles\n", c);
   kprintf("Avg. character cost: %10llu cycles\n", c / (width * height));
   kfree(buf);
}

void install_gfx_commands(Shell &sh)
{
    sh.register_command(
    {"display", "Display a specified image",
     "Usage : 'display <file>'",
     [&sh](const std::vector<kpp::string>& args)
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

         volatile bool escape { false };

         auto handl = kmsgbus.register_handler<kbd::KeyEvent>([&escape](const kbd::KeyEvent& e)
         {
             if (e.state == kbd::KeyEvent::Pressed && e.key == KeyEscape)
             {
                 escape = true;
             }
         });

         while (!escape)
         {
             nop();
         }

         kmsgbus.remove_handler(handl);

         graphics::clear_display(graphics::color_black);

         term().enable();
         //term().force_redraw();

         return 0;
     }});

    sh.register_command(
    {"setmode", "Set a graphical mode",
     "Usage : 'setmode <width>x<height>(x<depth>)'",
     [&sh](const std::vector<kpp::string>& args)
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

         if (!graphics::change_mode(kpp::stoul(toks[0].to_string()), kpp::stoul(toks[1].to_string()), kpp::stoul(toks[2].to_string())))
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
     [](const std::vector<kpp::string>&)
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
     [&sh](const std::vector<kpp::string>& args)
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

    sh.register_command(
    {"drawtest", "tests fb performance",
     "drawtest",
     [](const std::vector<kpp::string>&)
     {
         const size_t iters = 1024;

         graphics::Screen screen_blue(graphics::current_video_mode().width, graphics::current_video_mode().height);
         memsetl(screen_blue.data(), graphics::color_blue.rgb(), screen_blue.width()*screen_blue.height()*4);

         graphics::Screen screen_red(graphics::current_video_mode().width, graphics::current_video_mode().height);
         memsetl(screen_red.data(), graphics::color_red.rgb(), screen_red.width()*screen_red.height()*4);

         term().disable();

         uint64_t start_ticks = Time::total_ticks();
         for (size_t i { 0 }; i < iters; ++i)
         {
             graphics::draw_to_display(i % 2 ? screen_blue : screen_red);
         }
         uint64_t delta = (Time::total_ticks() - start_ticks) / iters;

         kprintf("---- clocks per pixel : %llu\n", delta/(graphics::current_video_mode().height*
         graphics::current_video_mode().bytes_per_line));
         kprintf("---- clocks per 32 pixels : %llu\n", delta*32/(graphics::current_video_mode().height*
         graphics::current_video_mode().bytes_per_line));

         term().enable();
         term().force_redraw();
         return 0;
     }});

    sh.register_command(
    {"scrolltest", "tests scroll",
     "scrolltest",
     [](const std::vector<kpp::string>&)
     {
         const size_t iters = 1024;

         uint64_t start_ticks = Time::total_ticks();
         for (size_t i { 0 }; i < iters; ++i)
         {
             kprintf("Line %d\n", i);
         }
         uint64_t delta = (Time::total_ticks() - start_ticks) / iters;

         kprintf("---- clocks per line : %llu\n", delta);

         return 0;
     }});

    sh.register_command(
    {"termtest", "tests term",
     "termtest",
     [](const std::vector<kpp::string>&)
     {
         console_perf_test();

         return 0;
     }});
}
