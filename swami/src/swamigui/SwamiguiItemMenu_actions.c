/*
 * SwamiguiTreeMenu_items.c - Built in menu items
 *
 * Swami
 * Copyright (C) 1999-2010 Joshua "Element" Green <jgreen@users.sourceforge.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License only.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA or point your web browser to http://www.gnu.org.
 */
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <libinstpatch/libinstpatch.h>

#include "SwamiguiItemMenu.h"
#include "icons.h"
#include "patch_funcs.h"
#include "i18n.h"


static void item_action_paste_handler (SwamiguiItemMenu *menu,
				       const char *action_id);
static void item_action_goto_link_handler (SwamiguiItemMenu *menu,
					   const char *action_id);
static void item_action_load_samples_handler (SwamiguiItemMenu *menu,
					      const char *action_id);
static void item_action_new_handler (SwamiguiItemMenu *menu,
				     const char *action_id);

static void item_cb_paste_items (IpatchList *selection, gpointer data);
static void item_cb_find_item (IpatchList *selection, gpointer data);
static void item_cb_find_next_item (IpatchList *selection, gpointer data);
static void item_cb_goto_link_item (IpatchList *selection, gpointer data);
static void item_cb_load_samples (IpatchList *selection, gpointer data);
static void item_cb_new (IpatchList *selection, gpointer data);
static void item_cb_solo_toggle (IpatchList *selection, gpointer data);

typedef struct
{
  char *action_id;
  SwamiguiItemMenuHandler handler;
  SwamiguiItemMenuInfo info;
} MultiActionInfo;

MultiActionInfo item_action_info[] =
{
  { "copy", swamigui_item_menu_handler_multi,
    { 100, N_("_Copy"), "<control>C", GTK_STOCK_COPY, 0,
      (SwamiguiItemMenuCallback)swamigui_copy_items, NULL } },
  { "paste", item_action_paste_handler,
    { 200, N_("_Paste"), "<control>V", GTK_STOCK_PASTE, 0,
      (SwamiguiItemMenuCallback)item_cb_paste_items, NULL } },
  { "delete", swamigui_item_menu_handler_multi,
    { 300, N_("_Delete"), "<control>D", GTK_STOCK_DELETE, 0,
      (SwamiguiItemMenuCallback)swamigui_delete_items, NULL } },
  { "find", swamigui_item_menu_handler_single_all,
    { 400, N_("_Find"), "<control>F", GTK_STOCK_FIND, 0,
      item_cb_find_item, NULL } },
  { "find-next", swamigui_item_menu_handler_single_all,
    { 410, N_("Find _Next"), "<control>G", GTK_STOCK_FIND, 0,
      item_cb_find_next_item, NULL } },
  { "goto-link", item_action_goto_link_handler,
    { 500, N_("_Goto Link"), "<control>L", GTK_STOCK_JUMP_TO, 0,
      item_cb_goto_link_item, NULL } },
  { "save", swamigui_item_menu_handler_multi,
    { 600, N_("_Save"), "<control>S", GTK_STOCK_SAVE, 0,
      (SwamiguiItemMenuCallback)swamigui_save_files, GINT_TO_POINTER (FALSE) } },
  { "save-as", swamigui_item_menu_handler_multi,
    { 700, N_("Save _As"), NULL, GTK_STOCK_SAVE_AS, 0,
      (SwamiguiItemMenuCallback)swamigui_save_files, GINT_TO_POINTER(TRUE) } },
  { "close", swamigui_item_menu_handler_multi,
    { 800, N_("Close"), "<control>W", GTK_STOCK_CLOSE, 0,
      (SwamiguiItemMenuCallback)swamigui_close_files, NULL } },
  { "load-samples", item_action_load_samples_handler,
    { 900, N_("Load Samples"), NULL, GTK_STOCK_OPEN, 0,
      item_cb_load_samples, NULL } },
  { "export-samples", swamigui_item_menu_handler_multi,
    { 1000, N_("Export Samples"), NULL, GTK_STOCK_SAVE, 0,
      (SwamiguiItemMenuCallback)swamigui_export_samples, NULL } },
  { "solo-item", swamigui_item_menu_handler_single,
    { 1100, N_("Solo Toggle"), "<control>T", GTK_STOCK_MEDIA_PLAY, 0,
      (SwamiguiItemMenuCallback)item_cb_solo_toggle, NULL } },

  /* some of these fields are dynamically generated by the handler */
  { "new", item_action_new_handler,
    { 1100, "", NULL, GTK_STOCK_NEW, 0, item_cb_new, NULL } }
};

