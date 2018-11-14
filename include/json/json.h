#ifndef __JSON_H__
#define __JSON_H__

#define JSON_TYPE_ARRAY '['
#define JSON_TYPE_OBJECT '{'
#define JSON_TYPE_PAIR ':'
#define JSON_TYPE_PAIR_NAME 'N' /* for N:V pairs */
#define JSON_TYPE_STRING '"'
#define JSON_TYPE_INT 'I'
#define JSON_TYPE_NUMBER '0'
#define JSON_TYPE_ERROR 0

/* how should we handle null vs false - both can be 0? */
#define JSON_TYPE_NULL 'n'
#define JSON_TYPE_TRUE 't'
#define JSON_TYPE_FALSE 'f'

#define JSON_TYPE_CALLBACK 'C'

enum {
    JSON_ERROR_OK,
    JSON_ERROR_SYNTAX,
    JSON_ERROR_UNEXPECTED_ARRAY,
    JSON_ERROR_UNEXPECTED_END_OF_ARRAY,
    JSON_ERROR_UNEXPECTED_OBJECT,
    JSON_ERROR_UNEXPECTED_STRING
};

#define JSON_CONTENT_TYPE "application/json"

#endif
