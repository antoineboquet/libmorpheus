/* SPDX-License-Identifier: MPL-2.0 */

#ifndef MORPHEUS_LEGACY_VALUES_H
#define MORPHEUS_LEGACY_VALUES_H

#include <morpheus/morpheus.h>

morpheus_part_of_speech morpheus_public_part_of_speech(
    uint32_t legacy_stemtype);
uint32_t morpheus_public_dialect(uint32_t legacy);
uint32_t morpheus_legacy_dialect(uint32_t public_value);
uint32_t morpheus_public_region(uint32_t legacy);
uint32_t morpheus_public_person(uint32_t legacy);
uint32_t morpheus_public_number(uint32_t legacy);
uint32_t morpheus_public_gender(uint32_t legacy);
uint32_t morpheus_public_case(uint32_t legacy);
uint32_t morpheus_public_tense(uint32_t legacy);
uint32_t morpheus_public_mood(uint32_t legacy);
uint32_t morpheus_public_voice(uint32_t legacy);
uint32_t morpheus_public_degree(
    uint32_t legacy, morpheus_part_of_speech part_of_speech,
    const unsigned char *legacy_flags);
void morpheus_public_morph_flags(
    uint8_t destination[MORPHEUS_MORPH_FLAG_CAPACITY],
    const unsigned char *legacy_flags);

#endif
