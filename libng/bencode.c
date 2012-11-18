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
 * @file bencode.c
 * @brief Bencode
 *
 * This file can be compiled with the following definitions:
 * -# WITH_GETTEXT - Include support for gettext
 * -# LOCALEDIR - Set locale directory (only if WITH_GETTEXT)
 *
 * @author AlannY
 * @version 1.0
 * @date 2010-03-24
 */

/**
 * @mainpage Bencode
 *
 * @b Bencode is a bencode encoder/decoder.
 *
 * @b Bencode is free under terms of GNU LGPL.
 *
 * @section overview A short overview
 *
 * API:
 * -# bdecode() - Decode Bencode
 * -# bencode() - Encode Bencode
 * -# bencode_new_from_integer() - New node from integer
 * -# bencode_new_from_string() - New node form string
 * -# bencode_new_from_list() - New node from list
 * -# bencode_new_from_dict() - New node from dictionary
 * -# bencode_delete() - Delete node
 * -# bencode_list_delete() - Delete list
 * -# bencode_dict_delete() - Delete dictionary
 * -# bencode_data_delete() - Delete data
 * -# bencode_set_integer() - Set integer on node
 * -# bencode_set_string() - Set string on node
 * -# bencode_set_list() - Set list on node
 * -# bencode_set_dict() - Set dictionary on node
 * -# bencode_get_type() - Get type of node
 * -# bencode_get_integer() - Get integer
 * -# bencode_get_string() - Get string
 * -# bencode_get_list() - Get list
 * -# bencode_get_dict() - Get dictionary
 * -# bencode_list_append() - Append node to list
 * -# bencode_list_remove() - Remove node from list
 * -# bencode_dict_assign() - Assign key-value pair to dictionary
 * -# bencode_dict_remove() - Remove key-value pair from dictionary
 * -# bencode_list_item() - Get list item
 * -# bencode_dict_key() - Get key from dictionary
 * -# bencode_dict_value() - Get value from dictionary
 * -# bencode_list_next() - Get next list item
 * -# bencode_dict_at() - Search dictionary for key
 * -# bencode_dict_iter_new() - Start iteration over dictionary
 * -# bencode_dict_iter_next() - Get next entry in iteration
 * -# bencode_strerror() - Represent error code as string
 */

/*@-branchstate@*/

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <assert.h>

// #include <_new.h>
// #include <delog.h>
#define delog_enter(x) fprintf(stdout, "Enter %s\n", x)
#define delog_return(x) return (x)
#define delog_assert(x) assert(x)
#define delog_void_return()  return
#define delog_print(x, y)  assert(1==1);
#define _new(x)     malloc(x)
#define _rea(x, y)   realloc(x, y)
#define _zero(x, y)  memset(x, 0, y)
#define _delete(x)   free(x)

#include "bencode.h"

#ifndef EOL
#define EOL '\0'                        /**< @brief End of line */
#endif

#ifdef WITH_GETTEXT
#ifdef HAVE_LIBINTL_H
#include <libintl.h>
#endif /* HAVE_LIBINTL_H */
#define _(s) dgettext ("bencode", s)
#define pvoid (void)
#else /* WITH_GETTEXT */
#define _(s) s                          /**< @brief gettext function */
#define bindtextdomain(a,b)             /**< @brief bindtextdomain function */
#define pvoid                           /**< @brief Dummy void */
#endif /* WITH_GETTEXT */

#ifndef N_
#define N_(s) s                         /**< @brief gettext_noop function */
#endif

/**
 * @brief Bencode
 */
struct bencode_t
{
  int type;                             /**< @brief Type */
  union {
    int integer;                        /**< @brief Integer */
    struct {
      char *str;                        /**< @brief String */
      size_t size;                      /**< @brief String size */
    } string;                           /**< @brief String struct */
    bencode_list_t *list;               /**< @brief List */
    bencode_dict_t *dict;               /**< @brief Dictionary */
  } data;                               /**< @brief Data */
};

/**
 * @brief List
 */
struct bencode_list_t
{
           bencode_t *item;             /**< @brief Item */
/*@null@*/ bencode_list_t *next;        /**< @brief Next item */
};

/**
 * @brief Dictionary
 */
struct bencode_dict_t
{
           char *key;                   /**< @brief Key */
           size_t size;                 /**< @brief Key size */
           bencode_t *value;            /**< @brief Value */
/*@null@*/ bencode_dict_t *parent;      /**< @brief Parent in tree */
/*@null@*/ bencode_dict_t *left;        /**< @brief Left node */
/*@null@*/ bencode_dict_t *right;       /**< @brief Right node */
};

static int init = 0;                    /**< @brief Init lock */

#ifdef __cplusplus
extern "C" {
#endif

static void                                     data_delete (bencode_t *node)
                                                             /*@modifies node@*/;
static int                                      string_cmp (const char *a,
                                                            size_t asize,
                                                            const char *b,
                                                            size_t bsize)
                                                            /*@*/;
static /*@null@*/ /*@only@*/ bencode_dict_t*    dict_new (/*@null@*/ bencode_dict_t *parent,
                                                                     const char *key,
                                                                     size_t size,
                                                                     bencode_t *value)
                                                          /*@*/;
static /*@null@*/ /*@only@*/ bencode_t*         decode (const char *data,
                                                        size_t size,
                                                        int flags,
                                                        int *result,
                                                        size_t *pos)
                                                        /*@modifies result,pos@*/;
static /*@null@*/ /*@only@*/ char*              encode (const bencode_t *node,
                                                        char **data,
                                                        size_t *size,
                                                        size_t *allocated,
                                                        int *result)
                                                        /*@modifies data,size,allocated,result@*/;
static int                                      data_put (char **data,
                                                          size_t *size,
                                                          size_t *allocated,
                                                          const char *to,
                                                          size_t tosize)
                                                          /*@modifies data,size,allocated@*/;
static void                                     sort (bencode_dict_t **array,
                                                      int l,
                                                      int r)
                                                      /*@modifies array@*/;
static int                                      partition (bencode_dict_t **array,
                                                           int l,
                                                           int r)
                                                           /*@modifies array@*/;

#ifdef __cplusplus
}
#endif

/**
 * @brief Decode Bencode
 *
 * @param data Data
 * @param size Size of data
 * @param flags Flags
 * @param[out] result Result
 *
 * @return Decoded content
 */
BENCODE_DLL_EXPORTED /*@external@*/ /*@null@*/ /*@only@*/ bencode_t*
bdecode (/*@null@*/ const char *data,
                    size_t size,
                    int flags,
         /*@null@*/ int *result)
/*@modifies result@*/
{
  bencode_t *r = NULL;
  size_t pos = 0;
  delog_enter ("bdecode");

  if (result==NULL)
    delog_return (NULL);

  if (data==NULL || size==0)
    {
      *result = BENCODE_ERROR_NULLARG;
      delog_return (NULL);
    }

  *result = BENCODE_NO_ERROR;

  r = decode (data, size, flags, result, &pos);

  delog_return (r);
}

