
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <schroedinger/schro.h>


void
schro_encoder_frame_analyse (SchroEncoder *encoder, SchroEncoderFrame *frame)
{
  int i;

  SCHRO_DEBUG("downsampling frame");

  for(i=0;i<5;i++){
    frame->downsampled_frames[i] =
      schro_frame_new_and_alloc (frame->original_frame->format,
          ROUND_UP_SHIFT(frame->original_frame->width, i+1),
          ROUND_UP_SHIFT(frame->original_frame->height, i+1));
  }

  schro_frame_downsample (frame->downsampled_frames[0],
      frame->original_frame);
  schro_frame_downsample (frame->downsampled_frames[1],
      frame->downsampled_frames[0]);
  schro_frame_downsample (frame->downsampled_frames[2],
      frame->downsampled_frames[1]);
  schro_frame_downsample (frame->downsampled_frames[3],
      frame->downsampled_frames[2]);
  schro_frame_downsample (frame->downsampled_frames[4],
      frame->downsampled_frames[3]);

}

static double
schro_frame_component_squared_error (SchroFrameComponent *a,
    SchroFrameComponent *b)
{
  int i;
  int j;
  uint8_t *da, *db;
  double sum;
  
  sum = 0;
  for(j=0;j<a->height;j++){
    da = OFFSET(a->data, a->stride * j);
    db = OFFSET(b->data, b->stride * j);
    for(i=0;i<a->width;i++){
      sum += (da[i]-db[i])*(da[i]-db[i]);
    }
  }
  return sum;
}

double
schro_frame_mean_squared_error (SchroFrame *a, SchroFrame *b)
{
  double sum, n;

  sum = schro_frame_component_squared_error (&a->components[0],
      &b->components[0]);
  n = a->components[0].width * a->components[0].height;

  sum += schro_frame_component_squared_error (&a->components[1],
      &b->components[1]);
  n += a->components[1].width * a->components[1].height;

  sum += schro_frame_component_squared_error (&a->components[2],
      &b->components[2]);
  n += a->components[2].width * a->components[2].height;

  return sum/n;
}

