#include <bmp.h>
#include <file.h>
#include <stat.h>
#include <unistd.h>
/*
 * Simmple bmp library used in kernel
 *
 * */

bmp_t * bmp_create(char * filename) {
    bmp_t * ret = kmalloc(sizeof(bmp_t));
    //qemu_printf("Opening %s\n", filename);
    int fd = file_open(filename, O_RDONLY);
    if(fd < 0) {
        cprintf("Fail to open %s\n", filename);
        assert(0);
        return NULL;
    }

    struct stat stat;
    if(file_fstat(fd, &stat) < 0) {
        cprintf("Fail to get file info %s fd %d\n", filename, fd);
        assert(0);
        return NULL;
    }
    uint32_t size = stat.st_size;
    void * buf = kmalloc(size);
    size_t copied;
    int page = 40960;
    int now = 0;
    while(now < size) {
        int count; 
        if (size - now > page) {
            count = page;
        } else {
            count = size - now;
        }
        file_read(fd, buf + now, count, &copied);
        //cprintf("read %d bytes, now %lu, size %lu\n", count, now, size);
        now += count;
    }
    assert(now == size);
    cprintf("%s file size %lu, copied %lu\n", filename, size, copied);


    // Parse the bmp
    bmp_fileheader_t * h = buf;
    unsigned int offset = h->bfOffBits;
    cprintf("bmp size: %u\n", h->bfSize);
    cprintf("bmp offset: %u\n", offset);

    bmp_infoheader_t * info = buf + sizeof(bmp_fileheader_t);

    ret->width = info->biWidth;
    ret->height = info->biHeight;
    ret->image_bytes= (void*)((unsigned int)buf + offset);
    ret->buf = buf;
    ret->total_size= size;
    ret->bpp = info->biBitCount;
    cprintf("bmp is %u x %u\n", ret->width, ret->height);
    //qemu_printf("file is here: %p\n", buf);
    //qemu_printf("image is here %p\n", ret->image_bytes);
    file_close(fd);
    return ret;
}



void bmp_display(bmp_t * bmp) {
    if(!bmp) return;
    uint8_t * image = (uint8_t *)bmp->image_bytes;
    uint32_t * screen = (uint32_t *)0xfd000000;
    int j = 0;
    // Do copy
    for(int i = 0; i < bmp->height; i++) {
        // Copy the ith row of image to height - 1 - i row of screen, each row is of length width * 3
        char * image_row = (char *)image + i * bmp->width * 3;
        uint32_t * screen_row = (void*)screen + (bmp->height - 1 - i) * bmp->width * 4;
        j = 0;
        for(int k = 0; k < bmp->width; k++) {
            uint32_t b = image_row[j++] & 0xff;
            uint32_t g = image_row[j++] & 0xff;
            uint32_t r = image_row[j++] & 0xff;
            uint32_t rgb = ((r << 16) | (g << 8) | (b)) & 0x00ffffff;
            rgb = rgb | 0xff000000;
            screen_row[k] = rgb;
        }
    }
}

/*
 * Copy bmp content to frame buffer
 * */
void bmp_to_framebuffer(bmp_t * bmp, uint32_t * frame_buffer) {
    if(!bmp) return;
    uint8_t * image = (uint8_t *)bmp->image_bytes;
    int j = 0;
    // Do copy
    for(int i = 0; i < bmp->height; i++) {
        // Copy the ith row of image to height - 1 - i row of frame buffer, each row is of length width * 3
        char * image_row = (char *)image + i * bmp->width * 3;
        //uint32_t * framebuffer_row = (void*)frame_buffer + (bmp->height - 1 - i) * bmp->width * 4;
        uint32_t * framebuffer_row = (void*)frame_buffer + (bmp->height - 1 - i) * bmp->width * 4;
        j = 0;
        for(int k = 0; k < bmp->width; k++) {
            uint32_t b = image_row[j++] & 0xff;
            uint32_t g = image_row[j++] & 0xff;
            uint32_t r = image_row[j++] & 0xff;
            uint32_t rgb = ((r << 16) | (g << 8) | (b)) & 0x00ffffff;
            rgb = rgb | 0xff000000;
            framebuffer_row[k] = rgb;
        }
    }
}
void bmp_to_framebuffer2(bmp_t * bmp, uint32_t * frame_buffer) {
    if(!bmp) return;
    uint8_t * image = (uint8_t *)bmp->image_bytes;
    int j = 0;
    // Do copy
    for(int i = 0; i < bmp->height; i++) {
        // Copy the ith row of image to height - 1 - i row of frame buffer, each row is of length width * 3
        char * image_row = (char *)image + i * bmp->width * 4;
        //uint32_t * framebuffer_row = (void*)frame_buffer + (bmp->height - 1 - i) * bmp->width * 4;
        uint32_t * framebuffer_row = (void*)frame_buffer + (bmp->height - 1 - i) * bmp->width * 4;
        j = 0;
        for(int k = 0; k < bmp->width; k++) {
            uint32_t b = image_row[j++] & 0xff;
            uint32_t g = image_row[j++] & 0xff;
            uint32_t r = image_row[j++] & 0xff;
            uint32_t a = image_row[j++] & 0xff;
            uint32_t rgba = ((a << 24) | (r << 16) | (g << 8) | (b));
            framebuffer_row[k] = rgba;
        }
    }
}
