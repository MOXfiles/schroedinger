
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <carid/caridarith.h>


unsigned int division_factor[1024];

static void
_carid_arith_division_factor_init (void)
{
  static int inited = 0;

  if (!inited) {
    int i;
    for(i=0;i<1024;i++){
      division_factor[i] = (1U<<31)/(i+1);
    }
  }
}

CaridArith *
carid_arith_new (void)
{
  CaridArith *arith;
  
  arith = malloc (sizeof(*arith));
  memset (arith, 0, sizeof(*arith));

  _carid_arith_division_factor_init();

  return arith;
}

void
carid_arith_free (CaridArith *arith)
{
  free(arith);
}

void
carid_arith_decode_init (CaridArith *arith)
{
  int i;

  arith->low = 0;
  arith->high = 0xffff;
  arith->code = 0;

  for(i=0;i<16;i++){
    carid_arith_input_bit(arith);
  }
}

void
carid_arith_encode_init (CaridArith *arith)
{
  arith->low = 0;
  arith->high = 0xffff;
  arith->code = 0;

  memset (arith->data, 0, arith->size);
}

void
carid_arith_input_bit (CaridArith *arith)
{
  arith->code <<= 1;
  arith->code += ((arith->data[arith->offset]>>(7-arith->bit_offset)) & 0x1);
  arith->bit_offset++;
  if (arith->bit_offset >= 8) {
    arith->bit_offset = 0;
    arith->offset++;
  }
}

void
carid_arith_output_bit (CaridArith *arith)
{
  int value;

  value = arith->low >> 15;

  arith->data[arith->offset] |= value << (7-arith->bit_offset);
  arith->bit_offset++;
  if (arith->bit_offset >= 8) {
    arith->bit_offset = 0;
    arith->offset++;
    arith->data[arith->offset] = 0;
  }
  
  value = !value;
  while(arith->cntr) {
    arith->data[arith->offset] |= value << (7-arith->bit_offset);
    arith->bit_offset++;
    if (arith->bit_offset >= 8) {
      arith->bit_offset = 0;
      arith->offset++;
      arith->data[arith->offset] = 0;
    }
    arith->cntr--;
  }
}

void
carid_arith_flush (CaridArith *arith)
{
  unsigned int value;

  //if (arith->cntr) {
    value = 1;
    arith->data[arith->offset] |= value << (7-arith->bit_offset);
    arith->bit_offset++;
    if (arith->bit_offset >= 8) {
      arith->bit_offset = 0;
      arith->offset++;
      arith->data[arith->offset] = 0;
    }
  //}
  if (arith->bit_offset > 0) {
    arith->bit_offset = 0;
    arith->offset++;
  }
}

void
carid_arith_context_init (CaridArith *arith, int i, int count0, int count1)
{
  arith->contexts[i].count0 = count0;
  arith->contexts[i].count1 = count1;
}


void
carid_arith_context_halve_counts (CaridArith *arith, int i)
{
  arith->contexts[i].count0 >>= 1;
  arith->contexts[i].count0++;
  arith->contexts[i].count1 >>= 1;
  arith->contexts[i].count1++;
}

void
carid_arith_context_halve_all_counts (CaridArith *arith)
{
  int i;
  for(i=0;i<arith->n_contexts;i++) {
    arith->contexts[i].count0 >>= 1;
    arith->contexts[i].count0++;
    arith->contexts[i].count1 >>= 1;
    arith->contexts[i].count1++;
  }
}

void
carid_arith_context_update (CaridArith *arith, int i, int value)
{
  if (value) {
    arith->contexts[i].count1++;
  } else {
    arith->contexts[i].count0++;
  }
  if (arith->contexts[i].count0 + arith->contexts[i].count1 >= 1024) {
    carid_arith_context_halve_counts (arith, i);
  }
}




int
carid_arith_context_binary_decode (CaridArith *arith, int i)
{
  unsigned int count;
  int value;
  int range;
  int scaled_count0;
  int weight;

  count = ((arith->code - arith->low + 1)<<10) - 1;
  range = arith->high - arith->low + 1;
  weight = arith->contexts[i].count0 + arith->contexts[i].count1;
  scaled_count0 = ((unsigned int)arith->contexts[i].count0 * division_factor[weight - 1]) >> 21;
#if 0
printf("count0=%d count1=%d division_factor=%08x\n",
    arith->contexts[i].count0, arith->contexts[i].count1,
    division_factor[weight - 1]);
printf("count=%d range=%d scaled_count0=%d (%d cmp %d)\n",
    count, range, scaled_count0, count, range*scaled_count0);
#endif
  if (count < range * scaled_count0) {
    value = 0;
  } else {
    value = 1;
  }

  carid_arith_context_update (arith, i, value);
  
  //printf("  %d [%04x %04x] -> ", value, arith->low, arith->high);
  if (value == 0) {
    arith->high = arith->low + ((range * scaled_count0)>>10) - 1;
  } else {
    arith->low = arith->low + ((range * scaled_count0)>>10);
  }
  //printf("[%04x %04x]\n", arith->low, arith->high);

  do {
    if ((arith->high & (1<<15)) == (arith->low & (1<<15))) {
      /* do nothing */
    } else if ((arith->low & (1<<14)) && !(arith->high & (1<<14))) {
      arith->code ^= (1<<14);
      arith->low ^= (1<<14);
      arith->high ^= (1<<14);
    } else {
      break;
    }

#if 0
    printf("shift: high=%04x low=%04x code=%04x -> ",
        arith->high, arith->low, arith->code);
#endif
    arith->low <<= 1;
    arith->high <<= 1;
    arith->high++;

    carid_arith_input_bit(arith);

#if 0
    printf("high=%04x low=%04x code=%04x\n",
        arith->high, arith->low, arith->code);
#endif
  } while (1);

  return value;
}


