#ifndef SBS_CONFIG_H
#define SBS_CONFIG_H

#include "sbs_buffer.h"

int sbs_config_initialize(int rank, sbs_buffer buffer);
int sbs_config_new_step(int step);
#endif
