// SPDX-License-Identifier: AGPL-3.0-or-later

#include <stddef.h>
#include <stdint.h>

#include <morpheus/morpheus.h>

int
main(void)
{
  if(morpheus_abi_version() != MORPHEUS_ABI_VERSION)
    return(1);
  if(morpheus_analysis_size() != sizeof(morpheus_analysis))
    return(1);
  if(morpheus_generation_size() != sizeof(morpheus_generation))
    return(1);
  {
    morpheus_generation_options options={0};
    options.version=MORPHEUS_GENERATION_OPTIONS_VERSION;
    options.struct_size=sizeof options;
    options.result_limit=MORPHEUS_GENERATION_DEFAULT_LIMIT;
    if(options.result_limit > MORPHEUS_GENERATION_MAX_LIMIT)
      return(1);
  }
  return(0);
}
