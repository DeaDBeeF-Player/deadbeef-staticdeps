/*
  Copyright (C) 2013 Rocky Bernstein <rocky@gnu.org>
  Copyright (C) 2014 Robert Kausch <robert.kausch@freac.org>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
   Regression test for lib/cdda_interface/toc.c

   To compile as s standalone program:

 gcc -g3 -Wall -DHAVE_CONFIG_H -I../.. -I../..include toc.c \
      ../../lib/paranoia/libcdio_paranoia.la ../../lib/cdda_interface/.lib/libcdio_cdda.a -lcdio -o toc

*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#define __CDIO_CONFIG_H__ 1
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include <cdio/paranoia/cdda.h>
#include <cdio/cd_types.h>
#include <cdio/logging.h>

#ifndef DATA_DIR
#define DATA_DIR "./../data"
#endif

static void
log_handler (cdio_log_level_t level, const char message[])
{
  switch(level) {
  case CDIO_LOG_DEBUG:
  case CDIO_LOG_INFO:
    return;
  default:
    printf("cdio %d message: %s\n", level, message);
  }
}

static int testit(cdrom_drive_t *d) {
    track_t t = 0;
    lsn_t lsn = 0;

    /* Check number of tracks.
     */
    t = cdda_tracks(d);
    if (t != 1) {
	printf("Should have found one track, got %d\n", t);
	return 1;
    }

    /* Check disc sector reporting.
     */
    lsn = cdda_disc_firstsector(d);
    if ( lsn != 0 ) {
	printf("Should have gotten 0 as the first sector, got %d.\n", lsn);
	return 2;
    }

    lsn = cdda_disc_lastsector(d);
    if ( lsn != 301 ) {
	printf("Should have gotten 301 as the last sector, got %d.\n", lsn);
	return 2;
    }

    /* Check pregap sector reporting (should fail here).
     */
    lsn = cdda_track_firstsector(d, 0);
    if ( lsn != -402 ) {
	printf("Should have gotten error code -402, got %d.\n", lsn);
	return 3;
    }

    lsn = cdda_track_lastsector(d, 0);
    if ( lsn != -402 ) {
	printf("Should have gotten error code -402, got %d.\n", lsn);
	return 3;
    }

    /* Check track sector reporting.
     */
    lsn  = cdda_track_firstsector(d, 1);
    if ( lsn != 0 ) {
	printf("Should have gotten 0 as the first sector, got %d.\n", lsn);
	return 2;
    }

    lsn  = cdda_track_lastsector(d, 1);
    if ( lsn != 301 ) {
	printf("Should have gotten 301 as the last sector, got %d.\n", lsn);
	return 2;
    }

    /* Check sector track reporting.
     */
    t = cdda_sector_gettrack(d, 100);
    if ( 1 != t )  {
	printf("Should have gotten 1 as the track, got %d.\n", t);
	return 2;
    }

    t = cdda_sector_gettrack(d, 1000);
    if ( CDIO_INVALID_TRACK != t )  {
	printf("Should have gotten CDIO_INVALID_TRACK as the track, got %d.\n", t);
	return 3;
    }

    return 0;
}

int
main(int argc, const char *argv[])
{
  cdrom_drive_t *d = NULL; /* Place to store handle given by cd-paranoia. */
  CdIo_t *p_cdio;
  int rc;

  cdio_log_set_handler (log_handler);

  if (cdio_have_driver(DRIVER_BINCUE)) {
      p_cdio = cdio_open("/src/external-vcs/github/rocky/libcdio-paranoia/test/data/cdda.cue", DRIVER_UNKNOWN);
  } else if (cdio_have_driver(DRIVER_CDRDAO)) {
      p_cdio = cdio_open("/src/external-vcs/github/rocky/libcdio-paranoia/test/data/cdda.toc", DRIVER_UNKNOWN);
  } else {
    printf("-- You don't have enough drivers for this test\n");
    return 77;
  }

  d=cdio_cddap_identify_cdio(p_cdio, CDDA_MESSAGE_PRINTIT, NULL);

  if ( !d ) {
    printf("Should have identified as an audio CD disc.\n");
    return 1;
  }

  if ( 0 != cdio_cddap_open(d) ) {
    printf("Unable to open disc.\n");
    return 2;
  }

  rc = testit(d);
  cdio_cddap_close_no_free_cdio(d);
  cdio_destroy(p_cdio);

  return rc;
}
