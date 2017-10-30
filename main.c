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

#include "json.h"
static void print_dict(json_dict_t *dict);
static void print_list(json_list_t *dict);
static void print_data(json_value_t *value) {
   switch(value->type) {
      case JSON_DATA_TYPE_LIST:
         print_list(value->data.listv);
         break;
      case JSON_DATA_TYPE_DICT:
         print_dict(value->data.dictv);
         break;
      case JSON_DATA_TYPE_STRING:
         printf("\"%s\"",value->data.stringv);
         break;
      case JSON_DATA_TYPE_INT:
         printf("%lld",value->data.intv);
         break;
      case JSON_DATA_TYPE_DOUBLE:
         printf("%lf",value->data.doublev);
         break;
      case JSON_DATA_TYPE_BOOL:
         printf("%s",value->data.boolv?"true":"false");
         break;
      break;
      default:
         break;
   }

}
static void print_dict(json_dict_t *dict) {
   struct list_head *pos, *q;
   json_dict_t *dic;
   int first=1;
   printf("{");
   list_for_each_safe(pos,q,&(dict->list)){
      if (!first) {
         printf(",");
      }
      dic=list_entry(pos,json_dict_t,list);
      printf("\"%s\" : ",dic->name);
      print_data(&(dic->value));
      first=0;
   }
   printf("}");
}
static void print_list(json_list_t *dict) {
   struct list_head *pos, *q;
   json_list_t *dic;
   int first=1;
   printf("[");
   list_for_each_safe(pos,q,&(dict->list)){
    if (!first) {
         printf(",");
      }
      dic=list_entry(pos,json_list_t,list);
      print_data(&(dic->value));
      first=0;
   }
   printf("]");
}
int main(int argc,char*argv[]) {

  char* str="{ \"test\" : 2, \"liste\" : [ true,1,01,0x1,+1,-1,2.2,\"string\",{ \"subdict\": \"item\"} ], \"test\" : {\"dict\":\"d\" },\"hex\":0xff,\"oct\":01232 }";
   json_parse_t parserRet;
   json_dict_t *js,*cl;
   json_parse_str(str,&parserRet,&js);
   
   if (!js) {
      printf("error: %d %d:%d\n",parserRet.err,parserRet.line,parserRet.pos);
      exit(1);
   }
   print_dict(js);
   printf("\n");
   json_clone_dict(js,&cl);
   print_dict(cl);
   printf("\n");
   json_free(js);
   json_free(cl);
   
   char* str0="{ \"test\" : 2, \"liste\" : [ true,1,01,0x1,+1,-1,2.2,\"string\",{ \"subdict\": \"item\"}],\"hex\":0Xff,  \"test\" : {\"dict\":\"d\" } }";

   json_parse_str(str0,&parserRet,&js);
   if (parserRet.err)
   {
     printf("error: %d %d:%d\n",parserRet.err,parserRet.line,parserRet.pos);
     exit(1);
   }
   print_dict(js);
   printf("\n");
   return 0;
}
