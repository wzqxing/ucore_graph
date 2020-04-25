#include <gui.h>
#include <compositor.h>
#include <draw.h>
#include <font.h>
#include <bmp.h>
#include <blend.h>
#include <vesa.h>
#include <memlayout.h>
#include <pmm.h>

void gui_init() {
    map_real_mode_1M();
    vesa_init();    
    mouse_init();
    compositor_init();
    window_t * red_w = window_create(get_super_window(), 20, 300, 750, 450, WINDOW_NORMAL, "window_black");
    window_add_title_bar(red_w);
    window_add_title_bar(red_w);
    window_add_close_button(red_w);
    window_add_minimize_button(red_w);
    window_add_maximize_button(red_w);
    canvas_t canvas_red = canvas_create(red_w->width, red_w->height, red_w->frame_buffer);
    set_font_color(VESA_COLOR_BLACK + 1);
    draw_text(&canvas_red, "Terminal", 1, 42);
    window_add_round_corner(red_w);
    blend_windows(red_w);
    unmap_real_mode_1M();
    cprintf("gui init successed!\n");
}
