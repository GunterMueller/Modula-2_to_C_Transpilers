/* Header file for library of m2c.
 * Copyright 1993, 1994, 1995, 1996, 1997 Vladimir Makarov
 * This file is part of m2c.
 *
 * m2c is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * m2c is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with m2c. If not, see <http://www.gnu.org/licenses/>.
 */

/* All dynamic memory is allocated and freed with the aid of
   the following macros. */

#define M2C_ALLOC(str,size) (((str)=malloc(size))?(str)\
                             :(error(ERR_no_memory),((char *)0)))

#define M2C_FREE(str) ((str)?free(str),0:0)

#define M2C_REALLOC(new,old,size) (((new)=realloc(old,size))?(new)\
				   :(error(ERR_no_memory),((char *)0)))



/* Macros for work with variable length strings (VLS).  Any number of
   characters my be added to and removed from the end of VLS.  If it is
   needed the memory allocated for storing variable length string may be
   expanded possibly with changing the string place.  But between any
   additions of the characters (or tailoring) the string place is not changed.
   To decrease changing the string place the memory being allocated for
   the string is longer than the current string length. */


/* Descriptor of variable length string is of the following type.  All work
   with variable length string is executed by the following macros through
   the descriptors.  Structure (implementation) of this type is not needed
   for using variable length string.  But it should remember that work with
   the string through several descriptors is not safe. */

typedef struct
{
  /* Pointer to memory currently used for storing the string. */
  char *vls_start;
  /* Pointer to first byte after the last string character. */
  char *vls_free;
  /* Pointer to first byte after the memory currently allocated for storing
     the string. */
  char *vls_boundary;
} VLS;


/* Default initial size of memory is allocated for variable length string when
   the string is created (with zero initial size). */

#define VLS_DEFAULT_LENGTH 512


/* Macro call value is TRUE if VLS is undefined (i.e. memory for VLS
   is undefined). */

#define VLS_IS_UNDEFINED(vls) (!(vls).vls_start)


/* Make VLS as undefined (without freeing memory allocated for it). */

#define VLS_UNDEFINE(vls) ((vls).vls_start=NULL)


/* Macros used for creation of VLS.  Initial memory for VLS is allocated
   but length of the created VLS is zero. */

#define VLS_CREATE(vls,init_length) (\
 M2C_ALLOC((vls).vls_start,(unsigned)(init_length\
				      ?init_length:VLS_DEFAULT_LENGTH)),\
 (vls).vls_boundary=(vls).vls_start+(init_length\
				     ?init_length:VLS_DEFAULT_LENGTH),\
				     (vls).vls_free=(vls).vls_start)


/* Macros used for freeing memory used by VLS. */

#define VLS_DELETE(vls) ((vls).vls_start?(free((vls).vls_start),0):(abort(),0))


/* Make length of VLS equal to zero (but memory for VLS is not freed). */

#define VLS_NULLIFY(vls) ((vls).vls_free=(vls).vls_start)


/* Length of memory allocated for VLS becames equal to VLS length. */

#define VLS_TAILOR(vls) VLS_tailor_function(&vls)


/* Value of the following macros is current VLS length. */

#define VLS_LENGTH(vls) ((vls).vls_start\
			 ?(vls).vls_free-(vls).vls_start:(abort(),0))


/* Return pointer to first character of VLS.  Remember that the string may
   change own place after any addition. */

#define VLS_BEGIN(vls) ((vls).vls_start?(vls).vls_start:(abort(),(char *)0))


/* Return pointer to last character of VLS.  Remember that the string may
   change own place after any addition. */

#define VLS_END(vls) ((vls).vls_start?(vls).vls_free-1:(abort(),(char *)0))


/* Remove N characters from the end of VLS.  VLS is nullified if its length
   is less than N. */

#define VLS_SHORTEN(vls,n) (!(vls).vls_start?abort(),(char *)0:\
 ((vls).vls_free=(VLS_LENGTH(vls)<n?(vls).vls_start:(vls).vls_free-(n))))


/* Add character C to the end of VLS. */

#define VLS_ADD_CHAR(vls,c) (!(vls).vls_start?abort(),0:\
 (((vls).vls_free>=(vls).vls_boundary\
   ?(VLS_expand(&(vls),1),0):0),*(vls).vls_free++=(c)))


/* Add memory starting with STR and with length LEN to the end of VLS. */

#define VLS_ADD_MEMORY(vls,str,len) (!(vls).vls_start?abort(),((char *)0):\
  (((vls).vls_free+len>(vls).vls_boundary?VLS_expand(&(vls),(len)),0:0),\
   memcpy((vls).vls_free,(str),(len)),(vls).vls_free+=(len)))


/* Add string STR (with end marker is '\0') to the end of VLS. */

#define VLS_ADD_STRING(vls,str) VLS_add_string_function(&(vls),str)


extern void VLS_tailor_function (), VLS_add_string_function (), VLS_expand ();



extern int source_line_number;
extern char *input_file_name;

extern int argument_count;
extern VLS argument_vector;

extern int command_tune ();
extern void insert_argument ();
extern void output_description_of_flags ();
extern int next_parameter ();
extern int number_of_parameters ();
extern int next_flag ();
extern char *flag_characteristics ();
extern int last_flag_place ();
extern char *flag_parameter ();

extern char *file_name_without_suffix ();
extern char *file_name_suffix ();
extern char *base_file_name ();
extern void test_input_file_name ();
extern int value_of_hexadecimal_digit ();
extern int M2_cap ();



/* The following functions and structures implement hash tables in the
   translator. */


/* The hash table element is represented by the following type. */

typedef char *hash_table_entry;


/* Hash tables are of the following type.  The structure (implementation)
   of this type is not needed for using the hash tables. */

typedef struct
{

  unsigned size;		/* current size (in entries) of the hash table */
  unsigned number_of_elements;	/* current number of elements */

  /* Pointer to function for evaluation of hash value (any unsigned value).
     This function has one parameter with type hash_table_entry. */
  unsigned (*hash_function) ();

  /* Pointer to function for comparison of hash table elements (two parameter
     with type hash_table_entry). */
  int (*compare_function) ();
  hash_table_entry *entries;	/* table itself */
} *hash_table;

extern hash_table create_hash_table ();
extern void delete_hash_table ();
extern hash_table_entry *find_hash_table_entry ();
