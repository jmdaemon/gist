#pragma once
#ifndef GIST_H
#define GIST_H

/* Support header include for C++ */
#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

typedef struct Gist {
  char* id;
  char* desc;
  char* filename;
  char* creation;
  char* modified;
} Gist;

#ifdef __cplusplus
}
#endif
#endif // GIST_H