void
_swamigui_item_menu_actions_init (void)
{
  GValue value = { 0 };
  GType *types;
  guint count, i;

  /* register the item actions */
  for (i = 0; i < G_N_ELEMENTS (item_action_info); i++)
    {
      swamigui_register_item_menu_action (item_action_info[i].action_id,
					  &item_action_info[i].info,
					  item_action_info[i].handler);
    }

  swamigui_register_item_menu_include_type ("copy", IPATCH_TYPE_ITEM, TRUE);
  swamigui_register_item_menu_exclude_type ("copy", IPATCH_TYPE_VIRTUAL_CONTAINER,
					    TRUE);
  swamigui_register_item_menu_include_type ("paste",
					    IPATCH_TYPE_CONTAINER, TRUE);
  swamigui_register_item_menu_include_type ("paste",
					    IPATCH_TYPE_VIRTUAL_CONTAINER, TRUE);
  swamigui_register_item_menu_include_type ("delete",
					    IPATCH_TYPE_ITEM, TRUE);
  swamigui_register_item_menu_exclude_type ("delete", IPATCH_TYPE_BASE, TRUE);
  swamigui_register_item_menu_exclude_type ("delete",
					  IPATCH_TYPE_VIRTUAL_CONTAINER, TRUE);
  swamigui_register_item_menu_include_type ("export-samples",
					    IPATCH_TYPE_SAMPLE, TRUE);
  swamigui_register_item_menu_include_type ("save", IPATCH_TYPE_ITEM, TRUE);

  swamigui_register_item_menu_include_type ("save-as", IPATCH_TYPE_ITEM, TRUE);

  swamigui_register_item_menu_include_type ("close", IPATCH_TYPE_BASE, TRUE);

  /* Add types for solo-item action to instrument ref and sample ref category types */
  g_value_init (&value, G_TYPE_INT);
  g_value_set_int (&value, IPATCH_CATEGORY_INSTRUMENT_REF);
  types = ipatch_type_find_types_with_property ("category", &value, &count);    /* ++ alloc types */

  if (types)
  {
    for (i = 0; i < count; i++)
      swamigui_register_item_menu_include_type ("solo-item", types[i], TRUE);
    g_free (types);     /* -- unref types */
  }

  g_value_set_int (&value, IPATCH_CATEGORY_SAMPLE_REF);
  types = ipatch_type_find_types_with_property ("category", &value, &count);    /* ++ alloc types */

  if (types)
  {
    for (i = 0; i < count; i++)
      swamigui_register_item_menu_include_type ("solo-item", types[i], TRUE);
    g_free (types);     /* -- unref types */
  }

#if 0
  { "wavetbl-load", N_("Wavetable Load"), "<control>L", 0,
    swamigui_ifaces_cb_wtbl_load_patch, NULL }
#endif
}

static void
item_action_paste_handler (SwamiguiItemMenu *menu, const char *action_id)
{
  GObject *item;

  item = swamigui_item_menu_get_selection_single (menu);
  if (!item) return;

  /* if item type matches an exclude type - return */
  if (!swamigui_test_item_menu_exclude_type (action_id, G_OBJECT_TYPE (item)))
    return;

  /* if item does not match an include type.. */
  if (!swamigui_test_item_menu_include_type (action_id, G_OBJECT_TYPE (item)))
    {
      GType link_type;

      /* IpatchItem's with a link type are valid */
      ipatch_type_get (G_OBJECT_TYPE (item), "link-type", &link_type, NULL);
      if (!IPATCH_IS_ITEM (item) || link_type == G_TYPE_NONE) return;
    }

  swamigui_item_menu_add_registered_info (menu, action_id);
}

