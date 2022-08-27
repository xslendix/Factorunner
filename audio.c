#include "audio.h"

#ifdef PSP
#include <pspaudio.h>
#include <pspaudiolib.h>
#include <pspaudio_kernel.h>

#include "stb_vorbis.c"
#endif

#ifdef RAYLIB
#include <raylib.h>
#endif

double sfx_level = 1;

#ifdef PSP
int sfx_num = 0;

struct sfx sounds[4];

int toggle_pickup = 0;

int channels[4] = {-1};

void
audio_init(void)
{
  pspAudioInit();

  sounds[0].samples = stb_vorbis_decode_filename("assets/death.ogg",  &sounds[0].channels, &sounds[0].sample_rate, &sounds[0].output);
  sounds[1].samples = stb_vorbis_decode_filename("assets/next.ogg",   &sounds[1].channels, &sounds[1].sample_rate, &sounds[1].output);
  sounds[2].samples = stb_vorbis_decode_filename("assets/pickup.ogg", &sounds[2].channels, &sounds[2].sample_rate, &sounds[2].output);
  sounds[3].samples = stb_vorbis_decode_filename("assets/select.ogg", &sounds[3].channels, &sounds[3].sample_rate, &sounds[3].output);

  channels[0] = sceAudioChReserve(0, PSP_AUDIO_SAMPLE_ALIGN(sounds[0].samples), PSP_AUDIO_FORMAT_MONO);
  channels[1] = sceAudioChReserve(1, PSP_AUDIO_SAMPLE_ALIGN(sounds[2].samples), PSP_AUDIO_FORMAT_MONO);
  channels[2] = sceAudioChReserve(2, PSP_AUDIO_SAMPLE_ALIGN(sounds[2].samples), PSP_AUDIO_FORMAT_MONO);
  channels[3] = sceAudioChReserve(3, PSP_AUDIO_SAMPLE_ALIGN(sounds[3].samples), PSP_AUDIO_FORMAT_MONO);
}

void
audio_sfx_play(int id)
{
  int cid = id;
  if (channels[cid] < 0) return;
  if (id == 2) {
    if (toggle_pickup) {
      if (channels[1] >= 0) {
        cid=1;
      }
    } else {
      if (channels[2] >= 0) {
        cid=2;
      }
    }

    toggle_pickup = !toggle_pickup;
  }

  sceAudioOutput(channels[cid], sfx_level * 0x8000, sounds[id].output);
}

void
audio_free(void)
{
  int i;
  
  pspAudioEnd();

  for (i = 0; i < 4; i++)
    free(sounds[i].output);
}

#endif

#ifdef RAYLIB
Sound sounds[4];

void
audio_init(void)
{
  InitAudioDevice();

  sounds[0] = LoadSound("assets/death.ogg");
  sounds[1] = LoadSound("assets/next.ogg");
  sounds[2] = LoadSound("assets/pickup.ogg");
  sounds[3] = LoadSound("assets/select.ogg");
}

void
audio_free(void)
{
  int i;
  for (i = 0; i < 4; i++)
    UnloadSound(sounds[i]);
  CloseAudioDevice();
}

void
audio_sfx_play(int id)
{
  if (!IsAudioDeviceReady()) return;

  PlaySound(sounds[id]);
}
#endif