/**
 * @brief Encode Bencode
 *
 * @param root Root node
 * @param[out] size Size of returned data
 * @param[out] result Result
 *
 * @return Data
 */
BENCODE_DLL_EXPORTED /*@external@*/ /*@null@*/ /*@only@*/ char*
bencode (/*@null@*/ bencode_t *root,
         /*@null@*/ size_t *size,
         /*@null@*/ int *result)
/*@modifies size,result@*/
{
  char *r = NULL;
  char *data = NULL;
  size_t allocated = 0;
  delog_enter ("bencode");

  if (result==NULL)
    delog_return (NULL);

  if (root==NULL || size==NULL)
    {
      *result = BENCODE_ERROR_NULLARG;
      delog_return (NULL);
    }

  r = encode (root, &data, size, &allocated, result);

  if (r!=NULL)
    {
      /*@-evalorder@*/
      r = (char *) _rea (r, (*size+1)*sizeof (char));
      /*@=evalorder@*/
      if (r==NULL)
        {
          *result = BENCODE_ERROR_NO_MEMORY;
          delog_return (NULL);
        }
      r[*size] = EOL;
    }

  delog_return (r);
}

/**
 * @brief New node from integer
 *
 * @param integer Integer
 *
 * @return New node or NULL
 */
BENCODE_DLL_EXPORTED /*@external@*/ /*@null@*/ /*@only@*/ bencode_t*
bencode_new_from_integer (int integer)
/*@*/
{
  bencode_t *node = NULL;
  delog_enter ("bencode_new_from_integer");

  node = (bencode_t *) _new (sizeof (bencode_t));
  if (node==NULL)
    delog_return (NULL);

  node->type = BENCODE_INTEGER;
  node->data.integer = integer;

  delog_return (node);
}

/**
 * @brief New node from string
 *
 * @param str String
 * @param size Size of string
 *
 * @return New node or NULL
 */
BENCODE_DLL_EXPORTED /*@external@*/ /*@null@*/ /*@only@*/ bencode_t*
bencode_new_from_string (/*@null@*/ const char *str,
                                    size_t size)
/*@*/
{
  bencode_t *node = NULL;
  char *s = NULL;
  delog_enter ("bencode_new_from_string");

  if (str==NULL)
    delog_return (NULL);

  node = (bencode_t *) _new (sizeof (bencode_t));
  if (node==NULL)
    delog_return (NULL);

  s = (char *) _new ((size+1)*sizeof (char));
  if (s==NULL)
    {
      _zero (node, sizeof (bencode_t));
      _delete (node);

      delog_return (NULL);
    }
  memcpy (s, str, size*sizeof (char));
  s[size] = EOL;

  node->type = BENCODE_STRING;
  node->data.string.str = s;
  node->data.string.size = size;

  delog_return (node);
}

/**
 * @brief New node from list
 *
 * @param list List
 *
 * @return New node or NULL
 */
BENCODE_DLL_EXPORTED /*@external@*/ /*@null@*/ /*@only@*/ bencode_t*
bencode_new_from_list (/*@null@*/ bencode_list_t *list)
/*@*/
{
  bencode_t *node = NULL;
  delog_enter ("bencode_new_from_list");

  if (list==NULL)
    delog_return (NULL);

  node = (bencode_t *) _new (sizeof (bencode_t));
  if (node==NULL)
    delog_return (NULL);

  node->type = BENCODE_LIST;
  /*@-temptrans@*/
  node->data.list = list;
  /*@=temptrans@*/

  delog_return (node);
}

/**
 * @brief New node from dictionary
 *
 * @param dict Dictionary
 *
 * @return New node or NULL
 */
BENCODE_DLL_EXPORTED /*@external@*/ /*@null@*/ /*@only@*/ bencode_t*
bencode_new_from_dict (/*@null@*/ bencode_dict_t *dict)
/*@*/
{
  bencode_t *node = NULL;
  delog_enter ("bencode_new_from_dict");

  if (dict==NULL)
    delog_return (NULL);

  node = (bencode_t *) _new (sizeof (bencode_t));
  if (node==NULL)
    delog_return (NULL);

  node->type = BENCODE_DICTIONARY;
  /*@-temptrans@*/
  node->data.dict = dict;
  /*@=temptrans@*/

  delog_return (node);
}

/**
 * @brief Delete node
 *
 * @param node Node to delete
 *
 * @return Always NULL
 */
BENCODE_DLL_EXPORTED /*@external@*/ /*@null@*/ bencode_t*
bencode_delete (/*@null@*/ /*@only@*/ bencode_t *node)
/*@modifies node@*/
{
  delog_enter ("bencode_delete");

  if (node==NULL)
    delog_return (NULL);

  data_delete (node);

  _zero (node, sizeof (bencode_t));
  _delete (node);

  delog_return (NULL);
}

/**
 * @brief Delete list
 *
 * @param list List to delete
 *
 * @return Always NULL
 */
BENCODE_DLL_EXPORTED /*@external@*/ /*@null@*/ bencode_list_t*
bencode_list_delete (/*@null@*/ /*@only@*/ bencode_list_t *list)
/*@modifies list@*/
{
  bencode_list_t *iter = NULL;
  bencode_list_t *next = NULL;
  delog_enter ("bencode_list_delete");

  if (list==NULL)
    delog_return (NULL);

  iter = list;

  while (iter!=NULL)
    {
      next = iter->next;

      data_delete (iter->item);

      _zero (iter->item, sizeof (bencode_t));
      _delete (iter->item);

      _zero (iter, sizeof (bencode_list_t));
      _delete (iter);

      iter = next;
    }

  delog_return (NULL);
}

/**
 * @brief Delete dictionary
 *
 * @param dict dictionary
 *
 * @return Always NULL
 */
BENCODE_DLL_EXPORTED /*@external@*/ /*@null@*/ bencode_dict_t*
bencode_dict_delete (/*@null@*/ /*@only@*/ bencode_dict_t *dict)
/*@modifies dict@*/
{
  delog_enter ("bencode_dict_delete");

  if (dict==NULL)
    delog_return (NULL);

  if (dict->left!=NULL)
    (void) bencode_dict_delete (dict->left);

  if (dict->right!=NULL)
    (void) bencode_dict_delete (dict->right);

  delog_assert (dict->key!=NULL);
  _zero (dict->key, dict->size*sizeof (char));
  _delete (dict->key);

  data_delete (dict->value);

  _zero (dict->value, sizeof (bencode_t));
  _delete (dict->value);

  _zero (dict, sizeof (bencode_dict_t));
  _delete (dict);

  delog_return (NULL);
}

/**
 * @brief Delete data
 *
 * @param data Data to delete
 * @param size Size of data
 *
 * @return Always NULL
 */
BENCODE_DLL_EXPORTED /*@external@*/ /*@null@*/ char*
bencode_data_delete (/*@null@*/ /*@only@*/ char *data,
                                           size_t size)
/*@modifies data@*/
{
  delog_enter ("bencode_data_delete");

  if (data==NULL)
    delog_return (NULL);

  _zero (data, size*sizeof (char));
  _delete (data);

  delog_return (NULL);
}

