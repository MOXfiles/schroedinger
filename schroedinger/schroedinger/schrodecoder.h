
#ifndef __SCHRO_DECODER_H__
#define __SCHRO_DECODER_H__

#include <schroedinger/schrobuffer.h>
#include <schroedinger/schroparams.h>
#include <schroedinger/schroframe.h>
#include <schroedinger/schrobits.h>
#include <schroedinger/schrobitstream.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _SchroDecoder SchroDecoder;

struct _SchroDecoder {
  SchroFrame *frame;
  SchroFrame *mc_tmp_frame;
  int n_reference_frames;
  SchroFrame *reference_frames[SCHRO_MAX_REFERENCE_FRAMES];
  SchroFrame *output_frames[SCHRO_MAX_REFERENCE_FRAMES];
  int n_output_frames;

  int next_frame_number;

  int picture_number;
  int n_refs;
  int reference1;
  int reference2;
  SchroFrame *ref0;
  SchroFrame *ref1;

  int16_t *tmpbuf;
  int16_t *tmpbuf2;

  int code;
  int au_frame_number;
  int next_parse_offset;
  int prev_parse_offset;

  SchroBits *bits;

  int major_version;
  int minor_version;
  int profile;
  int level;
  SchroVideoFormat video_format;
  SchroParams params;

  SchroSubband subbands[1+SCHRO_MAX_TRANSFORM_DEPTH*3];

  SchroMotionField *motion_field;

  int n_retire;
  int retire_list[SCHRO_MAX_REFERENCE_FRAMES];

  int frame_queue_length;
  SchroFrame *frame_queue[SCHRO_MAX_REFERENCE_FRAMES];

  int earliest_frame;
  SchroBuffer *input_buffer;

  int have_access_unit;
  int have_frame_number;

  double skip_value;
  double skip_ratio;
};

enum {
  SCHRO_DECODER_OK,
  SCHRO_DECODER_ERROR,
  SCHRO_DECODER_EOS,
  SCHRO_DECODER_FIRST_ACCESS_UNIT,
  SCHRO_DECODER_NEED_BITS,
  SCHRO_DECODER_NEED_FRAME
};

SchroDecoder * schro_decoder_new (void);
void schro_decoder_free (SchroDecoder *decoder);
void schro_decoder_reset (SchroDecoder *decoder);
SchroVideoFormat * schro_decoder_get_video_format (SchroDecoder *decoder);
void schro_decoder_add_output_frame (SchroDecoder *decoder, SchroFrame *frame);
void schro_decoder_push (SchroDecoder *decoder, SchroBuffer *buffer);
SchroFrame *schro_decoder_pull (SchroDecoder *decoder);
int schro_decoder_is_parse_header (SchroBuffer *buffer);
int schro_decoder_is_access_unit (SchroBuffer *buffer);
int schro_decoder_is_intra (SchroBuffer *buffer);
int schro_decoder_iterate (SchroDecoder *decoder);
void schro_decoder_decode_parse_header (SchroDecoder *decoder);
void schro_decoder_decode_access_unit (SchroDecoder *decoder);
void schro_decoder_decode_frame_header (SchroDecoder *decoder);
void schro_decoder_decode_frame_prediction (SchroDecoder *decoder);
void schro_decoder_decode_prediction_data (SchroDecoder *decoder);
void schro_decoder_decode_transform_parameters (SchroDecoder *decoder);
void schro_decoder_decode_transform_data (SchroDecoder *decoder);
void schro_decoder_decode_subband (SchroDecoder *decoder, int component, int index);
void schro_decoder_iwt_transform (SchroDecoder *decoder, int component);
void schro_decoder_copy_from_frame_buffer (SchroDecoder *decoder, SchroBuffer *buffer);
void schro_decoder_set_earliest_frame (SchroDecoder *decoder, int earliest_frame);
void schro_decoder_set_skip_ratio (SchroDecoder *decoder, double ratio);

#ifdef __cplusplus
}
#endif

#endif

