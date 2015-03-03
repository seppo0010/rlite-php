#include "hirlite/hirlite.h"
#include "php_rlite.h"
#include "php_rlite_struct.h"

#include "ext/standard/head.h"
#include "ext/standard/info.h"

#define RLITE_CONNECTION_NAME "rlite connection"

int le_rlite_context;

typedef struct callback {
    zval *function;
} callback;

static
void convert_rlite_to_php(zval* return_value, rliteReply* reply TSRMLS_DC);

static
void s_destroy_connection(rlite_connection *connection TSRMLS_DC)
{
    if (connection) {
        if (connection->c != NULL) {
            rliteFree(connection->c);
        }
        efree(connection);
    }
}

static
void php_rlite_connection_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
    rlite_connection *connection = (rlite_connection*) rsrc->ptr;
    s_destroy_connection(connection TSRMLS_CC);
}

static
rlite_connection *s_create_connection (const char *path)
{
    rliteContext *c;
    rlite_connection *connection;

    c = rliteConnect(path, 0);

    if (!c || c->err) {
        rliteFree(c);
        return NULL;
    }

    connection                = emalloc(sizeof(rlite_connection));
    connection->c             = c;

    return connection;
}

PHP_FUNCTION(rlite_connect)
{
    rlite_connection *connection;
    char *ip;
    int ip_size;
    long port = 6379;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &ip, &ip_size, &port) == FAILURE) {
        return;
    }

    connection = s_create_connection(ip);

    if (!connection) {
        RETURN_FALSE;
    }

    ZEND_REGISTER_RESOURCE(return_value, connection, le_rlite_context);
}

PHP_FUNCTION(rlite_disconnect)
{
    zval *connection;
    rliteContext *c;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &connection) == FAILURE) {
        return;
    }

    ZEND_FETCH_RESOURCE(c, rliteContext *, &connection, -1, RLITE_CONNECTION_NAME, le_rlite_context);
    zend_list_delete(Z_LVAL_P(connection));

    RETURN_TRUE;
}

PHP_FUNCTION(rlite_command)
{
    zval *resource;
    rliteReply *reply = NULL;
    rlite_connection *connection;
    zval *params;
    int argc;
    char ** argv;
    size_t * argvlen;
    zval **tmp;
    HashPosition pos;
    int i;
    void *replyTmp;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ra", &resource, &params) == FAILURE) {
        return;
    }

    ZEND_FETCH_RESOURCE(connection, rlite_connection *, &resource, -1, RLITE_CONNECTION_NAME, le_rlite_context);

    argc = zend_hash_num_elements(Z_ARRVAL_P(params));
    argvlen = emalloc(sizeof(size_t) * argc);
    argv = emalloc(sizeof(char*) * argc);

    i = 0;
    zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(params), &pos);

    while (zend_hash_get_current_data_ex(Z_ARRVAL_P(params), (void **) &tmp, &pos) == SUCCESS) {
        switch ((*tmp)->type) {
                case IS_STRING: {
                        argvlen[i] = (size_t) Z_STRLEN_PP(tmp);
                        argv[i] = emalloc(sizeof(char) * (argvlen[i] + 1));
                        memcpy(argv[i], Z_STRVAL_PP(tmp), argvlen[i]);
                        argv[i][argvlen[i]] = '\0';
                    }
                    break;

                case IS_OBJECT: {
                        int copy;
                        zval expr;
                        zend_make_printable_zval(*tmp, &expr, &copy);
                        argvlen[i] = Z_STRLEN(expr);
                        argv[i] = emalloc(sizeof(char) * (argvlen[i] + 1));
                        memcpy(argv[i], Z_STRVAL(expr), argvlen[i]);
                        if (copy) {
                            zval_dtor(&expr);
                        }
                        argv[i][argvlen[i]] = '\0';
                    }
                    break;

                default:
                    php_error_docref(NULL TSRMLS_CC, E_WARNING, "Array argument must contain strings");
                    break;
        }

        zend_hash_move_forward_ex(Z_ARRVAL_P(params), &pos);
        ++i;
    }

    rliteAppendCommandArgv(connection->c, argc, argv, argvlen);

    for (i = 0; i < argc; i++) {
        efree(argv[i]);
    }
    efree(argv);
    efree(argvlen);

    if (rliteGetReply(connection->c, &replyTmp) != RLITE_OK) {
        // only free if the reply was actually created
        if (replyTmp) rliteFreeReplyObject(replyTmp);

        RETURN_FALSE;
        return;
    }

    reply = replyTmp;

    if (reply->type == RLITE_REPLY_ERROR) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", reply->str);
        rliteFreeReplyObject(reply);

        RETURN_FALSE;
        return;
    }

    convert_rlite_to_php(return_value, reply TSRMLS_CC);
    rliteFreeReplyObject(reply);
}

