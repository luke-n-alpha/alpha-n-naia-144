#include "platform.h"

#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

typedef struct {
    Display *display;
    Window window;
    GC gc;
    XImage *image;
    Atom wm_delete;
    int screen;
    int width;
    int height;
    char *image_data;
} X11State;

int plat_init(Platform *plat, const PlatformConfig *config) {
    X11State *state;
    Visual *visual;
    int depth;

    plat->impl = calloc(1, sizeof(X11State));
    if (plat->impl == NULL) {
        return -1;
    }
    state = (X11State *)plat->impl;

    state->display = XOpenDisplay(NULL);
    if (state->display == NULL) {
        plat_shutdown(plat);
        return -1;
    }

    state->screen = DefaultScreen(state->display);
    state->width = config->width;
    state->height = config->height;
    visual = DefaultVisual(state->display, state->screen);
    depth = DefaultDepth(state->display, state->screen);

    state->window = XCreateSimpleWindow(
        state->display,
        RootWindow(state->display, state->screen),
        0,
        0,
        (unsigned int)state->width,
        (unsigned int)state->height,
        1,
        BlackPixel(state->display, state->screen),
        WhitePixel(state->display, state->screen)
    );
    state->gc = XCreateGC(state->display, state->window, 0, NULL);
    XSelectInput(state->display, state->window, ExposureMask | KeyPressMask | StructureNotifyMask);
    XStoreName(state->display, state->window, config->title);

    state->wm_delete = XInternAtom(state->display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(state->display, state->window, &state->wm_delete, 1);

    state->image_data = (char *)calloc((size_t)state->width * (size_t)state->height, sizeof(uint32_t));
    if (state->image_data == NULL) {
        plat_shutdown(plat);
        return -1;
    }

    state->image = XCreateImage(
        state->display,
        visual,
        (unsigned int)depth,
        ZPixmap,
        0,
        state->image_data,
        (unsigned int)state->width,
        (unsigned int)state->height,
        32,
        0
    );
    if (state->image == NULL) {
        plat_shutdown(plat);
        return -1;
    }

    XMapWindow(state->display, state->window);
    XFlush(state->display);
    return 0;
}

int plat_present(Platform *plat, const uint32_t *pixels, int width, int height) {
    X11State *state = (X11State *)plat->impl;
    size_t byte_count;

    if (state == NULL || state->image == NULL || width != state->width || height != state->height) {
        return -1;
    }

    byte_count = (size_t)width * (size_t)height * sizeof(uint32_t);
    memcpy(state->image_data, pixels, byte_count);
    XPutImage(state->display, state->window, state->gc, state->image, 0, 0, 0, 0, (unsigned int)width, (unsigned int)height);
    XFlush(state->display);
    return 0;
}

int plat_poll(Platform *plat, int *should_quit) {
    X11State *state = (X11State *)plat->impl;
    XEvent event;

    if (should_quit != NULL) {
        *should_quit = 0;
    }

    while (state != NULL && state->display != NULL && XPending(state->display) > 0) {
        XNextEvent(state->display, &event);
        if (event.type == ClientMessage && (Atom)event.xclient.data.l[0] == state->wm_delete) {
            if (should_quit != NULL) {
                *should_quit = 1;
            }
        } else if (event.type == DestroyNotify) {
            if (should_quit != NULL) {
                *should_quit = 1;
            }
        } else if (event.type == KeyPress) {
            if (should_quit != NULL) {
                *should_quit = 1;
            }
        }
    }

    return 0;
}

PlatKey plat_poll_key(Platform *plat) {
    X11State *state = (X11State *)plat->impl;
    XEvent event;

    while (state != NULL && state->display != NULL && XPending(state->display) > 0) {
        XNextEvent(state->display, &event);
        if (event.type == ClientMessage && (Atom)event.xclient.data.l[0] == state->wm_delete) {
            return PLAT_KEY_QUIT;
        } else if (event.type == DestroyNotify) {
            return PLAT_KEY_QUIT;
        } else if (event.type == KeyPress) {
            KeySym ks = XLookupKeysym(&event.xkey, 0);
            switch (ks) {
                case XK_Up: return PLAT_KEY_UP;
                case XK_Down: return PLAT_KEY_DOWN;
                case XK_Left: return PLAT_KEY_LEFT;
                case XK_Right: return PLAT_KEY_RIGHT;
                case XK_Return: case XK_space: return PLAT_KEY_CONFIRM;
                case XK_BackSpace: return PLAT_KEY_CANCEL;
                case XK_Escape: return PLAT_KEY_QUIT;
                default: break;
            }
        }
    }
    return PLAT_KEY_NONE;
}

void plat_shutdown(Platform *plat) {
    X11State *state = (X11State *)plat->impl;

    if (state == NULL) {
        return;
    }

    if (state->image != NULL) {
        state->image->data = state->image_data;
        XDestroyImage(state->image);
        state->image = NULL;
        state->image_data = NULL;
    } else if (state->image_data != NULL) {
        free(state->image_data);
        state->image_data = NULL;
    }

    if (state->display != NULL && state->gc != 0) {
        XFreeGC(state->display, state->gc);
        state->gc = 0;
    }

    if (state->display != NULL && state->window != 0) {
        XDestroyWindow(state->display, state->window);
        state->window = 0;
    }

    if (state->display != NULL) {
        XCloseDisplay(state->display);
        state->display = NULL;
    }

    free(state);
    plat->impl = NULL;
}