/**
 * @brief Set integer on node
 *
 * @param node Node
 * @param integer Integer
 *
 * @return Always BENCODE_NO_ERROR
 */
BENCODE_DLL_EXPORTED /*@external@*/ int
bencode_set_integer (/*@null@*/ bencode_t *node,
                                int integer)
/*@modifies node@*/
{       
  delog_enter ("bencode_set_integer");

  if (node==NULL)
    delog_return (BENCODE_ERROR_NULLARG);

  data_delete (node);

  node->type = BENCODE_INTEGER;
  node->data.integer = integer;

  delog_return (BENCODE_NO_ERROR);
}

/**
 * @brief Set string on node
 *
 * @param node Node
 * @param string String
 * @param size Size of string
 *
 * @return BENCODE_NO_ERROR or BENOCDE_ERROR_NO_MEMORY
 */
BENCODE_DLL_EXPORTED /*@external@*/ int
bencode_set_string (/*@null@*/ bencode_t *node,
                    /*@null@*/ const char *string,
                               size_t size)
/*@modifies node@*/
{
  char *s = NULL;
  delog_enter ("bencode_set_string");

  if (node==NULL || string==NULL)
    delog_return (BENCODE_ERROR_NULLARG);

  s = (char *) _new ((size+1)*sizeof (char));
  if (s==NULL)
    delog_return (BENCODE_ERROR_NO_MEMORY);
  memcpy (s, string, size*sizeof (char));
  s[size] = EOL;

  data_delete (node);

  node->type = BENCODE_STRING;
  /*@-mustfreeonly@*/
  node->data.string.str = s;
  /*@=mustfreeonly@*/
  node->data.string.size = size;

  delog_return (BENCODE_NO_ERROR);
}

/**
 * @brief Set list on node
 *
 * @param node Node
 * @param list List
 *
 * @return Always BENCODE_NO_ERROR
 */
BENCODE_DLL_EXPORTED /*@external@*/ int
bencode_set_list (/*@null@*/ bencode_t *node,
                  /*@null@*/ bencode_list_t *list)
/*@modifies node@*/
{       
  delog_enter ("bencode_set_list");

  if (node==NULL || list==NULL)
    delog_return (BENCODE_ERROR_NULLARG);

  data_delete (node);

  node->type = BENCODE_LIST;
  /*@-mustfreeonly -temptrans@*/
  node->data.list = list;
  /*@=mustfreeonly =temptrans@*/

  delog_return (BENCODE_NO_ERROR);
}

/**
 * @brief Set dictionary on node
 *
 * @param node Node
 * @param dict Dictionary
 *
 * @return Always BENOCDE_NO_ERROR
 */
BENCODE_DLL_EXPORTED /*@external@*/ int
bencode_set_dict (/*@null@*/ bencode_t *node,
                  /*@null@*/ bencode_dict_t *dict)
/*@modifies node@*/
{       
  delog_enter ("bencode_set_dict");

  if (node==NULL || dict==NULL)
    delog_return (BENCODE_ERROR_NULLARG);

  data_delete (node);

  node->type = BENCODE_DICTIONARY;
  /*@-mustfreeonly -temptrans@*/
  node->data.dict = dict;
  /*@=mustfreeonly =temptrans@*/

  delog_return (BENCODE_NO_ERROR);
}

/**
 * @brief Get type
 *
 * @param node Node
 *
 * @return Type
 */
BENCODE_DLL_EXPORTED /*@external@*/ int
bencode_get_type (/*@null@*/ const bencode_t *node)
/*@*/
{
  delog_enter ("bencode_get_type");

  if (node==NULL)
    delog_return (BENCODE_ERROR_NULLARG);

  delog_return (node->type);
}

/**
 * @brief Get integer
 *
 * @param node Node
 *
 * @return Integer
 */
BENCODE_DLL_EXPORTED /*@external@*/ int
bencode_get_integer (/*@null@*/ const bencode_t *node)
/*@*/
{
  delog_enter ("bencode_get_integer");

  if (node==NULL)
    delog_return (BENCODE_ERROR_NULLARG);

  delog_return (node->data.integer);
}

/**
 * @brief Get string
 *
 * @param node Node
 * @param[out] size Size of string
 *
 * @return String
 */
BENCODE_DLL_EXPORTED /*@external@*/ /*@null@*/ /*@observer@*/ const char*
bencode_get_string (/*@null@*/ const bencode_t *node,
                    /*@null@*/ size_t *size)
/*@*/
{
  delog_enter ("bencode_get_string");

  if (node==NULL || size==NULL)
    delog_return (NULL);

  *size = node->data.string.size;

  delog_return (node->data.string.str);
}

/**
 * @brief Get list
 *
 * @param node Node
 *
 * @return List
 */
BENCODE_DLL_EXPORTED /*@external@*/ /*@null@*/ bencode_list_t*
bencode_get_list (/*@null@*/ const bencode_t *node)
/*@*/
{
  delog_enter ("bencode_get_list");

  if (node==NULL)
    delog_return (NULL);

  /*@-usereleased@*/
  delog_return (node->data.list);
  /*@=usereleased@*/
}

/**
 * @brief Get dictionary
 *
 * @param node Node
 *
 * @return Dictionary
 */
BENCODE_DLL_EXPORTED /*@external@*/ /*@null@*/ bencode_dict_t*
bencode_get_dict (/*@null@*/ const bencode_t *node)
/*@*/
{
  delog_enter ("bencode_get_dict");

  if (node==NULL)
    delog_return (NULL);

  /*@-usereleased@*/
  delog_return (node->data.dict);
  /*@=usereleased@*/
}

/**
 * @brief Append node to list
 *
 * @param list List
 * @param value Node
 *
 * @return New list, old list or NULL
 */
BENCODE_DLL_EXPORTED /*@external@*/ /*@null@*/ bencode_list_t*
bencode_list_append (/*@null@*/ /*@returned@*/ bencode_list_t *list,
                     /*@null@*/                bencode_t *value)
/*@modifies list@*/
{
  bencode_list_t *r = NULL;
  bencode_list_t *n = NULL;
  delog_enter ("bencode_list_append");

  if (value==NULL)
    delog_return (NULL);

  n = (bencode_list_t *) _new (sizeof (bencode_list_t));
  if (n==NULL)
    delog_return (NULL);

  /*@-temptrans@*/
  n->item = value;
  /*@=temptrans@*/
  n->next = NULL;

  if (list==NULL)
    /*@-freshtrans@*/
    delog_return (n);
    /*@=freshtrans@*/

  r = list;
  while (r->next!=NULL)
    r = r->next;

  r->next = n;

  delog_return (list);
}

/**
 * @brief Remove node from list
 *
 * @param list List
 * @param value Node to remove
 *
 * @return New list
 */
BENCODE_DLL_EXPORTED /*@external@*/ /*@null@*/ bencode_list_t*
bencode_list_remove (/*@null@*/ /*@returned@*/ bencode_list_t *list,
                     /*@null@*/                bencode_t *value)
