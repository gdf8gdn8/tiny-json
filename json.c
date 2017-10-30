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

#include "json.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <stdarg.h>
#define NEW(X) ((X*)calloc(1,sizeof(X)))
#define FREE(X) if (X) { free(X); X=NULL;}
typedef struct scope {
   const char* str;
   const char* ptr;
   int line;
   int pos;
} scope;
static int _json_parse_dict(scope *s,json_dict_t **dict);
static int _json_parse_list(scope *s,json_list_t **list);
static int _json_parse_data(scope *s, json_value_t *value) ;
/***********************************************************************/
static void ADVANCE_(scope *s) {
   if (!*s->ptr) {
      return;
   }
   while(*s->ptr && strchr(" \t\r\n\a",(int)*s->ptr))
   {
      s->ptr++;
      if (strchr("\r\n",(int)*s->ptr)) {
         s->line++;
      }else {
         s->pos++;
      }
   }
}
/***********************************************************************/
static void ADVANCE(scope *s) {
   if (!*s->ptr) {
      return;
   }
   do {
   s->ptr++;
   if (strchr("\r\n",(int)*s->ptr)) {
      s->line++;
   }else {
      s->pos++;
   }
   }while(*s->ptr && strchr(" \t\r\n\a",(int)*s->ptr));

}
/***********************************************************************/
int json_parse_str(char* str,json_parse_t *parserRet,  json_dict_t **out) {
   int ret=JSON_ERROR_OK;
   if (!str || !out) {
      return JSON_ERROR_INVALID_PARAM;
   }
   *out=NULL;
   scope s={(const char*)str,(const char*)str,1,0};
   ADVANCE_(&s);
   if (*s.ptr == '{') {
      ADVANCE(&s);
      *out=json_create_dict();
      if ((ret=_json_parse_dict(&s,out)) != JSON_ERROR_OK) {
         //fprintf(stderr,"Error: wrong json format line %d.%d\n",s.line,s.pos);
         parserRet->line=s.line;
         parserRet->pos=s.pos;
         parserRet->err=(json_error_t)ret;
         return ret;
      }
      parserRet->err=(json_error_t)ret;
      return JSON_ERROR_OK;
   }else {
      parserRet->err=(json_error_t)ret;
      return JSON_ERROR_OK;
   }
}
/***********************************************************************/
static int _json_free_dict(json_dict_t *dict);
/***********************************************************************/
int json_destroy_dict(json_dict_t* dict) {
   return _json_free_dict(dict);
}
/***********************************************************************/
static int _json_free_list(json_list_t *lst);
/***********************************************************************/
int json_destroy_list(json_list_t* lst) {
   return _json_free_list(lst);
}
/***********************************************************************/
static void _json_free_data(json_value_t *value) {
   switch(value->type) {
      case JSON_DATA_TYPE_LIST:
         _json_free_list(value->data.listv);
         break;
      case JSON_DATA_TYPE_DICT:
         _json_free_dict(value->data.dictv);
         break;
      case JSON_DATA_TYPE_STRING:
         FREE(value->data.stringv);
         break;
      case JSON_DATA_TYPE_INT:
      case JSON_DATA_TYPE_DOUBLE:
      case JSON_DATA_TYPE_BOOL:
      break;
      default:
         break;
   }
}
/***********************************************************************/
int json_destroy_value(json_value_t *value) {
   if (!value) {
      return JSON_ERROR_INVALID_PARAM;
   }
   _json_free_data(value);
   return JSON_ERROR_OK;
}
/***********************************************************************/
static int _json_free_dict(json_dict_t *dict) {
   struct list_head *pos, *q;
   json_dict_t *dic;
   if (!dict) {
      return JSON_ERROR_INVALID_PARAM;
   }
   list_for_each_safe(pos,q,&(dict->list)){
      dic=list_entry(pos,json_dict_t,list);
      list_del(pos);
      FREE(dic->name);
      _json_free_data(&dic->value);
      FREE(dic);
   }
   FREE(dict);

   return JSON_ERROR_OK;
}
/***********************************************************************/
int json_clone_value(json_value_t *value,json_value_t *out) {
   if (!value || !out) {
      return JSON_ERROR_INVALID_PARAM;
   }
  switch(value->type) {
      case JSON_DATA_TYPE_LIST:
         out->type=value->type;
         json_clone_list(value->data.listv,&(out->data.listv));
         break;
      case JSON_DATA_TYPE_DICT:
         out->type=value->type;
         json_clone_dict(value->data.dictv,&(out->data.dictv));
         break;
      case JSON_DATA_TYPE_STRING:
         out->type=value->type;
         out->data.stringv=strdup(value->data.stringv);
         break;
      default:
         *out=*value;
         break;
  } 
  return JSON_ERROR_OK;
}
/***********************************************************************/
int json_clone_dict(json_dict_t* dict, json_dict_t **out) {
   struct list_head *pos, *q;
   json_dict_t *dic;
   json_value_t value;
   if (!dict) {
      return JSON_ERROR_INVALID_PARAM;
   }
   if (!out) {
      return JSON_ERROR_INVALID_PARAM;
   }
   *out =json_create_dict();
   list_for_each_safe(pos,q,&(dict->list)){
      dic=list_entry(pos,json_dict_t,list);
      json_clone_value(&(dic->value),&value);
      json_dict_add(*out,1,dic->name,value);
   }
   return JSON_ERROR_OK;
}
/***********************************************************************/
static int _json_free_list(json_list_t *dict) {
   struct list_head *pos, *q;
   json_list_t *dic;
   if (!dict) {
      return JSON_ERROR_INVALID_PARAM;
   }
   list_for_each_safe(pos,q,&(dict->list)){
      dic=list_entry(pos,json_list_t,list);
      list_del(pos);
      _json_free_data(&dic->value);
      FREE(dic);
   }
   FREE(dict);

   return JSON_ERROR_OK;
}
/***********************************************************************/
int json_clone_list(json_list_t* list, json_list_t **out) {
   struct list_head *pos, *q;
   json_list_t *dic;
   json_value_t value;
   if (!list) {
      return JSON_ERROR_INVALID_PARAM;
   }
   if (!out) {
      return JSON_ERROR_INVALID_PARAM;
   }
   *out =json_create_list();
   list_for_each_safe(pos,q,&(list->list)){
      dic=list_entry(pos,json_list_t,list);
      json_clone_value(&(dic->value),&value);
      json_list_add(*out,1,value);
   }
   return JSON_ERROR_OK;
}
/***********************************************************************/
int json_free(json_dict_t *pscr) {
   return _json_free_dict(pscr);
}
/***********************************************************************/
static int _json_parse_data(scope *s, json_value_t *value) {
   char* start;
   char* endptr;
   json_data_type typ=JSON_DATA_TYPE_INT;
   char hexOrOctal=0;

   if (*s->ptr == '{') {
      ADVANCE(s);
      value->data.dictv=json_create_dict();
      value->type=JSON_DATA_TYPE_DICT;
      return _json_parse_dict(s,&(value->data.dictv));
   }else if ( isdigit(*s->ptr) || strchr("+-",(int)*s->ptr)) {

      start=(char*)s->ptr;
      endptr=start;
      while(*endptr && !strchr(" ,}]",*endptr)) {
         ++endptr;
      }
      if (*start == '0')
      {
          hexOrOctal = 1;
      }
      
      for(;start!=endptr;++start) {
         if (*start == '.') {
            typ=JSON_DATA_TYPE_DOUBLE; 
            break;
         }else if (strchr("xX",*start) && hexOrOctal) {
            typ=JSON_DATA_TYPE_INT; 
            if (hexOrOctal == 2)
            {
               return JSON_ERROR_INVALID_VALUE;
            }
            hexOrOctal = 2;
            break;
         } 
      }

      errno=0;
      if (typ == JSON_DATA_TYPE_DOUBLE) {
         value->data.doublev=strtod(s->ptr,&endptr);
      }else {
         value->data.intv=strtoll(s->ptr,&endptr,0);
      }

      if (errno) {
         // invalid value
         return JSON_ERROR_INVALID_VALUE;
      }

      if (!strchr(" ,}]",*endptr))
      {
        return JSON_ERROR_INVALID_PARAM;
      }

      value->type=typ;
      s->ptr=endptr;
   }else if (*s->ptr=='"') {
      ADVANCE(s);
      start=(char*)s->ptr;
      while(*s->ptr && *s->ptr != '"') {
         ADVANCE(s);
      }
      value->data.stringv=(char*)calloc(1,(s->ptr-start)+1);
      strncpy(value->data.stringv,start,(s->ptr-start));
      ADVANCE(s);
      value->type=JSON_DATA_TYPE_STRING;
   }else if (isalpha(*s->ptr)) {
      start=(char*)s->ptr;
      endptr=(char*)s->ptr;
      endptr++;
      while(*endptr && isalpha(*endptr)) {
         ++endptr;
      }
      if (!strncasecmp(start,"true",4)) {
         value->data.boolv=1;   
      }else if (!strncasecmp(start,"false",5)) {
         value->data.boolv=0;   
      }else {
         return JSON_ERROR_INVALID_VALUE;
      }
      s->ptr=endptr;
      ADVANCE(s);
      value->type=JSON_DATA_TYPE_BOOL;
   }else if (*s->ptr == '[') {
      ADVANCE(s);
      value->data.listv=json_create_list();
      value->type=JSON_DATA_TYPE_LIST;
      return _json_parse_list(s,& (value->data.listv));
   }else {
      return JSON_ERROR_INVALID_VALUE;
   }
   return JSON_ERROR_OK;
}
/***********************************************************************/
static int _json_parse_dict(scope *s,json_dict_t **dict) {
   const char* start;
   char name[1024];
   json_value_t value;
   int ret=JSON_ERROR_OK;
   while(*s->ptr && *s->ptr!='}') {
      // parse string
      if(*s->ptr != '"') {
         goto dict_error;
      }
     
      // eat string
      ADVANCE(s);
      start=s->ptr;
      while(*s->ptr && *s->ptr!='"') {
         ADVANCE(s);
      }
      if (!*s->ptr) {
         ret=JSON_ERROR_END_OF_STREAM;
         goto dict_error;
      }
      
      if (s->ptr==start) {
         ret=JSON_ERROR_INVALID_DICTIONARY_KEY;
         goto dict_error;
      }
      memset(name,0,sizeof(name));
      strncpy(name,start, s->ptr-start);
      ADVANCE(s);
      if(*s->ptr != ':') {
         ret=JSON_ERROR_INVALID_DICTIONARY_ENTRY;
         goto dict_error;
      }
      ADVANCE(s);
      if ((ret=_json_parse_data(s,&value))!=JSON_ERROR_OK) {
         goto dict_error;
      }
      json_dict_add(*dict,1,name,value);
      ADVANCE_(s);
      if (*s->ptr == ',') {
         ADVANCE(s);
      }

      if (!strchr("\"}",*s->ptr) ) {
        ret=JSON_ERROR_INVALID_DICTIONARY_ENTRY;
        goto dict_error;
      }
          
   }
   ADVANCE(s);
   return ret;
dict_error:
   _json_free_dict(*dict);
   *dict=0;
   return ret;
}
/***********************************************************************/
int json_dict_add(json_dict_t *dict,int count,...) {
    if (!dict) {
      return JSON_ERROR_INVALID_PARAM;
    }
    va_list ap;
    json_value_t value;
    json_dict_t *dt;
    va_start(ap,count); 
    for(;count>0;count--) {
       dt=NEW(json_dict_t);
       dt->name=strdup(va_arg(ap,char*));
       value=va_arg(ap,json_value_t);
       memcpy(&dt->value,&value,sizeof(json_value_t));
       list_add_tail(&(dt->list),&(dict->list));
       dict->count++;
    }
    va_end(ap);
    return JSON_ERROR_OK;
}
/***********************************************************************/
static int _json_parse_list(scope *s,json_list_t **list) {
   //json_list_t *ls;
   json_value_t value;
   int err=JSON_ERROR_OK;
   while(*s->ptr && *s->ptr!=']') {
      err=_json_parse_data(s,&value);
      if (err) {
         goto array_error;
      }
      json_list_add(*list,1,value);
      ADVANCE_(s);
      if (*s->ptr == ',') {
         ADVANCE(s);
      }

 
   }
   ADVANCE(s);
   return JSON_ERROR_OK;
   array_error:
    _json_free_list(*list);
    *list=0;
    return err;
}
/***********************************************************************/

void json_list_add(json_list_t *lst,int count,...) {
    va_list ap;
    json_value_t value;
    json_list_t *ls;
    va_start(ap,count); 
    for(;count>0;--count) {
       ls=NEW(json_list_t);
       value=va_arg(ap,json_value_t);
       memcpy(&ls->value,&value,sizeof(json_value_t));
       list_add_tail(&(ls->list),&(lst->list));
       lst->count++;
    }
    va_end(ap);
}
/***********************************************************************/
json_dict_t *json_create_dict() {
   json_dict_t *dict;
   dict=NEW(json_dict_t);
   INIT_LIST_HEAD(&(dict->list));
   dict->count=0;   
   return dict;
}
/***********************************************************************/
json_list_t *json_create_list() {
   json_list_t *list;
   list=NEW(json_list_t);
   INIT_LIST_HEAD(&(list->list));
   list->count=0;   
   return list;
}
/***********************************************************************/
