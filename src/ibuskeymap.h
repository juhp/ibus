/* vim:set et sts=4: */
/* IBus - The Input Bus
 * Copyright (C) 2008-2009 Huang Peng <shawn.p.huang@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
/**
 * SECTION: ibuskeymap
 * @short_description: Keyboard mapping handling.
 * @stability: Stable
 * @see_also: #IBusComponent, #IBusEngineDesc
 *
 * An IBusKeymap defines the mapping between keyboard scancodes and
 * keyboard symbols such as numbers, alphabets, and punctuation marks.
 *
 * Some input methods assume certain keyboard layout
 * (such as Chewing and Wubi requires an US-QWERTY layout),
 * and expect key symbols to be arranged in that order.
 * These input methods should new an IBusKeymap
 * instance and define the keyboard layout.
 * Then ibus_keymap_lookup_keysym() can
 * convert scancodes back to the key symbols.
 *
 */

#ifndef __IBUS_KEYMAP_H_
#define __IBUS_KEYMAP_H_

#include "ibusobject.h"

/*
 * Type macros.
 */
/* define IBusKeymap macros */
#define IBUS_TYPE_KEYMAP             \
    (ibus_keymap_get_type ())
#define IBUS_KEYMAP(obj)             \
    (G_TYPE_CHECK_INSTANCE_CAST ((obj), IBUS_TYPE_KEYMAP, IBusKeymap))
#define IBUS_KEYMAP_CLASS(klass)     \
    (G_TYPE_CHECK_CLASS_CAST ((klass), IBUS_TYPE_KEYMAP, IBusKeymapClass))
#define IBUS_IS_KEYMAP(obj)          \
    (G_TYPE_CHECK_INSTANCE_TYPE ((obj), IBUS_TYPE_KEYMAP))
#define IBUS_IS_KEYMAP_CLASS(klass)  \
    (G_TYPE_CHECK_CLASS_TYPE ((klass), IBUS_TYPE_KEYMAP))
#define IBUS_KEYMAP_GET_CLASS(obj)   \
    (G_TYPE_INSTANCE_GET_CLASS ((obj), IBUS_TYPE_KEYMAP, IBusKeymapClass))

G_BEGIN_DECLS

typedef struct _IBusKeymap IBusKeymap;
typedef struct _IBusKeymapClass IBusKeymapClass;

/**
 * KEYMAP:
 *
 * Data structure for storing keymap.
 * keymap[.][i]
 * i:
 *  0 - without modifer
 *  1 - shift
 *  2 - caplock
 *  3 - shift caplock
 *  4 - altgr
 *  5 - shift altgr
 *  6 - numlock
 */
typedef guint KEYMAP[256][7];

/**
 * IBusKeymap:
 * @name: The name of the keymap, such as 'us', 'jp'.
 * @keymap: Keymap table. IME developers normally don have to touch this.
 *
 * A keymap object in IBus.
 */
struct _IBusKeymap {
    /*< private >*/
    IBusObject parent;
    /* members */
    /*< public >*/
    gchar *name;
    KEYMAP keymap;
};

struct _IBusKeymapClass {
    IBusObjectClass parent;
};

GType            ibus_keymap_get_type               (void);

/**
 * ibus_keymap_new:
 * @name: The keymap file to be loaded, such as 'us', 'jp'.
 * @returns: A newly allocated IBusKeymap; or NULL if failed.
 *
 * New an IBusKeymap.
 *
 * This function loads the keymap file specified in @name
 * in the IBUS_DATA_DIR/keymaps directory.
 */
IBusKeymap        *ibus_keymap_new                  (const gchar        *name);

/**
 * ibus_keymap_lookup_keysym:
 * @keymap: An IBusKeymap.
 * @keycode: A scancode to be converted.
 * @state: Modifier flags(such as Ctrl, Shift).
 * @returns: Corresponding keysym.
 *
 * Convert the scancode to keysym, given the keymap.
 */
guint              ibus_keymap_lookup_keysym        (IBusKeymap         *keymap,
                                                     guint16             keycode,
                                                     guint32             state);

G_END_DECLS
#endif

