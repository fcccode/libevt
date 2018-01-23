/*
 * Functions for testing
 *
 * Copyright (C) 2011-2018, Joachim Metz <joachim.metz@gmail.com>
 *
 * Refer to AUTHORS for acknowledgements.
 *
 * This software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this software.  If not, see <http://www.gnu.org/licenses/>.
 */

#if !defined( _EVT_TEST_FUNCTIONS_H )
#define _EVT_TEST_FUNCTIONS_H

#include <common.h>
#include <types.h>

#include "evt_test_libcerror.h"

#if defined( __cplusplus )
extern "C" {
#endif

int evt_test_get_narrow_source(
     const system_character_t *source,
     char *narrow_string,
     size_t narrow_string_size,
     libcerror_error_t **error );

#if defined( HAVE_WIDE_CHARACTER_TYPE )

int evt_test_get_wide_source(
     const system_character_t *source,
     wchar_t *wide_string,
     size_t wide_string_size,
     libcerror_error_t **error );

#endif /* defined( HAVE_WIDE_CHARACTER_TYPE ) */

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _EVT_TEST_FUNCTIONS_H ) */
