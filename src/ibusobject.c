/* vim:set et sts=4: */
/* ibus - The Input Bus
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

#include "ibusobject.h"
#include "ibusinternal.h"

#define IBUS_OBJECT_GET_PRIVATE(o)  \
   (G_TYPE_INSTANCE_GET_PRIVATE ((o), IBUS_TYPE_OBJECT, IBusObjectPrivate))

enum {
    DESTROY,
    LAST_SIGNAL,
};

typedef struct _IBusObjectPrivate IBusObjectPrivate;
struct _IBusObjectPrivate {
    gpointer pad;
};

static guint            object_signals[LAST_SIGNAL] = { 0 };

// #define DEBUG_MEMORY
#ifdef DEBUG_MEMORY
static GHashTable      *_count_table;
static guint            _count = 0;
#endif

/* functions prototype */
static void      ibus_object_class_init     (IBusObjectClass    *klass);
static void      ibus_object_init           (IBusObject         *obj);
static GObject  *ibus_object_constructor    (GType               type,
                                             guint               n,
                                             GObjectConstructParam
                                                                *args);
static void      ibus_object_dispose        (IBusObject         *obj);
static void      ibus_object_finalize       (IBusObject         *obj);
static void      ibus_object_real_destroy   (IBusObject         *obj);

static GObjectClass *parent_class = NULL;


GType
ibus_object_get_type (void)
{
    static GType type = 0;

    static const GTypeInfo type_info = {
        sizeof (IBusObjectClass),
        (GBaseInitFunc)     NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc)    ibus_object_class_init,
        NULL,               /* class finialize */
        NULL,               /* class data */
        sizeof (IBusObject),
        0,
        (GInstanceInitFunc) ibus_object_init,
    };

    if (type == 0) {
        type = g_type_register_static (G_TYPE_OBJECT,
                    "IBusObject",
                    &type_info,
                    G_TYPE_FLAG_ABSTRACT);
    }

    return type;
}

/**
 * ibus_object_new:
 *
 * Creates a new instance of an #IBusObject.
 *
 * Returns: a new instance of #IBusObject.
 */
IBusObject *
ibus_object_new (void)
{
    return IBUS_OBJECT (g_object_new (IBUS_TYPE_OBJECT, NULL));
}

static void
ibus_object_class_init     (IBusObjectClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    parent_class = (GObjectClass *) g_type_class_peek_parent (klass);

    g_type_class_add_private (klass, sizeof (IBusObjectPrivate));

    gobject_class->constructor = ibus_object_constructor;
    gobject_class->dispose = (GObjectFinalizeFunc) ibus_object_dispose;
    gobject_class->finalize = (GObjectFinalizeFunc) ibus_object_finalize;

    klass->destroy = ibus_object_real_destroy;

    /* install signals */
    /**
     * IBusObject::destroy:
     * @object: An IBusObject.
     *
     * Destroy and free an IBusObject
     *
     * See also:  ibus_object_destroy().
     *
     * <note><para>Argument @user_data is ignored in this function.</para></note>
     */
    object_signals[DESTROY] =
        g_signal_new (I_("destroy"),
            G_TYPE_FROM_CLASS (gobject_class),
            G_SIGNAL_RUN_LAST,
            G_STRUCT_OFFSET (IBusObjectClass, destroy),
            NULL, NULL,
            ibus_marshal_VOID__VOID,
            G_TYPE_NONE, 0);
#ifdef DEBUG_MEMORY
    _count_table = g_hash_table_new (g_direct_hash, g_direct_equal);
#endif

}

static void
ibus_object_init (IBusObject *obj)
{
    IBusObjectPrivate *priv;
    priv = IBUS_OBJECT_GET_PRIVATE (obj);

    obj->flags = 0;

}


static GObject *
ibus_object_constructor (GType                   type,
                         guint                   n,
                         GObjectConstructParam  *args)
{
    GObject *object;

    object = parent_class->constructor (type, n ,args);

#ifdef DEBUG_MEMORY
    if (object != NULL) {
        guint count;
        _count ++;

        count = GPOINTER_TO_UINT (g_hash_table_lookup (_count_table, (gpointer) type));
        g_hash_table_replace (_count_table, (gpointer) type, GUINT_TO_POINTER (++count));

        g_debug ("new %s, count = %d, all = %d", g_type_name (type), count, _count);
    }
#endif

    return object;
}


static void
ibus_object_dispose (IBusObject *obj)
{
    if (! (IBUS_OBJECT_FLAGS (obj) & IBUS_IN_DESTRUCTION)) {
        IBUS_OBJECT_SET_FLAGS (obj, IBUS_IN_DESTRUCTION);
        if (! (IBUS_OBJECT_FLAGS (obj) & IBUS_DESTROYED)) {
            g_signal_emit (obj, object_signals[DESTROY], 0);
            IBUS_OBJECT_SET_FLAGS (obj, IBUS_DESTROYED);
        }
        IBUS_OBJECT_UNSET_FLAGS (obj, IBUS_IN_DESTRUCTION);
    }

    G_OBJECT_CLASS(parent_class)->dispose (G_OBJECT (obj));
}

static void
ibus_object_finalize (IBusObject *obj)
{
#ifdef DEBUG_MEMORY 
    guint count;

    _count --;
    count = GPOINTER_TO_UINT (g_hash_table_lookup (_count_table, (gpointer)G_OBJECT_TYPE (obj)));
    g_hash_table_replace (_count_table, (gpointer)G_OBJECT_TYPE (obj), GUINT_TO_POINTER (--count));
    g_debug ("Finalize %s, count = %d, all = %d", G_OBJECT_TYPE_NAME (obj), count, _count);
#endif

    G_OBJECT_CLASS(parent_class)->finalize (G_OBJECT (obj));
}

static void
ibus_object_real_destroy (IBusObject *obj)
{
    g_signal_handlers_destroy (obj);
}

void
ibus_object_destroy (IBusObject *obj)
{
    IBusObjectPrivate *priv;
    priv = IBUS_OBJECT_GET_PRIVATE (obj);

    if (! (IBUS_OBJECT_FLAGS (obj) & IBUS_IN_DESTRUCTION)) {
        g_object_run_dispose (G_OBJECT (obj));
    }
}
