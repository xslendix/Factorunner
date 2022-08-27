#pragma once

struct sfx {
  int id,
      channels,
      sample_rate,
      samples;
  short *output;
};

extern double sfx_level;

void audio_init(void);
void audio_free(void);
void audio_sfx_play(int id);

