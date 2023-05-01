#include "usermodfx.h"
#include "dsp/biquad.h"

#define k_samplerate_recipf (2.08333333333333e-005f)

static biquad bq;
static biquad *bqp = &bq;

static float freq;
static float res;

void MODFX_INIT(uint32_t platform, uint32_t api) {
  (void)platform; (void)api;
  biquad_flush(bqp);
}

void MODFX_PROCESS(const float *main_xn, float *main_yn,
                   const float *sub_xn, float *sub_yn,
                   uint32_t frames) {
  (void)*sub_xn; (void)*sub_yn;
  
  for (uint32_t i = 0; i < frames; i++) {
    const float in_l   = main_xn[i * 2];
    const float in_r   = main_xn[i * 2 + 1];

    const float out_m  = biquad_process_so(bqp, (in_l + in_r) / 2);
    
    main_yn[i * 2]     = out_m;
    main_yn[i * 2 + 1] = out_m;
  }
}

void MODFX_PARAM(uint8_t idx, int32_t val) {
  const float val_f = q31_to_f32(val);

  switch (idx) {
  case k_user_modfx_param_time:
    freq = 40.0f + (clip01f(-0.1f * fastlog2f(clip01f(1.0f - val_f)))) * 16000;
    break;
  case k_user_modfx_param_depth:
    res = 1.0f + val_f * 9.0f;
    break;
  default:
    break;
  }

  biquad_so_lp(bqp,
	       fasttanfullf(M_PI * biquad_wc(freq,
					     k_samplerate_recipf)),
	       res);
}