/*@modifies list@*/
{
  bencode_list_t *iter = NULL;
  bencode_list_t *prev = NULL;
  bencode_list_t *next = NULL;
  delog_enter ("bencode_list_remove");

  if (list==NULL || value==NULL)
    delog_return (NULL);

  iter = list;
  while (iter!=NULL)
    {
      if (iter->item == value)
        {
          next = iter->next;

          (void) bencode_delete (iter->item);

          _zero (iter, sizeof (bencode_list_t));
          /*@-temptrans@*/
          _delete (iter);
          /*@=temptrans@*/

          if (prev==NULL)
            {
              /*@-onlytrans@*/
              delog_return (next);
              /*@=onlytrans@*/
            }
          else
            {
              /*@-mustfreeonly@*/
              prev->next = next;
              /*@=mustfreeonly@*/
              /*@loopbreak@*/ break;
            }
        }

      prev = iter;
      iter = iter->next;
    }

  delog_return (list);
}

/**
 * @brief Assign key-value pair to dictionary
 *
 * @param dict Dictionary
 * @param key Key
 * @param size Size of key
 * @param value Value
 *
 * @return New dictionary, old dictionary or NULL
 */
BENCODE_DLL_EXPORTED /*@external@*/ /*@null@*/ bencode_dict_t*
bencode_dict_assign (/*@null@*/ /*@returned@*/ bencode_dict_t *dict,
                     /*@null@*/                const char *key,
                                               size_t size,
                     /*@null@*/                bencode_t *value)
/*@modifies dict@*/
{
  bencode_dict_t *iter = NULL;
  char *s = NULL;
  int n = 0;
  delog_enter ("bencode_dictionary_assign");

  if (key==NULL || value==NULL)
    delog_return (NULL);

  if (dict==NULL)
    {
      iter = dict_new (NULL, key, size, value);
      if (iter==NULL)
        delog_return (NULL);
      /*@-freshtrans@*/
      delog_return (iter);
      /*@=freshtrans@*/
    }
  else
    {
      iter = dict;
      while (iter!=NULL)
        {
          n = string_cmp (iter->key, iter->size, key, size);
          if (n<0)
            {   
              if (iter->left!=NULL)
                iter = iter->left;
              else
                {
                  iter->left = dict_new (iter, key, size, value);
                  if (iter->left==NULL)
                    delog_return (NULL);
                  /*@loopbreak@*/ break;
                }
            }
          else if (n>0)
            {
              if (iter->right!=NULL)
                iter = iter->right;
              else
                {
                  iter->right = dict_new (iter, key, size, value);
                  if (iter->right==NULL)
                    delog_return (NULL);
                  /*@loopbreak@*/ break;
                }
            }
          else
            {
              s = (char *) _new ((size+1)*sizeof (char));
              if (s==NULL)
                delog_return (NULL);
              memcpy (s, key, size*sizeof (char));
              s[size] = EOL;

              delog_assert (iter->key!=NULL);
              _zero (iter->key, iter->size*sizeof (char));
              _delete (iter->key);

              data_delete (iter->value);

              iter->key = s;
              iter->size = size;
              /*@-temptrans@*/
              iter->value = value;
              /*@=temptrans@*/
              /*@loopbreak@*/ break;
            }
        }
    }

  delog_return (dict);
}

/**
 * @brief Remove key-value pair from dictionary
 *
 * @param dict Dictionary
 * @param key Key
 * @param size Size of key
 *
 * @return New dictionary
 */
BENCODE_DLL_EXPORTED /*@external@*/ /*@null@*/ bencode_dict_t*
bencode_dict_remove (/*@null@*/ /*@returned@*/ bencode_dict_t *dict,
                     /*@null@*/                const char *key,
                                               size_t size)
/*@modifies dict@*/
{
  bencode_dict_t *r = NULL;
  bencode_dict_t *iter = NULL;
  int n = 0;
  delog_enter ("bencode_dict_remove");

  if (dict==NULL || key==NULL)
    delog_return (NULL);

  n = string_cmp (dict->key, dict->size, key, size);
  if (n<0)
    {
      if (dict->left!=NULL)
        {
          dict->left = bencode_dict_remove (dict->left, key, size);
          if (dict->left!=NULL)
            /*@-mustfreeonly -temptrans@*/
            dict->left->parent = dict;
            /*@=mustfreeonly =temptrans@*/
        }
    }
  else if (n>0)
    {
      if (dict->right!=NULL)
        {
          dict->right = bencode_dict_remove (dict->right, key, size);
          if (dict->right!=NULL)
            /*@-mustfreeonly -temptrans@*/
            dict->right->parent = dict;
            /*@=mustfreeonly =temptrans@*/
        }
    }
  else
    {
      if (dict->left==NULL && dict->right==NULL)
        {
          delog_assert (dict->key!=NULL);
          _zero (dict->key, dict->size*sizeof (char));
          _delete (dict->key);

          (void) bencode_delete (dict->value);

          _zero (dict, sizeof (bencode_dict_t));
          /*@-temptrans@*/
          _delete (dict);
          /*@=temptrans@*/

          delog_return (NULL);
        }

      if (dict->left==NULL)
        {
          r = dict->right;
          delog_assert (r!=NULL);

          delog_assert (dict->key!=NULL);
          _zero (dict->key, dict->size*sizeof (char));
          _delete (dict->key);

          (void) bencode_delete (dict->value);

          _zero (dict, sizeof (bencode_dict_t));
          /*@-temptrans@*/
          _delete (dict);
          /*@=temptrans@*/

          /*@-mustfreeonly@*/
          r->parent = NULL;
          /*@=mustfreeonly@*/
          delog_return (r);
        }

      if (dict->right==NULL)
        {
          r = dict->left;
          delog_assert (r!=NULL);

          delog_assert (dict->key!=NULL);
          _zero (dict->key, dict->size*sizeof (char));
          _delete (dict->key);

          (void) bencode_delete (dict->value);

          _zero (dict, sizeof (bencode_dict_t));
          /*@-temptrans@*/
          _delete (dict);
          /*@=temptrans@*/

          /*@-mustfreeonly@*/
          r->parent = NULL;
          /*@=mustfreeonly@*/
          delog_return (r);
        }

      iter = dict->left;
      while (iter->right!=NULL)
        iter = iter->right;

      iter->right = dict->right;
      /*@-mustfreeonly@*/
      iter->right->parent = iter;
      /*@=mustfreeonly@*/

      r = dict->left;

      delog_assert (dict->key!=NULL);
      _zero (dict->key, dict->size*sizeof (char));
      _delete (dict->key);

      (void) bencode_delete (dict->value);

      _zero (dict, sizeof (bencode_dict_t));
      /*@-temptrans@*/
      _delete (dict);
      /*@=temptrans@*/

      /*@-mustfreeonly@*/
      r->parent = NULL;
      /*@=mustfreeonly@*/
      delog_return (r);
    }

  delog_return (dict);
}

/**
 * @brief Get list item
 *
 * @param list List
 *
 * @return Node
 */
