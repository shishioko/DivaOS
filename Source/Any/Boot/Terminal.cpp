#include "Boot/Terminal.hpp"

namespace DivaOS::Boot::Terminal {
    namespace
    {
        /// @brief Requests a framebuffer from the Limine Bootloader
        static volatile limine_framebuffer_request FramebufferRequest = {
            .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
            .revision = 0
        };

        bool Initialized = false;

        limine_framebuffer* Framebuffer;
        flanterm_context* Context;

    }
    const bool Initialize(){
        if (Initialized) return true;
        if (FramebufferRequest.response == null || FramebufferRequest.response->framebuffer_count <= 0) {
            return false;
        }

        Framebuffer = FramebufferRequest.response->framebuffers[0];

        Context = flanterm_fb_init(
            null,
            null,
            (u32*)Framebuffer->address,
            Framebuffer->width,
            Framebuffer->height,
            Framebuffer->pitch,
            Framebuffer->red_mask_size,
            Framebuffer->red_mask_shift,
            Framebuffer->green_mask_size,
            Framebuffer->green_mask_shift,
            Framebuffer->blue_mask_size,
            Framebuffer->blue_mask_shift,
            null,
            null,
            null,
            null,
            null,
            null,
            null,
            null,
            0,
            0,
            0,
            1,
            1,
            0,
            0
        );

        Initialized = true;
        return true;
    }

    void Write(const t8 character){
        if (!Initialized) return;
        flanterm_write(Context, &character, sizeof(t8));
    }
    void Write(const t8* text){
        if (!Initialized) return;
        u64 length = 0;
        while (text[length] != '\0') {
            length++;
        }
        flanterm_write(Context, text, length);
    }
}