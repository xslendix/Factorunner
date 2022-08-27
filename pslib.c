//typedef struct {} particle;
//typedef struct {} emitter;
//typedef struct {} emit_timer;
//
//typedef void(*draw_func)(void);
//
//typedef struct {} affector;
//
//struct ps {
//  u8 auto_remove;
//  int min_life, max_life;
//  int min_start_size, max_start_size;
//  int min_end_size, max_end_size;
//  particle *particles;
//  emitter *emitters;
//  emit_timer *emit_timers;
//  draw_func *draw_funcs;
//  affector *affectors;
//};
//
//struct ps *particle_systems;
//int particle_systems_count;
//
//void
//update_ps(struct ps *particle_system, int time_now)
//{
//  
//}
//
//void
//update_psystems()
//{
//  // FIXME: = time()
//  int time_now, i;
//  for (i = 0; i < particle_systems_count; i++) {
//    update_ps(&particle_systems[i], time_now);
//  }
//}
//
