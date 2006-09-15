
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <schroedinger/schrooil.h>
#include <liboil/liboil.h>


void
oil_add_s16(int16_t *d_n, int16_t *s1_n, int16_t *s2_n, int n)
{
  int i;

  for(i=0;i<n;i++){
    d_n[i] = s1_n[i] + s2_n[i];
  }
}

void
oil_add_s16_u8(int16_t *d_n, int16_t *s1_n, uint8_t *s2_n, int n)
{
  int i;

  for(i=0;i<n;i++){
    d_n[i] = s1_n[i] + s2_n[i];
  }
}

void
oil_subtract_s16(int16_t *d_n, int16_t *s1_n, int16_t *s2_n, int n)
{
  int i;

  for(i=0;i<n;i++){
    d_n[i] = s1_n[i] - s2_n[i];
  }
}

void
oil_subtract_s16_u8(int16_t *d_n, int16_t *s1_n, uint8_t *s2_n, int n)
{
  int i;

  for(i=0;i<n;i++){
    d_n[i] = s1_n[i] - (s2_n[i]<<4);
  }
}

void
oil_lshift_s16(int16_t *data, int *shift, int n)
{
  int i;
  for(i=0;i<n;i++){
    data[i] <<= *shift;
  }
}

void
oil_splat_s16_ns (int16_t *dest, int16_t *src, int n)
{
  oil_splat_u16_ns ((uint16_t *)dest, (uint16_t *)src, n);
}

void
oil_add_and_rshift_s16(int16_t *dest, int16_t *src, int32_t *add,
    int32_t *shift, int n)
{
  int i;

  for(i=0;i<n;i++){
    dest[i] = (src[i] + *add) >> (*shift);
  }
}

void
oil_lift_haar_split (int16_t *i1, int16_t *i2, int n)
{
  int i;
  for(i=0;i<n;i++){
    i2[i] -= i1[i];
    i1[i] += i2[i]>>1;
  }
}

void
oil_lift_haar_synth (int16_t *i1, int16_t *i2, int n)
{
  int i;
  for(i=0;i<n;i++){
    i1[i] -= i2[i]>>1;
    i2[i] += i1[i];
  }
}

void
oil_synth_haar (int16_t *d, int16_t *s, int n)
{
  int i;

  for(i=0;i<n;i++){
    d[2*i] = s[2*i] - (s[2*i+1]>>1);
    d[2*i + 1] = s[2*i+1] + d[2*i];
  }
}

void
oil_split_haar (int16_t *d, int16_t *s, int n)
{
  int i;

  for(i=0;i<n;i++){
    d[2*i+1] = s[2*i+1] - s[2*i];
    d[2*i] = s[2*i] + (d[2*i + 1]>>1);
  }
}

void
oil_multiply_and_add_s16 (int16_t *dest, int16_t *src1, uint8_t *src2,
    int n)
{
  int i;
  for(i=0;i<n;i++){
    dest[i] += src1[i] * src2[i];
  }
}