static
void convert_rlite_to_php(zval* return_value, rliteReply* reply TSRMLS_DC) {
    //int type; /* RLITE_REPLY_* */
    //long long integer; /* The integer when type is RLITE_REPLY_INTEGER */
    //int len; /* Length of string */
    //char *str; /* Used for both RLITE_REPLY_ERROR and RLITE_REPLY_STRING */
    //size_t elements; /* number of elements, for RLITE_REPLY_ARRAY */
    //struct rliteReply **element; /* elements vector for RLITE_REPLY_ARRAY */

    switch (reply->type) {
        case RLITE_REPLY_INTEGER:
            ZVAL_LONG(return_value, reply->integer);
            return;

        case RLITE_REPLY_ERROR:
        case RLITE_REPLY_STATUS:
        case RLITE_REPLY_STRING:
            ZVAL_STRINGL(return_value, reply->str, reply->len, 1);
            return;

        case RLITE_REPLY_ARRAY: {
                zval *val;
                int j;

                array_init(return_value);
                for (j = 0; j < reply->elements; j++) {
                    MAKE_STD_ZVAL(val);
                    convert_rlite_to_php(val, reply->element[j] TSRMLS_CC);
                    add_index_zval(return_value, j, val);
                }
            }
            return;

        case RLITE_REPLY_NIL:
        default:
            ZVAL_NULL(return_value);
            return;
    }
}

PHP_MINIT_FUNCTION(rlite)
{
    le_rlite_context = zend_register_list_destructors_ex(php_rlite_connection_dtor, NULL, RLITE_CONNECTION_NAME, module_number);

    REGISTER_LONG_CONSTANT("RLITE_REPLY_STRING", RLITE_REPLY_STRING, CONST_PERSISTENT|CONST_CS);
    REGISTER_LONG_CONSTANT("RLITE_REPLY_ARRAY", RLITE_REPLY_ARRAY, CONST_PERSISTENT|CONST_CS);
    REGISTER_LONG_CONSTANT("RLITE_REPLY_INTEGER", RLITE_REPLY_INTEGER, CONST_PERSISTENT|CONST_CS);
    REGISTER_LONG_CONSTANT("RLITE_REPLY_NIL", RLITE_REPLY_NIL, CONST_PERSISTENT|CONST_CS);
    REGISTER_LONG_CONSTANT("RLITE_REPLY_STATUS", RLITE_REPLY_STATUS, CONST_PERSISTENT|CONST_CS);
    REGISTER_LONG_CONSTANT("RLITE_REPLY_ERROR", RLITE_REPLY_ERROR, CONST_PERSISTENT|CONST_CS);

    return SUCCESS;
}

static zend_function_entry rlite_functions[] = {
    PHP_FE(rlite_connect, NULL)
    PHP_FE(rlite_disconnect, NULL)
    PHP_FE(rlite_command, NULL)
    {NULL, NULL, NULL}
};

zend_module_entry rlite_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_RLITE_EXTNAME,
    rlite_functions,
    PHP_MINIT(rlite),
    NULL,
    NULL,
    NULL,
    NULL,
#if ZEND_MODULE_API_NO >= 20010901
    PHP_RLITE_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_RLITE
ZEND_GET_MODULE(rlite)
#endif
