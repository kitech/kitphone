/* Copyright (C) 2010 - AlannY
 * 
 * This file is part of Bencode.
 * 
 * Bencode is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * Bencode is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with Bencode; If not, see <http://www.gnu.org/licenses/>.
 */
/** 
 * @file bencode.h
 * @brief Bencode
 * @author AlannY
 * @version 1.0
 * @date 2010-03-24
 */

#ifndef __BENCODE_H__
#define __BENCODE_H__

#define BENCODE_VERSION 10000                                   /**< @brief Current version */

#ifndef BENCODE_DLL_EXPORTED
#ifdef _MSC_VER
#ifdef IN_BENCODE
#define BENCODE_DLL_EXPORTED __declspec(dllexport)
#else
#define BENCODE_DLL_EXPORTED __declspec(dllimport)
#endif
#else
#define BENCODE_DLL_EXPORTED                                    /**< @brief Export symbols */
#endif
#endif

typedef /*@abstract@*/ struct bencode_t bencode_t;              /**< @brief Bencode */
typedef /*@abstract@*/ struct bencode_list_t bencode_list_t;    /**< @brief List */
typedef /*@abstract@*/ struct bencode_dict_t bencode_dict_t;    /**< @brief Dictionary */

enum
{
  BENCODE_INTEGER = 1,                                          /**< @brief Integer */
  BENCODE_STRING = 2,                                           /**< @brief String */
  BENCODE_LIST = 3,                                             /**< @brief List */
  BENCODE_DICTIONARY = 4                                        /**< @brief Dictionary */
};

enum
{
  BENCODE_IGNORE_ERRORS = 1 << 0                                /**< @brief Ignore non-fatal errors */
};

enum
{
  BENCODE_NO_ERROR = 0,                                         /**< @brief No error */
  BENCODE_WAS_ERROR = 1,                                        /**< @brief Was error */
  BENCODE_ERROR_NULLARG = -10,                                  /**< @brief Required argument is NULL */
  BENCODE_ERROR_NO_MEMORY = -11,                                /**< @brief No memory */
  BENCODE_ERROR_BAD_DATA = -12                                  /**< @brief Bad data */
};

