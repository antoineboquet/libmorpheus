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
  return(0);
}
