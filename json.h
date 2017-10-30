/*This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>*/

/**
 author: uwe strempel <u.strempel@gmail.com>
 */
#ifndef  __H_JSON__
#define  __H_JSON__

#include <stdlib.h>
#include <stdio.h>
#include "list.h"


typedef struct json_dict_tag json_dict_t;
typedef struct json_list_tag json_list_t;
typedef struct json_value_tag json_value_t;
typedef struct json_parse_tag json_parse_t;
typedef enum json_data_type {
   JSON_DATA_TYPE_NONE,
   JSON_DATA_TYPE_BOOL,
   JSON_DATA_TYPE_INT,
   JSON_DATA_TYPE_DOUBLE,
   JSON_DATA_TYPE_STRING,
   JSON_DATA_TYPE_LIST,
   JSON_DATA_TYPE_DICT,
}json_data_type ;

typedef enum json_error_tag {
   JSON_ERROR_OK,
   JSON_ERROR_INVALID_PARAM,
   JSON_ERROR_INVALID_DICTIONARY_KEY,
   JSON_ERROR_INVALID_VALUE,
   JSON_ERROR_INVALID_STRING,
   JSON_ERROR_INVALID_DICTIONARY_ENTRY,
   JSON_ERROR_INVALID_ARRAY,
   JSON_ERROR_END_OF_STREAM,
   
}json_error_t;
/**
 *  json value type
 */
struct json_value_tag {
   json_data_type type;
   union json_data_tag {
      char boolv:1;
      long long intv;
      double doublev;
      char *stringv;
      json_list_t* listv;
      json_dict_t* dictv;
   } data;
};
/**
 * dictionary and json file
 */
struct json_dict_tag {
   struct list_head list;
   char *name;
   int count;
   json_value_t value;
};
/**
 * json list
 */
struct json_list_tag {
   struct list_head list;
   int count;
   json_value_t value;
};

struct json_parse_tag {
   int line;
   int pos;
   json_error_t err;
};
/**
 * parse json from string
 * @param str string to parse (null terminated)
 * @param parserRet error return
 * @param out out dictionary pointer of pointer - will be allocated internaly
 * @return json error code
 */
int json_parse_str(char* str, json_parse_t *parserRet, json_dict_t **out);
/* *
 * create dictionary
 * @return json dictionary
 */
json_dict_t *json_create_dict();
/**
 * destroy dictionary
 * @return json error code
 */
int json_destroy_dict(json_dict_t* dict);
/**
 * clone dictionary
 * @return json error code
 */
int json_clone_dict(json_dict_t* dict, json_dict_t **out);
/**
 * add value to dict (name,value) pair, json_value
 * @param dict json dictionary 
 * @param count of items (count of pair name-value)
 * @return json error code
 */
int json_dict_add(json_dict_t *dict,int count,...);
/**
 * create a json list
 * @return json list
 */
json_list_t *json_create_list();
/**
 * destroy json list
 * @return json error code
 */
 int json_destroy_list(json_list_t* lst);
 /**
  * clone list
  * @param list input json list
  * @param out reference on output list
  * @return json error code
  */
int json_clone_list(json_list_t* list, json_list_t **out);
/**
 * add value to list json_value
 */
void json_list_add(json_list_t *lst,int count,...);
/**
 * clone value
 * @return json error code
 */
int json_clone_value(json_value_t *value,json_value_t *out);
/**
 * destroy value
 * @param value to destroy
 * @return json error code
 */
int json_destroy_value(json_value_t* value);
int json_free(json_dict_t *pscr);

#endif // define
