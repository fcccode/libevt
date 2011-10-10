/*
 * Item functions
 *
 * Copyright (c) 2011, Joachim Metz <jbmetz@users.sourceforge.net>
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

#include <common.h>
#include <memory.h>
#include <types.h>

#include <liberror.h>

#include "libevt_definitions.h"
#include "libevt_io_handle.h"
#include "libevt_item.h"
#include "libevt_libbfio.h"

/* Initializes the item and its values
 * Returns 1 if successful or -1 on error
 */
int libevt_item_initialize(
     libevt_item_t **item,
     libevt_io_handle_t *io_handle,
     libbfio_handle_t *file_io_handle,
     libevt_record_t *event_record,
     uint8_t flags,
     liberror_error_t **error )
{
	libevt_internal_item_t *internal_item = NULL;
	static char *function                 = "libevt_item_initialize";

	if( item == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid item.",
		 function );

		return( -1 );
	}
	if( event_record == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid event record.",
		 function );

		return( -1 );
	}
	if( ( flags & ~( LIBEVT_ITEM_FLAG_MANAGED_FILE_IO_HANDLE ) ) != 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported flags: 0x%02" PRIx8 ".",
		 function,
		 flags );

		return( -1 );
	}
	if( *item == NULL )
	{
		internal_item = memory_allocate_structure(
		                 libevt_internal_item_t );

		if( internal_item == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create internal item.",
			 function );

			goto on_error;
		}
		if( memory_set(
		     internal_item,
		     0,
		     sizeof( libevt_internal_item_t ) ) == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_MEMORY,
			 LIBERROR_MEMORY_ERROR_SET_FAILED,
			 "%s: unable to clear internal item.",
			 function );

			memory_free(
			 internal_item );

			return( -1 );
		}
		if( ( flags & LIBEVT_ITEM_FLAG_MANAGED_FILE_IO_HANDLE ) == 0 )
		{
			internal_item->file_io_handle = file_io_handle;
		}
		else
		{
			if( libbfio_handle_clone(
			     &( internal_item->file_io_handle ),
			     file_io_handle,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy file IO handle.",
				 function );

				goto on_error;
			}
			if( libbfio_handle_set_open_on_demand(
			     internal_item->file_io_handle,
			     1,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to set open on demand in file IO handle.",
				 function );

				goto on_error;
			}
		}
		internal_item->io_handle    = io_handle;
		internal_item->event_record = event_record;
		internal_item->flags        = flags;

		*item = (libevt_item_t *) internal_item;
	}
	return( 1 );

on_error:
	if( internal_item != NULL )
	{
		if( ( flags & LIBEVT_ITEM_FLAG_MANAGED_FILE_IO_HANDLE ) != 0 )
		{
			if( internal_item->file_io_handle != NULL )
			{
				libbfio_handle_free(
				 &( internal_item->file_io_handle ),
				 NULL );
			}
		}
		memory_free(
		 internal_item );
	}
	return( -1 );
}

/* Frees an item
 * Returns 1 if successful or -1 on error
 */
int libevt_item_free(
     libevt_item_t **item,
     liberror_error_t **error )
{
	libevt_internal_item_t *internal_item = NULL;
	static char *function                 = "libevt_item_free";

	if( item == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid item.",
		 function );

		return( -1 );
	}
	if( *item != NULL )
	{
		internal_item = (libevt_internal_item_t *) *item;
		*item         = NULL;

		/* The io_handle and event_record references are freed elsewhere
		 */
		if( ( internal_item->flags & LIBEVT_ITEM_FLAG_MANAGED_FILE_IO_HANDLE ) != 0 )
		{
			if( internal_item->file_io_handle != NULL )
			{
				if( libbfio_handle_close(
				     internal_item->file_io_handle,
				     error ) != 0 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_IO,
					 LIBERROR_IO_ERROR_CLOSE_FAILED,
					 "%s: unable to close file IO handle.",
					 function );

					return( -1 );
				}
				if( libbfio_handle_free(
				     &( internal_item->file_io_handle ),
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
					 "%s: unable to free file IO handle.",
					 function );

					return( -1 );
				}
			}
		}
		memory_free(
		 internal_item );
	}
	return( 1 );
}

/* Retrieves the size of the UTF-8 encoded computer name
 * The string size includes the end of string character
 * Returns 1 if successful, 0 if value not present or -1 on error
 */
int libevt_item_get_utf8_computer_name_size(
     libevt_item_t *item,
     size_t *utf8_string_size,
     liberror_error_t **error )
{
	libevt_internal_item_t *internal_item = NULL;
	static char *function                 = "libevt_item_get_utf8_computer_name_size";

	if( item == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid item.",
		 function );

		return( -1 );
	}
	internal_item = (libevt_internal_item_t *) item;
/* TODO */

	return( 1 );
}

/* Retrieves the UTF-8 encoded header computer name
 * The string size should include the end of string character
 * Returns 1 if successful, 0 if value not present or -1 on error
 */
int libevt_item_get_utf8_computer_name(
     libevt_item_t *item,
     uint8_t *utf8_string,
     size_t utf8_string_size,
     liberror_error_t **error )
{
	libevt_internal_item_t *internal_item = NULL;
	static char *function                 = "libevt_item_get_utf8_computer_name";

	if( item == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid item.",
		 function );

		return( -1 );
	}
	internal_item = (libevt_internal_item_t *) item;
/* TODO */

	return( 1 );
}

/* Retrieves the size of the UTF-16 encoded computer name
 * The string size includes the end of string character
 * Returns 1 if successful, 0 if value not present or -1 on error
 */
int libevt_item_get_utf16_computer_name_size(
     libevt_item_t *item,
     size_t *utf16_string_size,
     liberror_error_t **error )
{
	libevt_internal_item_t *internal_item = NULL;
	static char *function                 = "libevt_item_get_utf16_computer_name_size";

	if( item == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid item.",
		 function );

		return( -1 );
	}
	internal_item = (libevt_internal_item_t *) item;
/* TODO */

	return( 1 );
}

/* Retrieves the UTF-16 encoded header computer name
 * The string size should include the end of string character
 * Returns 1 if successful, 0 if value not present or -1 on error
 */
int libevt_item_get_utf16_computer_name(
     libevt_item_t *item,
     uint16_t *utf16_string,
     size_t utf16_string_size,
     liberror_error_t **error )
{
	libevt_internal_item_t *internal_item = NULL;
	static char *function                 = "libevt_item_get_utf16_computer_name";

	if( item == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid item.",
		 function );

		return( -1 );
	}
	internal_item = (libevt_internal_item_t *) item;
/* TODO */

	return( 1 );
}