BENCODE_DLL_EXPORTED /*@external@*/ /*@null@*/ bencode_t*
bencode_list_item (/*@null@*/ const bencode_list_t *list)
/*@*/
{
  delog_enter ("bencode_list_item");

  if (list==NULL)
    delog_return (NULL);

  /*@-usereleased -compdef@*/
  delog_return (list->item);
  /*@=usereleased =compdef@*/
}

/**
 * @brief Get dictionary key
 *
 * @param dict Dictionary
 * @param[out] size Size of key
 *
 * @return Key
 */
BENCODE_DLL_EXPORTED /*@external@*/ /*@null@*/ /*@observer@*/ const char*
bencode_dict_key (/*@null@*/ const bencode_dict_t *dict,
                  /*@null@*/ size_t *size)
/*@modifies size@*/
{
  delog_enter ("bencode_dict_key");

  if (dict==NULL || size==NULL)
    delog_return (NULL);

  *size = dict->size;

  delog_return (dict->key);
}

/**
 * @brief Get dictionary value
 *
 * @param dict Dictionary
 *
 * @return Value
 */
BENCODE_DLL_EXPORTED /*@external@*/ /*@null@*/ bencode_t*
bencode_dict_value (/*@null@*/ const bencode_dict_t *dict)
/*@*/
{
  delog_enter ("bencode_dict_value");

  if (dict==NULL)
    delog_return (NULL);

  /*@-usereleased -compdef@*/
  delog_return (dict->value);
  /*@=usereleased =compdef@*/
}

/**
 * @brief Get next list item
 *
 * @param list List
 *
 * @return Next item or NULL
 */
BENCODE_DLL_EXPORTED /*@external@*/ /*@null@*/ bencode_list_t*
bencode_list_next (/*@null@*/ bencode_list_t *list)
/*@*/
{
  delog_enter ("bencode_list_next");

  if (list==NULL)
    delog_return (NULL);

  /*@-usereleased -compdef@*/
  delog_return (list->next);
  /*@=usereleased =compdef@*/
}

/**
 * @brief Get dictionary node at key
 *
 * @param dict Dictionary
 * @param key Key
 * @param size Size of key
 *
 * @return Dictionary node
 */
BENCODE_DLL_EXPORTED /*@external@*/ /*@null@*/ bencode_dict_t*
bencode_dict_at (/*@null@*/ /*@returned@*/ bencode_dict_t *dict,
                 /*@null@*/                const char *key,
                                           size_t size)
/*@*/
{
  int n = 0;
  bencode_dict_t *iter = NULL;
  delog_enter ("bencode_dict_at");

  if (dict==NULL || key==NULL)
    delog_return (NULL);

  iter = dict;
  while (iter!=NULL)
    {
      n = string_cmp (iter->key, iter->size, key, size);
      if (n<0)
        iter = iter->left;
      else if (n>0)
        iter = iter->right;
      else
        delog_return (iter);
    }

  delog_return (NULL);
}

/**
 * @brief Start iteration on dictionary
 *
 * @param dict Dictionary
 *
 * @return Start entry
 */
BENCODE_DLL_EXPORTED /*@external@*/ /*@null@*/ bencode_dict_t*
bencode_dict_iter_new (/*@null@*/ /*@returned@*/ bencode_dict_t *dict)
/*@*/
{
  bencode_dict_t *iter = NULL;
  delog_enter ("bencode_dict_iter_new");

  if (dict==NULL)
    delog_return (NULL);

  iter = dict;
  while (iter->left!=NULL)
    iter = iter->left;

  delog_return (iter);
}

/**
 * @brief Get next entry in iteration
 *
 * @param dict Dictionary
 *
 * @return Next entry or NULL
 */
BENCODE_DLL_EXPORTED /*@external@*/ /*@null@*/ bencode_dict_t*
bencode_dict_iter_next (/*@null@*/ bencode_dict_t *dict)
/*@*/
{
  bencode_dict_t *iter = NULL;
  delog_enter ("bencode_dict_iter_next");

  if (dict==NULL)
    delog_return (NULL);

  iter = dict;
  if (iter->right)
    {
      iter = iter->right;
      while (iter->left!=NULL)
        iter = iter->left;
    }
  else
    {
      while (iter->parent!=NULL && iter==iter->parent->right)
        iter = iter->parent;
      iter = iter->parent;
    }

  delog_return (iter);
}

/**
 * @brief Represent error code as string
 *
 * @param error Error code
 *
 * @return String of characters
 */
BENCODE_DLL_EXPORTED /*@observer@*/ const char*
bencode_strerror (const int error)
/*@*/
{
  delog_enter ("bencode_strerror");

  if (init==0)
    {
      pvoid bindtextdomain ("bencode", LOCALEDIR);
      init = 1;
    }

  switch (error)
    {
      case BENCODE_NO_ERROR:
        delog_return (_("no error"));
        /*@notreached@*/ /*@switchbreak@*/ break;
      case BENCODE_WAS_ERROR:
        delog_return (_("was error"));
        /*@notreached@*/ /*@switchbreak@*/ break;
      case BENCODE_ERROR_NULLARG:
        delog_return (_("required argument is NULL"));
        /*@notreached@*/ /*@switchbreak@*/ break;
      case BENCODE_ERROR_NO_MEMORY:
        delog_return (_("no memory"));
        /*@notreached@*/ /*@switchbreak@*/ break;
      case BENCODE_ERROR_BAD_DATA:
        delog_return (_("bad data"));
        /*@notreached@*/ /*@switchbreak@*/ break;
      default:
        delog_return (_("unknown error"));
        /*@notreached@*/ /*@switchbreak@*/ break;
    }

  /*@notreached@*/
  delog_return (_("unknown error"));
}

/**
 * @brief Delete node's data
 *
 * @param node Node
 */
static void
data_delete (bencode_t *node)
/*@modifies node@*/
{
  delog_enter ("data_delete");

  delog_assert (node!=NULL);

  switch (node->type)
    {
      case BENCODE_INTEGER:
        /*@switchbreak@*/ break;
      case BENCODE_STRING:
        {
          delog_assert (node->data.string.str!=NULL);
          _zero (node->data.string.str, node->data.string.size*sizeof (char));
          _delete (node->data.string.str);
          /*@switchbreak@*/ break;
        }
      case BENCODE_LIST:
        {
          (void) bencode_list_delete (node->data.list);
          /*@switchbreak@*/ break;
        }
      case BENCODE_DICTIONARY:
        {
          (void) bencode_dict_delete (node->data.dict);
          /*@switchbreak@*/ break;
        }
      default:
        delog_assert (0);
        /*@switchbreak@*/ break;
    }

  /*@-usereleased -compdef@*/
  delog_void_return ();
  /*@=usereleased =compdef@*/
}

/**
 * @brief Compare two strings with sizes
 *
 * @param a A
 * @param asize Size of A
 * @param b B
 * @param bsize Size of B
 *
 * @return -1, 0, 1
 */
static int
string_cmp (const char *a,
            size_t asize,
            const char *b,
            size_t bsize)
