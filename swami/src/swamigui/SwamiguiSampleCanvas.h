/*
 * SwamiguiSampleCanvas.h - Sample data canvas item
 * A canvas item for sample data
 *
 * Swami
 * Copyright (C) 1999-2014 Element Green <element@elementsofsound.org>
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
#ifndef __SWAMIGUI_SAMPLE_CANVAS_H__
#define __SWAMIGUI_SAMPLE_CANVAS_H__

#include <gtk/gtk.h>
#include <libinstpatch/libinstpatch.h>

typedef struct _SwamiguiSampleCanvas SwamiguiSampleCanvas;
typedef struct _SwamiguiSampleCanvasClass SwamiguiSampleCanvasClass;

#define SWAMIGUI_TYPE_SAMPLE_CANVAS   (swamigui_sample_canvas_get_type ())
#define SWAMIGUI_SAMPLE_CANVAS(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), SWAMIGUI_TYPE_SAMPLE_CANVAS, \
			       SwamiguiSampleCanvas))
#define SWAMIGUI_SAMPLE_CANVAS_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), SWAMIGUI_TYPE_SAMPLE_CANVAS, \
   SwamiguiSampleCanvasClass))
#define SWAMIGUI_IS_SAMPLE_CANVAS(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SWAMIGUI_TYPE_SAMPLE_CANVAS))
#define SWAMIGUI_IS_SAMPLE_CANVAS_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), SWAMIGUI_TYPE_SAMPLE_CANVAS))

/* Swami Sample Object */
struct _SwamiguiSampleCanvas
{
  GtkDrawingArea parent_instance;

  /*< private >*/

  IpatchSampleData *sample; 	/* sample being displayed */
  guint sample_size;		/* cached size of sample */
  IpatchSampleHandle handle;	/* cached sample handle */
  gboolean right_chan;		/* use right channel of stereo audio? */
  guint max_frames;	/* max sample frames that can be converted at at time */

  gboolean loop_mode;		/* display loop mode? */
  guint loop_start, loop_end;	/* cached loop start and end in samples */

  guint start;			/* start sample (not used in LOOP mode) */
  double zoom;			/* zoom factor samples/pixel */
  double zoom_ampl;		/* amplitude zoom factor */

  guint32 peak_line_color;	/* color of peak sample lines */
  guint32 line_color;		/* color of connecting sample lines */
  guint32 point_color;		/* color of sample points */
  guint32 loop_start_color;	/* color of sample points for start of loop */
  guint32 loop_end_color;	/* color of sample points for end of loop */
  guint32 bg_color;             /* color of background */
};

struct _SwamiguiSampleCanvasClass
{
  GtkDrawingAreaClass parent_class;
};

GType swamigui_sample_canvas_get_type (void);
void swamigui_sample_canvas_set_sample (SwamiguiSampleCanvas *canvas,
					IpatchSampleData *sample);
int swamigui_sample_canvas_xpos_to_sample (SwamiguiSampleCanvas *canvas,
					   int xpos, int *onsample);
int swamigui_sample_canvas_sample_to_xpos (SwamiguiSampleCanvas *canvas,
					   int index, int *inview);
#endif