static void
item_action_goto_link_handler (SwamiguiItemMenu *menu, const char *action_id)
{
  IpatchList *list;
  GObject *item, *origin;

  list = swamigui_item_menu_get_selection (menu);

  /* make sure exactly one item is selected */
  if (!list || !list->items || list->items->next) return;

  /* only makes sense to goto link if a tree is the origin of selection */
  origin = swami_object_get_origin (G_OBJECT (list));
  if (!origin || !SWAMIGUI_IS_TREE (origin)) return;

  item = (GObject *)(list->items->data);

  /* if item type matches an exclude type - return */
  if (!swamigui_test_item_menu_exclude_type (action_id, G_OBJECT_TYPE (item)))
    return;

  /* if item does not match an include type.. */
  if (!swamigui_test_item_menu_include_type (action_id, G_OBJECT_TYPE (item)))
    {
      GType link_type;
      GObject *link;

      /* if object has link-type type property and link-item property is set,
	 create menu item */

      ipatch_type_get (G_OBJECT_TYPE (item), "link-type", &link_type, NULL);
      if (!IPATCH_IS_ITEM (item) || link_type == G_TYPE_NONE) return;

      g_object_get (item, "link-item", &link, NULL);
      if (link) g_object_unref (link);
      if (!link) return;
    }

  swamigui_item_menu_add_registered_info (menu, action_id);
}

static void
item_action_load_samples_handler (SwamiguiItemMenu *menu, const char *action_id)
{
  GObject *item;
  const GType *child_types, child;
  int category;

  item = swamigui_item_menu_get_selection_single (menu);
  if (!item) return;

  /* if item type matches an exclude type - return */
  if (!swamigui_test_item_menu_exclude_type (action_id, G_OBJECT_TYPE (item)))
    return;

  /* if item does not match an include type.. */
  if (!swamigui_test_item_menu_include_type (action_id, G_OBJECT_TYPE (item)))
  {
    /* virtual container which contains sample category items? - Add */
    if (IPATCH_IS_VIRTUAL_CONTAINER (item))
    {
      ipatch_type_get (G_OBJECT_TYPE (item), "virtual-child-type", &child, NULL);
      if (child == G_TYPE_NONE) return;

      ipatch_type_get (child, "category", &category, NULL);
      if (category != IPATCH_CATEGORY_SAMPLE) return;
    }
    else /* IpatchBase derived and has child item with sample category? - add */
    {
      if (!IPATCH_IS_BASE (item)) return;

      child_types = ipatch_container_get_child_types (IPATCH_CONTAINER (item));
      for (; *child_types; child_types++)
      {
	ipatch_type_get (*child_types, "category", &category, NULL);
	if (category == IPATCH_CATEGORY_SAMPLE) break;
      }

      if (!*child_types) return;
    }
  }

  swamigui_item_menu_add_registered_info (menu, action_id);
}