void
carid_arith_context_binary_encode (CaridArith *arith, int i, int value)
{
  int range;
  int scaled_count0;
  int weight;

  range = arith->high - arith->low + 1;
  weight = arith->contexts[i].count0 + arith->contexts[i].count1;
  scaled_count0 = ((unsigned int)arith->contexts[i].count0 * division_factor[weight - 1]) >> 21;

  carid_arith_context_update (arith, i, value);
  
  //printf("  %d [%04x %04x] -> ", value, arith->low, arith->high);
  if (value == 0) {
    arith->high = arith->low + ((range * scaled_count0)>>10) - 1;
  } else {
    arith->low = arith->low + ((range * scaled_count0)>>10);
    //arith->code = arith->code + ((range * scaled_count0)>>10);
  }
  //printf("[%04x %04x]\n", arith->low, arith->high);

  do {
    if ((arith->high & (1<<15)) == (arith->low & (1<<15))) {
//printf("  shift %d\n", arith->low >> 15);
      carid_arith_output_bit(arith);

      arith->low <<= 1;
      arith->high <<= 1;
      arith->high++;
    } else if ((arith->low & (1<<14)) && !(arith->high & (1<<14))) {
//printf("  underflow\n");
      arith->low ^= (1<<14);
      arith->high ^= (1<<14);
      //arith->code ^= (1<<14);

      arith->low <<= 1;
      arith->high <<= 1;
      arith->high++;
      arith->cntr++;
    } else {
      break;
    }

#if 0
    printf("shift: high=%04x low=%04x code=%04x -> ",
        arith->high, arith->low, arith->code);
#endif

#if 0
    printf("high=%04x low=%04x code=%04x\n",
        arith->high, arith->low, arith->code);
#endif
  } while (1);
}





void
carid_arith_context_encode_uu (CaridArith *arith, int context, int value)
{
  int i;

  for(i=0;i<value;i++){
    carid_arith_context_binary_encode (arith, context, 0);
  }
  carid_arith_context_binary_encode (arith, context, 1);
}

void
carid_arith_context_encode_su (CaridArith *arith, int context, int value)
{
  int i;
  int sign;

  if (value==0) {
    carid_arith_context_binary_encode (arith, context, 1);
    return;
  }
  if (value < 0) {
    sign = 0;
    value = -value;
  } else {
    sign = 1;
  }
  for(i=0;i<value;i++){
    carid_arith_context_binary_encode (arith, context, 0);
  }
  carid_arith_context_binary_encode (arith, context, 1);
  carid_arith_context_binary_encode (arith, context, sign);
}

void
carid_arith_context_encode_ut (CaridArith *arith, int context, int value, int max)
{
  int i;

  for(i=0;i<value;i++){
    carid_arith_context_binary_encode (arith, context, 0);
  }
  if (value < max) {
    carid_arith_context_binary_encode (arith, context, 1);
  }
}

static int
maxbit (unsigned int x)
{
  int i;
  for(i=0;x;i++){
    x >>= 1;
  }
  return i;
}

void
carid_arith_context_encode_uegol (CaridArith *arith, int context, int value)
{
  int i;
  int n_bits;

  value++;
  n_bits = maxbit(value);
  for(i=0;i<n_bits - 1;i++){
    carid_arith_context_binary_encode (arith, context, 0);
  }
  carid_arith_context_binary_encode (arith, context, 1);
  for(i=0;i<n_bits - 1;i++){
    carid_arith_context_binary_encode (arith, context, (value>>(n_bits - 2 - i))&1);
  }
}

void
carid_arith_context_encode_segol (CaridArith *arith, int context, int value)
{
  int sign;

  if (value < 0) {
    sign = 0;
    value = -value;
  } else {
    sign = 1;
  }
  carid_arith_context_encode_uegol (arith, context, value);
  if (value) {
    carid_arith_context_binary_encode (arith, context, sign);
  }
}

void
carid_arith_context_encode_ue2gol (CaridArith *arith, int context, int value)
{
  int i;
  int n_bits;

  value++;
  n_bits = maxbit(value);
  carid_arith_context_encode_uegol (arith, context, n_bits - 1);
  for(i=0;i<n_bits - 1;i++){
    carid_arith_context_binary_encode (arith, context, (value>>(n_bits - 2 - i))&1);
  }
}

void
carid_arith_context_encode_se2gol (CaridArith *arith, int context, int value)
{
  int sign;

  if (value < 0) {
    sign = 0;
    value = -value;
  } else {
    sign = 1;
  }
  carid_arith_context_encode_ue2gol (arith, context, value);
  if (value) {
    carid_arith_context_binary_encode (arith, context, sign);
  }
}

