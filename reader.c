/*-
 * Modifications : Roman Oberenkowski, Pawel Szalczyk (for ASB project)
 *
 * Original example by:
 * Free/Libre Near Field Communication (NFC) library
 *
 * Libnfc historical contributors:
 * Copyright (C) 2009      Roel Verdult
 * Copyright (C) 2009-2013 Romuald Conty
 * Copyright (C) 2010-2012 Romain Tartière
 * Copyright (C) 2010-2013 Philippe Teuwen
 * Copyright (C) 2012-2013 Ludovic Rousseau
 * See AUTHORS file for a more comprehensive list of contributors.
 * Additional contributors of this file:
 * Copyright (C) 2020      Adam Laurie
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  1) Redistributions of source code must retain the above copyright notice,
 *  this list of conditions and the following disclaimer.
 *  2 )Redistributions in binary form must reproduce the above copyright
 *  notice, this list of conditions and the following disclaimer in the
 *  documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Note that this license only applies on the examples, NFC library itself is
 * under LGPL
 *
 */

/**
 * @file nfc-poll.c
 * @brief Polling example
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <err.h>
#include <inttypes.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <nfc/nfc-types.h>
#include <nfc/nfc.h>

#define MAX_DEVICE_COUNT 16

static nfc_device *pnd = NULL;
static nfc_context *context;

const bool human_readable_output = false;

long int get_num_uid(const uint8_t *pbtData, const size_t szBytes) {
  int szPos;
  long int numeric_id = 0;
  long int multi = 1;
  for (szPos = szBytes - 1; szPos >= 0; szPos--) {
    numeric_id += multi * pbtData[szPos];
    multi *= 256;
  }
  return numeric_id;
}

void print_hex(const uint8_t *pbtData, const size_t szBytes) {
  int szPos;

  for (szPos = 0; szPos < szBytes; szPos++) {
    printf("%02x  ", pbtData[szPos]);
    // printf("%d ", pbtData[szPos]);
  }
}

static void stop_polling(int sig) {
  (void)sig;
  if (pnd != NULL)
    nfc_abort_command(pnd);
  else {
    nfc_exit(context);
    exit(EXIT_FAILURE);
  }
}

int main(int argc, const char *argv[]) {
  bool verbose = false;

  signal(SIGINT, stop_polling);

  // Display libnfc version
  const char *acLibnfcVersion = nfc_version();

  if (human_readable_output) {
    printf("Using libnfc %s\n", acLibnfcVersion);
    printf("Tested on 1.8.0 \n");
  }

  const uint8_t uiPollNr = 20;
  const uint8_t uiPeriod = 2;
  const nfc_modulation nmModulations[] = {
      {.nmt = NMT_ISO14443A, .nbr = NBR_106},
      {.nmt = NMT_ISO14443B, .nbr = NBR_106},
      {.nmt = NMT_FELICA, .nbr = NBR_212},
      {.nmt = NMT_FELICA, .nbr = NBR_424},
      {.nmt = NMT_JEWEL, .nbr = NBR_106},
  };
  const size_t szModulations = sizeof(nmModulations) / sizeof(nfc_modulation);

  nfc_target nt;
  int res = 0;

  nfc_init(&context);
  if (context == NULL) {
    perror("Unable to init libnfc (malloc)");
    exit(EXIT_FAILURE);
  }

  pnd = nfc_open(context, NULL);

  if (pnd == NULL) {
    printf("%s", "Unable to open NFC device.");
    nfc_exit(context);
    exit(EXIT_FAILURE);
  }

  if (nfc_initiator_init(pnd) < 0) {
    nfc_perror(pnd, "nfc_initiator_init");
    nfc_close(pnd);
    nfc_exit(context);
    exit(EXIT_FAILURE);
  }
  if (human_readable_output) {
    printf("NFC reader: %s opened\n", nfc_device_get_name(pnd));
    printf("Waiting for card...\n");
  }
  while (true) {

    if ((res = nfc_initiator_poll_target(pnd, nmModulations, szModulations,
                                         uiPollNr, uiPeriod, &nt)) < 0) {
      if (res == NFC_ECHIP)
        continue; // my reader timeouts with this code...
      nfc_perror(pnd, "nfc_initiator_poll_target");
      printf("revolution");
      nfc_close(pnd);
      nfc_exit(context);
      exit(EXIT_FAILURE);
    }

    if (res > 0) {
      // print_nfc_target(&nt, verbose);
      if (human_readable_output) {
        printf("DETECTED: \n");
        printf(" UID (NFCID%c): ", (nt.nti.nai.abtUid[0] == 0x08 ? '3' : '1'));
        print_hex(nt.nti.nai.abtUid, nt.nti.nai.szUidLen);
        printf(" Decimal UID: %ld\n",
               get_num_uid(nt.nti.nai.abtUid, nt.nti.nai.szUidLen));
        printf("Waiting for card to be removed");
        fflush(stdout);
      } else {
        printf("%ld\n", get_num_uid(nt.nti.nai.abtUid, nt.nti.nai.szUidLen));
        fflush(stdout);
      }

      while (0 == nfc_initiator_target_is_present(pnd, NULL)) {
        if (human_readable_output) {
          printf(".");
          fflush(stdout);
        }
        sleep(0.500);
      }
      if (human_readable_output) {
        printf("\nCard removed, waiting for next card...\n");
      }
    } else {
      // printf("Nothing found, ");
    }
  }
  nfc_close(pnd);
  nfc_exit(context);
  exit(EXIT_SUCCESS);
}
