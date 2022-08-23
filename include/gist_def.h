#pragma once
#ifndef GIST_DEF_H
#define GIST_DEF_H

/* Support header include for C++ */
#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

typedef struct Gist {
  char* id;
  char* desc;
  char* conts;
  char* filename;
  char* creation;
  char* modified;
} Gist;

#ifdef __cplusplus
}
#endif
#endif // GIST_DEF_H