/*@*/
{
  delog_enter ("string_cmp");

  delog_assert (a!=NULL);
  delog_assert (b!=NULL);

  if (asize<bsize)
    {
      delog_return (-1);
    }
  else if (asize>bsize)
    {
      delog_return (1);
    }
  else
    {
      delog_return (memcmp (a, b, asize*sizeof (char)));
    }

  /*@notreached@*/
  delog_return (0);
}

/**
 * @brief New dictionary
 *
 * @param parent Parent node
 * @param key Key
 * @param size Size of key
 * @param value Value
 *
 * @return New dictionary or NULL
 */
static /*@null@*/ /*@only@*/ bencode_dict_t*
dict_new (/*@null@*/ bencode_dict_t *parent,
                     const char *key,
                     size_t size,
                     bencode_t *value)
/*@*/
{
  bencode_dict_t *dict = NULL;
  char *s = NULL;
  delog_enter ("dict_new");

  delog_assert (key!=NULL);
  delog_assert (value!=NULL);

  dict = (bencode_dict_t *) _new (sizeof (bencode_dict_t));
  if (dict==NULL)
    delog_return (NULL);

  s = (char *) _new ((size+1)*sizeof (char));
  if (s==NULL)
    {
      _zero (dict, sizeof (bencode_dict_t));
      /*@-compdestroy@*/
      _delete (dict);
      /*@=compdestroy@*/

      delog_return (NULL);
    }
  memcpy (s, key, size*sizeof (char));
  s[size] = EOL;

  dict->key = s;
  dict->size = size;
  /*@-temptrans@*/
  dict->value = value;
  dict->parent = parent;
  /*@=temptrans@*/
  dict->left = NULL;
  dict->right = NULL;

  delog_return (dict);
}

/**
 * @brief Decode Bencode
 *
 * @param data Data
 * @param size Size of data
 * @param flags Flags
 * @param[out] result Result
 * @param pos Position
 *
 * @return Bencode
 */
static /*@null@*/ /*@only@*/ bencode_t*
decode (const char *data,
        size_t size,
        int flags,
        int *result,
        size_t *pos)
/*@modifies result,pos@*/
{
  bencode_t *r = NULL;
  int num = 0;
  int neg = 0;
  bencode_list_t *list = NULL;
  bencode_t *key = NULL;
  bencode_t *value = NULL;
  bencode_dict_t *dict = NULL;
  delog_enter ("decode");

  delog_assert (data!=NULL);
  delog_assert (result!=NULL);
  delog_assert (pos!=NULL);
  delog_assert (*pos<size);

  if (data[*pos]=='i')
    {
      (*pos)++; /*it's an `i'*/
      if (*pos>=size)
        {
          *result = (flags & BENCODE_IGNORE_ERRORS)!=0 ? BENCODE_WAS_ERROR : BENCODE_ERROR_BAD_DATA;
          delog_return (NULL);
        }

      num = 0;
      neg = 0;

      if (data[*pos]=='-')
        {
          neg = 1;
          (*pos)++;
        }
        
      while (*pos<size && isdigit (data[*pos])!=0)
        {
          num *= 10;
          num += (int)data[(*pos)++] - (int)'0';
        }

      if (neg!=0)
        num = -num;

      if (*pos<size)
        {
          if (data[*pos]=='e')
            (*pos)++; /*it's an `e'*/
          else
            {
              if ((flags & BENCODE_IGNORE_ERRORS)!=0)
                {
                  while (*pos<size && data[*pos]!='e' && data[*pos]!='i' && data[*pos]!='l' && data[*pos]!='d' && isdigit (data[*pos])==0)
                    (*pos)++;
                  if (*pos<size && data[*pos]=='e')
                    (*pos)++;
                  *result = BENCODE_WAS_ERROR;
                }
              else
                {
                  *result = BENCODE_ERROR_BAD_DATA;
                  delog_return (NULL);
                }
            }
        }
      else
        {
          if ((flags & BENCODE_IGNORE_ERRORS)!=0)
            {
              r = bencode_new_from_integer (num);
              if (r==NULL)
                {
                  *result = BENCODE_ERROR_NO_MEMORY;
                  delog_return (NULL);
                }

              *result = BENCODE_WAS_ERROR;
              delog_return (r);
            }
          else
            {
              *result = BENCODE_ERROR_BAD_DATA;
              delog_return (NULL);
            }
        }

      r = bencode_new_from_integer (num);
      if (r==NULL)
        {
          *result = BENCODE_ERROR_NO_MEMORY;
          delog_return (NULL);
        }

      delog_return (r);
    }
  else if (data[*pos]=='l')
    {
      (*pos)++; /*it's a `l'*/
      if (*pos>=size)
        {
          *result = (flags & BENCODE_IGNORE_ERRORS)!=0 ? BENCODE_WAS_ERROR : BENCODE_ERROR_BAD_DATA;
          delog_return (NULL);
        }

      list = NULL;
      for (;;)
        {
          r = decode (data, size, flags, result, pos);
          if (r==NULL)
            delog_return (NULL);

          list = bencode_list_append (list, r);
          if (list==NULL)
            {
              (void) bencode_delete (r);
              *result = BENCODE_ERROR_NO_MEMORY;
              delog_return (NULL);
            }

          if (*pos<size)
            {
              if (data[*pos]=='e')
                {
                  (*pos)++;
                  /*@loopbreak@*/ break;
                }
            }
          else
            {
              if ((flags & BENCODE_IGNORE_ERRORS)!=0)
                {
                  *result = BENCODE_WAS_ERROR;
                  /*@loopbreak@*/ break;
                }
              else
                {
                  (void) bencode_list_delete (list);
                  *result = BENCODE_ERROR_BAD_DATA;
                  /*@-mustfreefresh@*/ /*it's `r'*/
                  delog_return (NULL);
                  /*@-mustfreefresh@*/
                }
            }
        }

      if (list!=NULL)
        {
          r = bencode_new_from_list (list);
          if (r==NULL)
            {
              (void) bencode_list_delete (list);
              *result = BENCODE_ERROR_NO_MEMORY;
              delog_return (NULL);
            }
        }

      delog_return (r);
    }
  else if (data[*pos]=='d')
    {
      (*pos)++; /*it's a `d'*/
      if (*pos>=size)
        {
          *result = (flags & BENCODE_IGNORE_ERRORS)!=0 ? BENCODE_WAS_ERROR : BENCODE_ERROR_BAD_DATA;
          delog_return (NULL);
        }

      dict = NULL;

      for (;;)
        {
          key = decode (data, size, flags, result, pos);
          if (key==NULL)
            delog_return (NULL);

          value = decode (data, size, flags, result, pos);
          if (value==NULL)
            {
              (void) bencode_delete (key);
              delog_return (NULL);
            }

          if (key->type==BENCODE_STRING)
            {
              dict = bencode_dict_assign (dict, key->data.string.str, key->data.string.size, value);
              if (dict==NULL)
                {
                  (void) bencode_delete (value);
                  (void) bencode_delete (key);
                  *result = BENCODE_ERROR_NO_MEMORY;
                  delog_return (NULL);
                }
            }
          else
            {
              if ((flags & BENCODE_IGNORE_ERRORS)!=0)
                {
                  (void) bencode_delete (value);
                  *result = BENCODE_WAS_ERROR;
                }
              else
                {
                  (void) bencode_delete (value);
                  (void) bencode_delete (key);
                  (void) bencode_dict_delete (dict);
                  *result = BENCODE_ERROR_BAD_DATA;
                  delog_return (NULL);
                }
            }

          (void) bencode_delete (key);

          if (*pos<size)
            {
              if (data[*pos]=='e')
                {
                  (*pos)++;
                  /*@loopbreak@*/ break;
                }
            }
          else
            {
              if ((flags & BENCODE_IGNORE_ERRORS)!=0)
                {
                  *result = BENCODE_WAS_ERROR;
                  /*@loopbreak@*/ break;
                }
              else
                {
                  (void) bencode_dict_delete (dict);
                  *result = BENCODE_ERROR_BAD_DATA;
                  delog_return (NULL);  
                }
            }
        }

      if (dict!=NULL)
        {
          r = bencode_new_from_dict (dict);
          if (r==NULL)
            {
              (void) bencode_dict_delete (dict);
              *result = BENCODE_ERROR_NO_MEMORY;
              delog_return (NULL);
            }
        }

      delog_return (r);
    }
  else if (isdigit (data[*pos])!=0)
    {
      num = 0;
      while (*pos<size && isdigit (data[*pos])!=0)
        {
          num *= 10;
          num += (int)data[(*pos)++] - (int)'0';
        }
      
      if (*pos<size && data[*pos]!=':')
        {
          if ((flags & BENCODE_IGNORE_ERRORS)!=0)
            *result = BENCODE_WAS_ERROR;
          else
            {
              *result = BENCODE_ERROR_BAD_DATA;
              delog_return (NULL);
            }
        }
      else
        (*pos)++; /*it's a `:'*/

      /*here, pos may be >= size, but it's not fatal */

      delog_print ("num", ("%d", num));

      if (*pos+num<=size)
        {
          r = bencode_new_from_string (data+*pos, (size_t)num);
          *pos += num;
        }
      else
        {
          if ((flags & BENCODE_IGNORE_ERRORS)!=0)
            {
              /*copy everything till the end*/
              r = bencode_new_from_string (data+*pos, size-(*pos-1));
              *pos = size;
              *result = BENCODE_WAS_ERROR;
            }
          else
            {
              *result = BENCODE_ERROR_BAD_DATA;
              delog_return (NULL);
            }
        }

      delog_return (r);
    }
  else
    {
      if ((flags & BENCODE_IGNORE_ERRORS)!=0)
        {
          while (*pos<size && data[*pos]!='i' && data[*pos]!='l' && data[*pos]!='d' && isdigit (data[*pos])==0)
            (*pos)++;

          if (*pos<size)
            r = decode (data, size, flags, result, pos);
          else
            r = NULL;
            
          *result = BENCODE_WAS_ERROR;
          delog_return (r);
        }
      else
        {
          *result = BENCODE_ERROR_BAD_DATA;
          delog_return (NULL);
        }
    }

  /*@notreached@*/
  delog_return (NULL);
}

