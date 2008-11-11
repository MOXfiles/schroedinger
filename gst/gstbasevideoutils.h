/* GStreamer
 * Copyright (C) 2008 David Schleef <ds@schleef.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef _GST_BASE_VIDEO_UTILS_H_
#define _GST_BASE_VIDEO_UTILS_H_

#include <gst/gst.h>
#include <gst/video/video.h>
#include <gst/base/gstadapter.h>

typedef struct _GstVideoState GstVideoState;
typedef struct _GstVideoFrame GstVideoFrame;

struct _GstVideoState
{
  GstVideoFormat format;
  int width, height;
  int fps_n, fps_d;
  int par_n, par_d;
  gboolean interlaced;

  int clean_width, clean_height;
  int clean_offset_left, clean_offset_top;

  int bytes_per_picture;

  GstSegment segment;

  int picture_number;
};

struct _GstVideoFrame
{
  guint64 decode_timestamp;
  guint64 presentation_timestamp;
  guint64 presentation_duration;

  gint system_frame_number;
  gint decode_frame_number;
  gint presentation_frame_number;

  int distance_from_sync;
  gboolean is_sync_point;
  gboolean is_eos;

  GstBuffer *sink_buffer;
  GstBuffer *src_buffer;

  void *coder_hook;
};

gboolean gst_base_video_rawvideo_convert (GstVideoState *state,
    GstFormat src_format, gint64 src_value,
    GstFormat * dest_format, gint64 *dest_value);
gboolean gst_base_video_encoded_video_convert (GstVideoState *state,
    GstFormat src_format, gint64 src_value,
    GstFormat * dest_format, gint64 *dest_value);

gboolean gst_base_video_state_from_caps (GstVideoState *state,
    GstCaps *caps);

GstClockTime gst_video_state_get_timestamp (const GstVideoState *state,
    int frame_number);

void gst_adapter_copy_full (GstAdapter *adpater, void *dest, int offset,
    int size);
int gst_adapter_masked_scan_uint32 (GstAdapter *adapter,
    guint32 pattern, guint32 mask, int offset, int n);
GstBuffer *gst_adapter_get_buffer (GstAdapter *adapter);

#endif

