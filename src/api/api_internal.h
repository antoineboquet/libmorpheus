#ifndef MORPHEUS_API_INTERNAL_H
#define MORPHEUS_API_INTERNAL_H
#include <morpheus/morpheus.h>
struct morpheus_result {
  size_t count;
  morpheus_analysis *analyses;
};
morpheus_result *morpheus_result_create(size_t count);
#endif