static const int MAX_INTEGER = 25;                              /**< @brief Max integer numbers */

/**
 * @brief Encode Bencode
 *
 * @param node Node to encode
 * @param data Returned data
 * @param[out] size Size of data
 * @param allocated Size of memory allocated for data
 * @param[out] result Result
 *
 * @return Data
 */
static /*@null@*/ /*@only@*/ char*
encode (const bencode_t *node,
        char **data,
        size_t *size,
        size_t *allocated,
        int *result)
/*@modifies data,size,allocated,result@*/
{
  int rc = 0;
  bencode_list_t *list = NULL;
  bencode_dict_t *dict = NULL;
  bencode_dict_t **array = NULL;
  int array_alloc = 0;
  int array_pos = 0;
  static char digits[] = "0123456789";
  char integer[MAX_INTEGER+1];
  char *iter = NULL;
  int neg = 0;
  int num = 0;
  size_t nums = 0;
  int i = 0;
  delog_enter ("encode");

  delog_assert (node!=NULL);
  delog_assert (data!=NULL);
  delog_assert (size!=NULL);
  delog_assert (allocated!=NULL);
  delog_assert (result!=NULL);

  if (node->type==BENCODE_INTEGER)
    {
      iter = integer+MAX_INTEGER;

      num = node->data.integer;

      neg = 0;
      if (num<0)
        {
          neg = 1;
          num = -num;
        }

      do
        {
          *iter-- = digits[num%10];
          num /= 10;
        }
      while (num!=0);

      if (neg!=0)
        *iter = '-';
      else
        iter++;

      if ((rc = data_put (data, size, allocated, "i", 1))!=BENCODE_NO_ERROR)
        {
          *result = rc;
          delog_return (NULL);
        }

      /*@-compdef@*/
      if ((rc = data_put (data, size, allocated, iter, (size_t)(integer+MAX_INTEGER-iter+1)))!=BENCODE_NO_ERROR)
      /*@=compdef@*/
        {
          *result = rc;
          delog_return (NULL);
        }

      if ((rc = data_put (data, size, allocated, "e", 1))!=BENCODE_NO_ERROR)
        {
          *result = rc;
          delog_return (NULL);
        }
    }
  else if (node->type==BENCODE_STRING)
    {
      iter = integer+MAX_INTEGER;

      nums = node->data.string.size;

      do
        {
          *iter-- = digits[nums%10];
          nums /= 10;
        }
      while (nums!=0);

      iter++;

      /*@-compdef@*/
      if ((rc = data_put (data, size, allocated, iter, (size_t)(integer+MAX_INTEGER-iter+1)))!=BENCODE_NO_ERROR)
      /*@=compdef@*/
        {
          *result = rc;
          delog_return (NULL);
        }

      if ((rc = data_put (data, size, allocated, ":", 1))!=BENCODE_NO_ERROR)
        {
          *result = rc;
          delog_return (NULL);
        }

      if ((rc = data_put (data, size, allocated, node->data.string.str, node->data.string.size))!=BENCODE_NO_ERROR)
        {
          *result = rc;
          delog_return (NULL);
        }
    }
  else if (node->type==BENCODE_LIST)
    {
      if ((rc = data_put (data, size, allocated, "l", 1))!=BENCODE_NO_ERROR)
        {
          *result = rc;
          delog_return (NULL);
        }

      list = node->data.list;
      while (list!=NULL)
        {
          (void) encode (list->item, data, size, allocated, result);
          if (*result!=BENCODE_NO_ERROR)
            {
              _zero (*data, (*size)*sizeof (char));
              /*@-unqualifiedtrans@*/
              _delete (*data);
              /*@=unqualifiedtrans@*/

              /*@-usereleased -compdef@*/
              delog_return (NULL);
              /*@=usereleased =compdef@*/
            }
          list = list->next;
        }

      if ((rc = data_put (data, size, allocated, "e", 1))!=BENCODE_NO_ERROR)
        {
          *result = rc;
          delog_return (NULL);
        }
    }
  else if (node->type==BENCODE_DICTIONARY)
    {
      array_alloc = 16;
      array = (bencode_dict_t **) _new (array_alloc*sizeof (bencode_dict_t *));
      if (array==NULL)
        {
          *result = BENCODE_ERROR_NO_MEMORY;
          delog_return (NULL);
        }

      /*@-compdestroy@*/

      array_pos = 0;
      dict = bencode_dict_iter_new (node->data.dict);
      while (dict!=NULL)
        {
          array[array_pos++] = dict;
          if (array_pos>=array_alloc)
            {
              array_alloc *= 2;
              /*@-evalorder@*/
              array = (bencode_dict_t **) _rea (array, array_alloc*sizeof (bencode_dict_t *));
              /*@=evalorder@*/
              if (array==NULL)
                {
                  *result = BENCODE_ERROR_NO_MEMORY;
                  delog_return (NULL);
                }
            }
          dict = bencode_dict_iter_next (dict);
        }

      delog_print ("array_pos", ("%d", array_pos));

      /*@-compdef@*/
      sort (array, 0, array_pos-1);
      /*@=compdef@*/

      if ((rc = data_put (data, size, allocated, "d", 1))!=BENCODE_NO_ERROR)
        {
          _zero (array, array_pos*sizeof (bencode_dict_t *));
          _delete (array);

          *result = rc;
          delog_return (NULL);
        }

      for (i=0;i<array_pos;i++)
        {
          iter = integer+MAX_INTEGER;

          nums = array[i]->size;

          do
            {
              *iter-- = digits[nums%10];
              nums /= 10;
            }
          while (nums!=0);

          iter++;

          /*@-compdef@*/
          if ((rc = data_put (data, size, allocated, iter, (size_t)(integer+MAX_INTEGER-iter+1)))!=BENCODE_NO_ERROR)
          /*@=compdef@*/
            {
              _zero (array, array_pos*sizeof (bencode_dict_t *));
              _delete (array);

              *result = rc;
              delog_return (NULL);
            }

          if ((rc = data_put (data, size, allocated, ":", 1))!=BENCODE_NO_ERROR)
            {
              _zero (array, array_pos*sizeof (bencode_dict_t *));
              _delete (array);

              *result = rc;
              delog_return (NULL);
            }

          if ((rc = data_put (data, size, allocated, array[i]->key, array[i]->size))!=BENCODE_NO_ERROR)
            {
              _zero (array, array_pos*sizeof (bencode_dict_t *));
              _delete (array);

              *result = rc;
              delog_return (NULL);
            }

          (void) encode (array[i]->value, data, size, allocated, result);
          if (*result!=BENCODE_NO_ERROR)
            {
              _zero (array, array_pos*sizeof (bencode_dict_t *));
              _delete (array);

              _zero (*data, (*size)*sizeof (char));
              /*@-unqualifiedtrans@*/
              _delete (*data);
              /*@=unqualifiedtrans@*/

              /*@-usereleased -compdef@*/
              delog_return (NULL);
              /*@=usereleased -compdef@*/
            }
        }

      _zero (array, array_pos*sizeof (bencode_dict_t *));
      _delete (array);

      /*@=compdestroy@*/

      if ((rc = data_put (data, size, allocated, "e", 1))!=BENCODE_NO_ERROR)
        {
          *result = rc;
          delog_return (NULL);
        }
    }
  else
    delog_assert (0);

  /*@-unqualifiedtrans -usereleased@*/
  delog_return (*data);
  /*@=unqualifiedtrans =usereleased@*/
}