#ifdef __cplusplus
extern "C" {
#endif

BENCODE_DLL_EXPORTED /*@null@*/ /*@only@*/ bencode_t*           bdecode (/*@null@*/ const char *data,
                                                                                    size_t size,
                                                                                    int flags,
                                                                         /*@null@*/ int *result)
                                                                         /*@modifies result@*/;
BENCODE_DLL_EXPORTED /*@null@*/ /*@only@*/ char*                bencode (/*@null@*/ bencode_t *root,
                                                                         /*@null@*/ size_t *size,
                                                                         /*@null@*/ int *result)
                                                                         /*@modifies size,result@*/;

BENCODE_DLL_EXPORTED /*@null@*/ /*@only@*/ bencode_t*           bencode_new_from_integer (int integer)
                                                                                          /*@*/;
BENCODE_DLL_EXPORTED /*@null@*/ /*@only@*/ bencode_t*           bencode_new_from_string (/*@null@*/ const char *str,
                                                                                                    size_t size)
                                                                                         /*@*/;
BENCODE_DLL_EXPORTED /*@null@*/ /*@only@*/ bencode_t*           bencode_new_from_list (/*@null@*/ bencode_list_t *list)
                                                                                       /*@*/;
BENCODE_DLL_EXPORTED /*@null@*/ /*@only@*/ bencode_t*           bencode_new_from_dict (/*@null@*/ bencode_dict_t *dict)
                                                                                       /*@*/;

BENCODE_DLL_EXPORTED /*@null@*/ bencode_t*                      bencode_delete (/*@null@*/ /*@only@*/ bencode_t *node)
                                                                                /*@modifies node@*/;
BENCODE_DLL_EXPORTED /*@null@*/ bencode_list_t*                 bencode_list_delete (/*@null@*/ /*@only@*/ bencode_list_t *list)
                                                                                     /*@modifies list@*/;
BENCODE_DLL_EXPORTED /*@null@*/ bencode_dict_t*                 bencode_dict_delete (/*@null@*/ /*@only@*/ bencode_dict_t *dict)
                                                                                     /*@modifies dict@*/;
BENCODE_DLL_EXPORTED /*@null@*/ char*                           bencode_data_delete (/*@null@*/ /*@only@*/ char *data,
                                                                                                           size_t size)
                                                                                     /*@modifies data@*/;

BENCODE_DLL_EXPORTED int                                        bencode_set_integer (/*@null@*/ bencode_t *node,
                                                                                                int integer)
                                                                                     /*@modifies node@*/;
BENCODE_DLL_EXPORTED int                                        bencode_set_string (/*@null@*/ bencode_t *node,
                                                                                    /*@null@*/ const char *string,
                                                                                               size_t size)
                                                                                    /*@modifies node@*/;
BENCODE_DLL_EXPORTED int                                        bencode_set_list (/*@null@*/ bencode_t *node,
                                                                                  /*@null@*/ bencode_list_t *list)
                                                                                  /*@modifies node@*/;
BENCODE_DLL_EXPORTED int                                        bencode_set_dict (/*@null@*/ bencode_t *node,
                                                                                  /*@null@*/ bencode_dict_t *dict)
                                                                                  /*@modifies node@*/;

BENCODE_DLL_EXPORTED int                                        bencode_get_type (/*@null@*/ const bencode_t *node)
                                                                                  /*@*/;
BENCODE_DLL_EXPORTED int                                        bencode_get_integer (/*@null@*/ const bencode_t *node)
                                                                                     /*@*/;
BENCODE_DLL_EXPORTED /*@null@*/ /*@observer@*/ const char*      bencode_get_string (/*@null@*/ const bencode_t *node,
                                                                                    /*@null@*/ size_t *size)
                                                                                    /*@modifies size@*/;
BENCODE_DLL_EXPORTED /*@null@*/ bencode_list_t*                 bencode_get_list (/*@null@*/ const bencode_t *node)
                                                                                  /*@*/;
BENCODE_DLL_EXPORTED /*@null@*/ bencode_dict_t*                 bencode_get_dict (/*@null@*/ const bencode_t *node)
                                                                                  /*@*/;

BENCODE_DLL_EXPORTED /*@null@*/ bencode_list_t*                 bencode_list_append (/*@null@*/ /*@returned@*/ bencode_list_t *list,
                                                                                     /*@null@*/                bencode_t *value)
                                                                                     /*@modifies list@*/;
BENCODE_DLL_EXPORTED /*@null@*/ bencode_list_t*                 bencode_list_remove (/*@null@*/ /*@returned@*/ bencode_list_t *list,
                                                                                     /*@null@*/                bencode_t *value)
                                                                                     /*@modifies list@*/;
BENCODE_DLL_EXPORTED /*@null@*/ bencode_dict_t*                 bencode_dict_assign (/*@null@*/ /*@returned@*/ bencode_dict_t *dict,
                                                                                     /*@null@*/                const char *key,
                                                                                                               size_t size,
                                                                                     /*@null@*/                bencode_t *value)
                                                                                     /*@modifies dict@*/;
BENCODE_DLL_EXPORTED /*@null@*/ bencode_dict_t*                 bencode_dict_remove (/*@null@*/ /*@returned@*/ bencode_dict_t *dict,
                                                                                     /*@null@*/                const char *key,
                                                                                                               size_t size)
                                                                                     /*@modifies dict@*/;
BENCODE_DLL_EXPORTED /*@null@*/ bencode_t*                      bencode_list_item (/*@null@*/ const bencode_list_t *list)
                                                                                   /*@*/;
BENCODE_DLL_EXPORTED /*@null@*/ /*@observer@*/ const char*      bencode_dict_key (/*@null@*/ const bencode_dict_t *dict,
                                                                                  /*@null@*/ size_t *size)
                                                                                  /*@modifies size@*/;
BENCODE_DLL_EXPORTED /*@null@*/ bencode_t*                      bencode_dict_value (/*@null@*/ const bencode_dict_t *dict)
                                                                                    /*@*/;
BENCODE_DLL_EXPORTED /*@null@*/ bencode_list_t*                 bencode_list_next (/*@null@*/ bencode_list_t *list)
                                                                                   /*@*/;
BENCODE_DLL_EXPORTED /*@null@*/ bencode_dict_t*                 bencode_dict_at (/*@null@*/ /*@returned@*/ bencode_dict_t *dict,
                                                                                 /*@null@*/                const char *key,
                                                                                                           size_t size)
                                                                                 /*@*/;
BENCODE_DLL_EXPORTED /*@null@*/ bencode_dict_t*                 bencode_dict_iter_new (/*@null@*/ /*@returned@*/ bencode_dict_t *dict)
                                                                                       /*@*/;
BENCODE_DLL_EXPORTED /*@null@*/ bencode_dict_t*                 bencode_dict_iter_next (/*@null@*/ bencode_dict_t *dict)
                                                                                        /*@*/;

BENCODE_DLL_EXPORTED /*@observer@*/ const char*                 bencode_strerror (const int error)
                                                                                  /*@*/;


#ifdef __cplusplus
}
#endif

#endif