static void
item_action_new_handler (SwamiguiItemMenu *menu, const char *action_id)
{
  GObject *item;
  const GType *child_types;
  GType type;
  SwamiguiItemMenuInfo newinfo, *infop;
  char *type_name;
  int category;

  item = swamigui_item_menu_get_selection_single (menu);
  if (!item) return;

  /* if item type matches an exclude type - return */
  if (!swamigui_test_item_menu_exclude_type (action_id, G_OBJECT_TYPE (item)))
    return;

  if (IPATCH_IS_VIRTUAL_CONTAINER (item))
  {
    swamigui_lookup_item_menu_action (action_id, &infop, NULL);
    newinfo = *infop;

    ipatch_type_get (G_OBJECT_TYPE (item), "virtual-child-type", &type, NULL);
    if (type == G_TYPE_NONE) return;

    /* only New for Program and Instrument types */
    ipatch_type_get (type, "category", &category, NULL);
    if (category != IPATCH_CATEGORY_PROGRAM
	&& category != IPATCH_CATEGORY_INSTRUMENT) return;

    ipatch_type_get (type, "name", &type_name, NULL);
    newinfo.label = g_strdup_printf (_("New %s"), type_name ? type_name
				     : g_type_name (type));
    g_free (type_name);
    newinfo.data = GUINT_TO_POINTER (type);

    swamigui_item_menu_add (menu, &newinfo, action_id);

    g_free (newinfo.label);

    return;
  }

  if (!IPATCH_IS_CONTAINER (item)) return;

  swamigui_lookup_item_menu_action (action_id, &infop, NULL);
  newinfo = *infop;

  child_types = ipatch_container_get_child_types (IPATCH_CONTAINER (item));
  for (; *child_types; child_types++)
    {
      /* skip any types in exclude list */
      if (!swamigui_test_item_menu_exclude_type (action_id, *child_types))
	continue;

      /* only New for Program and Instrument types */
      ipatch_type_get (*child_types, "category", &category, NULL);
      if (category != IPATCH_CATEGORY_PROGRAM
	  && category != IPATCH_CATEGORY_INSTRUMENT)
	continue;

      newinfo.order++;
      ipatch_type_get (*child_types, "name", &type_name, NULL);
      newinfo.label = g_strdup_printf (_("New %s"), type_name ? type_name
				       : g_type_name (*child_types));
      g_free (type_name);
      newinfo.data = GUINT_TO_POINTER (*child_types);

      swamigui_item_menu_add (menu, &newinfo, action_id);

      g_free (newinfo.label);
    }
}

/* SwamiguiItemMenu action callbacks */

static void
item_cb_paste_items (IpatchList *selection, gpointer data)
{
  if (!selection->items || selection->items->next) return;

  swamigui_paste_items (IPATCH_ITEM (selection->items->data), NULL);
}

static void
item_cb_find_item (IpatchList *selection, gpointer data)
{
  GObject *origin;

  /* ++ ref origin object */
  origin = swami_object_get_origin (G_OBJECT (selection));
  if (!origin || !SWAMIGUI_IS_TREE (origin)) return;

  swamigui_tree_search_set_visible (SWAMIGUI_TREE (origin), TRUE);

  g_object_unref (origin);	/* -- unref */
}

static void
item_cb_find_next_item (IpatchList *selection, gpointer data)
{
  GObject *origin;

  /* ++ ref origin object */
  origin = swami_object_get_origin (G_OBJECT (selection));
  if (!origin || !SWAMIGUI_IS_TREE (origin)) return;

  swamigui_tree_search_next (SWAMIGUI_TREE (origin));

  g_object_unref (origin);	/* -- unref */
}

static void
item_cb_goto_link_item (IpatchList *selection, gpointer data)
{
  GObject *origin;

  if (!selection->items || selection->items->next) return;

  /* ++ ref origin object */
  origin = swami_object_get_origin (G_OBJECT (selection));
  if (!origin || !SWAMIGUI_IS_TREE (origin)) return;

  swamigui_goto_link_item (IPATCH_ITEM (selection->items->data),
			   SWAMIGUI_TREE (origin));

  g_object_unref (origin);	/* -- unref */
}

static void
item_cb_load_samples (IpatchList *selection, gpointer data)
{
  if (!selection->items || selection->items->next) return;

  swamigui_load_samples (IPATCH_ITEM (selection->items->data));
}

static void
item_cb_new (IpatchList *selection, gpointer data)
{
  if (!selection->items || selection->items->next) return;

  swamigui_new_item (IPATCH_ITEM (selection->items->data),
		     (GType)GPOINTER_TO_UINT (data));
}

static void
item_cb_solo_toggle (IpatchList *selection, gpointer data)
{
  gboolean enabled;

  if (!selection->items || selection->items->next) return;

  /* Toggle solo-item-enable property of swamiguiroot */
  g_object_get (swamigui_root, "solo-item-enable", &enabled, NULL);
  g_object_set (swamigui_root, "solo-item-enable", !enabled, NULL);
}