/**
 * @brief Append data to data
 *
 * @param data Data
 * @param size Size of data
 * @param allocated Size of memory allocated for data
 * @param to Data to append
 * @param tosize Size of data to append
 *
 * @return BENCODE_NO_ERROR or BENCODE_ERROR_NO_MEMORY
 */
static int
data_put (char **data,
          size_t *size,
          size_t *allocated,
          const char *to,
          size_t tosize)
/*@modifies data,size,allocated@*/
{
  delog_enter ("data_put");

  delog_assert (data!=NULL);
  delog_assert (size!=NULL);
  delog_assert (allocated!=NULL);
  delog_assert (to!=NULL);

  if (*allocated==0)
    {
      *allocated = 4096;

      *data = (char *) _new ((*allocated)*sizeof (char));
      if (*data==NULL)
        /*@-nullstate@*/
        delog_return (BENCODE_ERROR_NO_MEMORY);
        /*@=nullstate@*/
    }
  else
    {
      if (*size+tosize>=*allocated)
        {
          while (*size+tosize>=*allocated)
            *allocated += 4096;

          /*@-unqualifiedtrans -evalorder@*/
          *data = (char *) _rea (*data, (*allocated)*sizeof (char));
          /*@=unqualifiedtrans =evalorder@*/
          if (*data==NULL)
            /*@-nullstate@*/
            delog_return (BENCODE_ERROR_NO_MEMORY);
            /*@=nullstate@*/
        }
    }

  /*@-mayaliasunique@*/
  memcpy (*data+*size, to, tosize*sizeof (char));
  /*@=mayaliasunique@*/
  *size += tosize;

  /*@-nullstate@*/
  delog_return (BENCODE_NO_ERROR);
  /*@=nullstate@*/
}

/**
 * @brief Sort array of dictionaries
 *
 * @param array Array
 * @param l Left bound
 * @param r Right bound
 */
static void
sort (bencode_dict_t **array,
      int l,
      int r)
/*@modifies array@*/
{
  int j = 0;
  delog_enter ("sort");

  delog_assert (array!=NULL);

  if (l<r)
    {
      j = partition (array, l, r);
      sort (array, l, j-1);
      sort (array, j+1, r);
    }

  delog_void_return ();
}

#define _memcmp(a,asize,b,bsize) (memcmp (a, b, (asize<bsize?asize:bsize)))  /**< @brief Compare two strings with size */

/**
 * @brief Quicksort partitiion
 *
 * @param array Array
 * @param l Left bound
 * @param r Right bound
 *
 * @return Partition bound
 */
static int
partition (bencode_dict_t **array,
           int l,
           int r)
/*@modifies array@*/
{
  int i = 0;
  int j = 0;
  bencode_dict_t *pivot = NULL;
  bencode_dict_t *t = NULL;
  delog_enter ("partition");

  delog_assert (array!=NULL);

  pivot = array[l];
  i = l;
  j = r+1;

  for (;;)
    {
      do i++; while (i<=r && _memcmp (array[i]->key, array[i]->size, pivot->key, pivot->size) <=0);
      do j--; while (_memcmp (array[j]->key, array[j]->size, pivot->key, pivot->size) > 0);
      if (i>=j)
        /*@loopbreak@*/ break;
      t = array[i];
      array[i] = array[j];
      array[j] = t;
   }
  t = array[l];
  array[l] = array[j];
  array[j] = t;

  delog_return (j);
}
