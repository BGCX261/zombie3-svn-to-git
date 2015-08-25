/******************************************************************************
** Copyright JRD Communication Inc.
*******************************************************************************/
/******************************************************************************
** Name :  was_api.h
**
** Description : Web Application Server inter process communication API
**
**
** Created 04/01/09
**
*******************************************************************************/

/*******************************************************************************
| PR/CRQ/EVO| Date   | Author     | Comments                                   |
+-----------+--------+------------+--------------------------------------------+
|           |04/01/09|F.Deschamps | Creation, based on  WAS Protocol v0.2      |
+-----------+--------+------------+--------------------------------------------+
|           |04/02/09|F.Deschamps | Update according to WAS Protocol v0.3      |
+-----------+--------+------------+--------------------------------------------+
|           |        |            |                                            |
*******************************************************************************/

#ifndef __WAS_API_H
#define __WAS_API_H

/*============================================================================*
 *      Include Files
 *============================================================================*/
#include <stdlib.h>   /* size_t, ... */
#include <stdarg.h>   /* va_list, ... */
//#include <stdint.h>   /* tUInt8, ... */
#include <assert.h>
#include "mytypes.h"


#ifdef WIN32

#ifdef MESSAGE_EXPORTS
// the dll exports
#ifdef EXPORT_MESSAGE
#undef EXPORT_MESSAGE
#endif
#define EXPORT_MESSAGE __declspec(dllexport)
#else
// the exe imports
#define EXPORT_MESSAGE __declspec(dllimport)
#endif
#else /* WIN32 */
#define EXPORT_MESSAGE
#endif /* WIN32 */

#ifdef EXTERN
#undef EXTERN
#endif

#if defined WAS_API_C
#define EXTERN
#else
#define EXTERN extern
#endif

/*============================================================================*
 *    Define/typedef/struct
 *============================================================================*/

/* ======================== HTTP response code ============================== */
/** \brief HTTP response codes */
#define X_WAS_RSPC_CONTINUE                           100
#define X_WAS_RSPC_SWITCHING_PROTOCOL                 101

#define X_WAS_RSPC_OK                                 200
#define X_WAS_RSPC_CREATED                            201
#define X_WAS_RSPC_ACCEPTED                           202
#define X_WAS_RSPC_NON_AUTHORITATIVE_INFO             203
#define X_WAS_RSPC_NO_CONTENT                         204
#define X_WAS_RSPC_RESET_CONTENT                      205
#define X_WAS_RSPC_PARTIAL_CONTENT                    206

#define X_WAS_RSPC_MULTIPLE_CHOICES                   300
#define X_WAS_RSPC_MOVED_PERMANENTLY                  301
#define X_WAS_RSPC_FOUND                              302
#define X_WAS_RSPC_SEE_OTHER                          303
#define X_WAS_RSPC_NOT_MODIFIED                       304
#define X_WAS_RSPC_USE_PROXY                          305
#define X_WAS_RSPC_TEMPORARY_REDIRECT                 306

#define X_WAS_RSPC_BAD_REQUEST                        400
#define X_WAS_RSPC_UNAUTHORIZED                       401
#define X_WAS_RSPC_PAYMENT_REQUIRED                   402
#define X_WAS_RSPC_FORBIDDEN                          403
#define X_WAS_RSPC_NOT_FOUND                          404
#define X_WAS_RSPC_METHOD_NOT_ALLOWED                 405
#define X_WAS_RSPC_NOT_ACCEPTABLE                     406
#define X_WAS_RSPC_PROXY_AUTHENTICATION_REQUIRED      407
#define X_WAS_RSPC_REQUEST_TIMEOUT                    408
#define X_WAS_RSPC_CONFLICT                           409
#define X_WAS_RSPC_GONE                               410
#define X_WAS_RSPC_LENGTH_REQUIRED                    411
#define X_WAS_RSPC_PRECONDITION_FAILED                412
#define X_WAS_RSPC_REQUEST_ENTITY_TOO_LARGE           413
#define X_WAS_RSPC_REQUEST_URI_TOO_LONG               414
#define X_WAS_RSPC_UNSUPPORTED_MEDIA_TYPE             415
#define X_WAS_RSPC_REQUESTED_RANGE_NOT_SATISFIABLE    416
#define X_WAS_RSPC_EXPECTATION_FAILED                 417

#define X_WAS_RSPC_INTERNAL_SERVER_ERROR              500
#define X_WAS_RSPC_NOT_IMPLEMENTED                    501
#define X_WAS_RSPC_BAD_GATEWAY                        502
#define X_WAS_RSPC_SERVICE_UNAVAILABLE                503
#define X_WAS_RSPC_GATEWAY_TIMEOUT                    504
#define X_WAS_RSPC_HTTP_VERSION_NOT_SUPPORTED         505

#define X_WAS_RSPC_USER_FORCE_FINISHED				  600
#define X_WAS_RSPC_CENTRALS_FORWARDED				  601
#define X_WAS_RSPC_CENTRALS_BUSY					  602
#define X_WAS_RSPC_CENTRALS_AUTHORIZATION_FAILED	  603
#define X_WAS_RSPC_MEMBER_UNAVAILABLE				  604
#define X_WAS_RSPC_REJECTED							  605
#define X_WAS_RSPC_MEMBER_BUSY						  606
#define X_WAS_RSPC_MEMBER_ERROR						  607
//#define X_WAS_RSPC_OK_BUT							  608  
#define X_WAS_RSPC_OK_REDIRECT						  609  
#define X_WAS_RSPC_REJECT_REDIRECT					  609  

/** \brief Minimal size of a WAS message */
#define X_WAS_MSG_MIN_SIZE (sizeof(S_WAS_MSG_HEADER) + sizeof(S_WAS_MSG_FOOTER))

/** \brief Default values for the transaction id field of the header */
#define X_WAS_TID_NONE                  0

/** \brief Default values for the response code field of the header */
#define X_WAS_RSPC_NONE                 0

/** \brief TAG length */
#define X_WAS_TAG_LENGTH                4

/** \brief Max Length Of Body size: 10 MB */
#define X_WAS_MAX_LOB_SIZE              (10 * 1024 * 1024)

/** \brief Message types */
typedef enum {
  E_WAS_MSG_INVALID,
  E_WAS_MSG_ACKNOWLEDGE,
  E_WAS_MSG_LOGIN_REQ,
  E_WAS_MSG_LOGIN_CNF,
  E_WAS_MSG_LOGOFF_REQ,
  E_WAS_MSG_LOGOFF_CNF,
  E_WAS_MSG_ENDPOINTS_REGISTER_REQ,
  E_WAS_MSG_ENDPOINTS_REGISTER_CNF,
  E_WAS_MSG_INSTANTMSG,
  E_WAS_MSG_VOICE_CHAT_REQ,
  E_WAS_MSG_VOICE_CHAT_CNF,
  E_WAS_MSG_VOICE_GROUP_CHAT_REQ,
  E_WAS_MSG_VOICE_GROUP_CHAT_CNF,
  E_WAS_MSG_VIDEO_CHAT_REQ,
  E_WAS_MSG_VIDEO_CHAT_CNF,
  E_WAS_MSG_VIDEO_GROUP_CHAT_REQ,
  E_WAS_MSG_VIDEO_GROUP_CHAT_CNF,
  E_WAS_MSG_TRANSFER_REQ,
  E_WAS_MSG_TRANSFER_CNF,
  E_WAS_MSG_NOTIFY_ADD,
  E_WAS_MSG_NOTIFY_DEL,
  E_WAS_MSG_NOTIFY_QUIT,	// initiatively
  E_WAS_MSG_NOTIFY_LEAVE,	// passively by admin
  E_WAS_MSG_NOTIFY_CANCEL,
  E_WAS_MSG_BLOCK_DATA,
  E_WAS_MSG_COMMON_CNF,
  E_WAS_MSG_SHUTDOWN_IND,
  E_WAS_MSG_TYPE_NB
} E_WAS_MSG_TYPE;

/** \brief Message parts */
typedef enum {
  E_WAS_MSG_FIRST_PART,
  E_WAS_MSG_LAST_PART,
  E_WAS_MSG_INTERMEDIATE_PART,
  E_WAS_MSG_PART_ID_NB
} E_WAS_MSG_PART_ID;

/** \brief Tag types */
typedef enum {
  E_WAS_TAG_TYPE_NONE,
  E_WAS_TAG_TYPE_MARKER,
  E_WAS_TAG_TYPE_BYTE_STREAM,
  E_WAS_TAG_TYPE_KEY_VAL
} E_WAS_TAG_TYPE;

/** \brief Field ids of the message header */
typedef enum {
  E_WAS_MSG_HEADER_FIELD_MPI,
  E_WAS_MSG_HEADER_FIELD_TOM,
  E_WAS_MSG_HEADER_FIELD_TID,
  E_WAS_MSG_HEADER_FIELD_RSPC,
  E_WAS_MSG_HEADER_FIELD_LOB,
  E_WAS_MSG_HEADER_FIELD_ID_NB
} E_WAS_MSG_HEADER_FIELD_ID;

#define X_WAS_MAKE_STRING_1(line)   #line
#define X_WAS_MAKE_STRING(line)     X_WAS_MAKE_STRING_1(line)

#define X_WAS_TAG_MACRO(a, b) \
  HEAD(a) \
  ITEM(b, NONE, NONE) \
  ITEM(b, SOBM, MARKER) \
  ITEM(b, EOBM, MARKER) \
  ITEM(b, FLNM, BYTE_STREAM) \
  ITEM(b, FPID, BYTE_STREAM) \
  ITEM(b, SSID, BYTE_STREAM) \
  ITEM(b, PHOT, BYTE_STREAM) \
  ITEM(b, PPOT, BYTE_STREAM) \
  ITEM(b, IHOT, BYTE_STREAM) \
  ITEM(b, IPOT, BYTE_STREAM) \
  ITEM(b, PP2T, BYTE_STREAM) \
  ITEM(b, IP2T, BYTE_STREAM) \
  ITEM(b, MBIP, BYTE_STREAM) \
  ITEM(b, MBNM, BYTE_STREAM) \
  ITEM(b, MBID, BYTE_STREAM) \
  ITEM(b, MYID, BYTE_STREAM) \
  ITEM(b, MYNM, BYTE_STREAM) \
  ITEM(b, NBID, BYTE_STREAM) \
  ITEM(b, NBNM, BYTE_STREAM) \
  ITEM(b, AMID, BYTE_STREAM) \
  ITEM(b, FLAG, BYTE_STREAM) \
  ITEM(b, UKEY, BYTE_STREAM) \
  ITEM(b, EMSG, BYTE_STREAM) \
  ITEM(b, COTY, BYTE_STREAM) \
  ITEM(b, CODI, BYTE_STREAM) \
  ITEM(b, ATTA, BYTE_STREAM) \
  ITEM(b, ETAG, BYTE_STREAM) \
  ITEM_LAST(b, NB, NONE) \
  FOOT(b)

/** \brief Tag id enumerates */
#define HEAD(a) typedef a { 
#define ITEM(a, b, c) a##_##b,
#define ITEM_LAST(a, b, c) a##_##b
#define FOOT(a) } a

X_WAS_TAG_MACRO(enum, E_WAS_TAG_ID);

#undef HEAD
#undef ITEM
#undef ITEM_LAST
#undef FOOT

/** \brief Message validity checker return codes */
typedef enum {
  E_WAS_MSG_INVALID_HEADER_TOO_SHORT,
  E_WAS_MSG_INVALID_HEADER_TAG,
  E_WAS_MSG_INVALID_HEADER_MPI,
  E_WAS_MSG_INVALID_HEADER_TOM,
  E_WAS_MSG_INVALID_HEADER_LOB,
  E_WAS_MSG_INVALID_TOO_SHORT,
  E_WAS_MSG_INVALID_FOOTER_TAG,
  E_WAS_MSG_VALID
} E_WAS_MSG_VALIDITY;

/** \brief Message re-synchronization status */
typedef enum {
  E_WAS_MSG_NOT_SYNC,
  E_WAS_MSG_NOT_SYNC_TOO_SHORT,
  E_WAS_MSG_SYNC
} E_WAS_MSG_RESYNC;

/** \brief Tag byte stream data structure */
typedef struct {
  tUInt32 byte_stream_len;     /**< Length of the byte stream */
  const tUInt8 * byte_stream;  /**< Start address of byte stream */
} S_WAS_TAG_BYTE_STREAM;

/** \brief Tag key value data structure */
typedef struct {
  tUInt32 key_len;       /**< Length of the key */
  const tUInt8 * key;    /**< Start address of the key */
  tUInt32 val_len;       /**< Length of the value */
  const tUInt8 * val;    /**< Start address of the value */
} S_WAS_TAG_KEY_VAL;

/** \brief Byte stream / key value union */
typedef union {
  S_WAS_TAG_BYTE_STREAM bs; /**< Store byte stream data */
  S_WAS_TAG_KEY_VAL kv;     /**< Store key/value data */
} U_WAS_TAG_DATA;

/** \brief Tag data structure */
typedef struct {
  E_WAS_TAG_TYPE tag_type;  /**< Type of tag */
  E_WAS_TAG_ID tag_id;      /**< Tag id */
  tUInt32 offset;          /**< Offset of last found tag, 0 means get first tag */
  U_WAS_TAG_DATA data;      /**< Type of tag dependent: S_WAS_TAG_BYTE_STREAM or S_WAS_TAG_KEY_VAL */
} S_WAS_TAG_DATA;

/** \brief Message header structure */
typedef struct {
  tUInt8 som[4];   /**< Start Of Message tag */
  tUInt32 mpi;     /**< Message Part Id */
  tUInt32 tom;     /**< Type Of Message, see E_WAS_MSG_TYPE */
  tUInt32 tid;     /**< Transaction ID */
  tUInt32 rspc;    /**< ReSPonse Code, see X_WAS_RSPC_... */
  tUInt32 lob;     /**< Length Of Body */
} S_WAS_MSG_HEADER;

/** \brief Message footer structure */
typedef struct {
  tUInt8 eom[4];   /**< EOM End Of Message tag */
} S_WAS_MSG_FOOTER;

/*============================================================================*
 *    Const variables
 *============================================================================*/

/*============================================================================*
 *    Global variables
 *============================================================================*/

/*============================================================================*
 *    Prototypes
 *============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif
/* Functions used to set or get a field of the header of a WAS message */

/** \brief Set the specified message header field with the given value
 *
 * [In/Out] p_msg   : non NULL pointer on a valid WAS message (SOBM tag has to be set already)
 *
 * [In]     id      : message header field id
 *
 * [In]     value   : value to set for the specified field
 *
 * Returns: Nothing
 *
 * was_set_header_field() checks for the presence of the SOBM tag and also for the validity of the given value for the MPI and TOM fields of the message
 */
EXPORT_MESSAGE void was_set_header_field(tUInt8 * p_msg,
                                 E_WAS_MSG_HEADER_FIELD_ID id,
                                 tUInt32 value);

/** \brief Get the value of specified message header field
 *
 * [In/Out] p_msg   : non NULL pointer on a valid WAS message
 *
 * [In]     id      : message header field id
 *
 * Returns: the value of the specified field as an tUInt32
 */
EXPORT_MESSAGE tUInt32 was_get_header_field(tUInt8 * p_msg,
                                     E_WAS_MSG_HEADER_FIELD_ID id);

/* Functions allocating and filling messages by type */

/** \brief Allocates and fills a WAS message
 *
 * [Out]    p_len   : non NULL pointer that will contain the length of the message
 *
 * [In]     mpi     : message part id
 *
 * [In]     tom     : type of message
 *
 * [In]     tid     : transaction id
 *
 * [In]     rspc    : response code
 *
 * [In]     ...     : a 3-tuple or 5-tuple formatted like this:
 *
 * (E_WAS_TAG_ID tag_id, tUInt32 byte_stream_len, tUInt8 * byte_stream) for an E_WAS_TAG_TYPE_BYTE_STREAM tag type
 *
 * (E_WAS_TAG_ID tag_id, tUInt32 key_len, tUInt8 * key, tUInt32 val_len, tUInt8 * val) for an E_WAS_TAG_TYPE_KEY_VAL tag type
 *
 * The list of n-tuple(s) shall be terminated with E_WAS_TAG_ID_NONE
 *
 * Example: p_msg = was_write_msg(&len, E_WAS_MSG_FIRST_PART, E_WAS_MSG_QUERY_REQ, 1, X_WAS_RSPC_NONE, E_WAS_TAG_ID_SRVI, 1, "1", E_WAS_TAG_ID_QRYC, 12, "get_forecast", E_WAS_TAG_ID_KVAL, 9, "city_code", 22, "ASI|CN|CH024|SHANGHAI|", E_WAS_TAG_ID_KVAL, 6, "metric", 1, "1", E_WAS_TAG_ID_NONE);
 *
 * Returns: a newly allocated pointer on the message or NULL if an error occured. The pointer has to be freed with was_free_msg()
 */
EXPORT_MESSAGE tUInt8 * was_write_msg(tUInt32 * p_len, tUInt32 mpi, tUInt32 tom,
                               tUInt32 tid, tUInt32 rspc, ...);

/** \brief Behaves exactly like was_write_msg(), but takes a va_list, instead of varargs. This function is mainly meant for language bindings.
 *
 * [Out]    p_len   : non NULL pointer that will contain the length of the message
 *
 * [In]     mpi     : message part id
 *
 * [In]     tom     : type of message
 *
 * [In]     tid     : transaction id
 *
 * [In]     rspc    : response code
 *
 * [In]     ap      : va_list formatted as detailed for was_write_msg()
 *
 * Returns: a newly allocated pointer on the message or NULL if an error occured. The pointer has to be freed with was_free_msg()
 */
EXPORT_MESSAGE tUInt8 * was_write_msgv(tUInt32 * p_len, tUInt32 mpi, tUInt32 tom,
                                tUInt32 tid, tUInt32 rspc, va_list ap);

/** \brief Appends tag(s) to a valid WAS message
 *
 * [Out]    p_len   : non NULL pointer that will contain the length of the message
 *
 * [In]     p_in    : non NULL pointer on a valid message
 *
 * [In]     ...     : a 3-tuple or 5-tuple formatted like this:
 *
 * (E_WAS_TAG_ID tag_id, tUInt32 len, tUInt8 *) for an E_WAS_TAG_TYPE_BYTE_STREAM tag type
 *
 * (E_WAS_TAG_ID tag_id, tUInt32 key_len, tUInt8 * key, tUInt32 val_len, tUInt8 * val) for an E_WAS_TAG_TYPE_KEY_VAL tag type
 *
 * The list of n-tuple(s) shall be terminated with E_WAS_TAG_ID_NONE
 *
 * Example: p_msg = was_append_to_msg(&len, p_msg, E_WAS_TAG_ID_QRYC, 12, "get_forecast", E_WAS_TAG_ID_NONE);
 *
 * Returns: a newly allocated pointer on the message or NULL if an error occured. The pointer has to be freed with was_free_msg()
 */
EXPORT_MESSAGE tUInt8 * was_append_to_msg(tUInt32 * p_len,
                                   tUInt8 * p_in, ...);

/** \brief Behaves exactly like was_append_to_msg(), but takes a va_list, instead of varargs. This function is mainly meant for language bindings.
 *
 * [Out]    p_len   : non NULL pointer that will contain the length of the message
 *
 * [In]     p_in    : non NULL pointer on a valid message
 *
 * [In]     ap      : va_list formatted as detailed for was_append_to_msg()
 *
 * Returns: a newly allocated pointer on the message or NULL if an error occured. The pointer has to be freed with was_free_msg()
 */
EXPORT_MESSAGE tUInt8 * was_append_to_msgv(tUInt32 * p_len,
                                    tUInt8 * p_in,
                                    va_list ap);

/** \brief Get data from the first tag of a WAS message body
 *
 * [In]     p_msg : pointer on a WAS message
 *
 * [In]     len   : length of the message
 *
 * [In/Out] p_td  : non NULL pointer on a structure that will be filled with tag data
 *
 * Returns: the tag id of the tag found or E_WAS_TAG_ID_NONE if no valid tag has been found
 */
EXPORT_MESSAGE E_WAS_TAG_ID was_get_first_tag_data(const tUInt8 * p_msg, tUInt32 len,
                                           S_WAS_TAG_DATA * p_td);

/** \brief Get data from the next tag of a WAS message body
 *
 * [In]     p_msg : pointer on a WAS message
 *
 * [In]     len   : length of the message
 *
 * [In/Out] p_td  : non NULL pointer on a structure that will be filled with tag data
 *
 * if p_td->offset is 0, it behaves like was_get_first_tag_data
 *
 * Returns: the tag id of the tag found or E_WAS_TAG_ID_NONE if no valid tag has been found
 */
EXPORT_MESSAGE E_WAS_TAG_ID was_get_next_tag_data(const tUInt8 * p_msg, tUInt32 len,
                                          S_WAS_TAG_DATA * p_td);

/** \brief Frees a pointer allocated by WAS module
 *
 * [In/Out] pp_ptr : a non NULL pointer on a pointer allocated by WAS
 *
 * Returns: Nothing
 *
 * Frees the memory pointed by pp_ptr and sets pp_ptr as NULL.
 */
EXPORT_MESSAGE void was_free_msg(void ** pp_ptr);

/** \brief Check validity of a WAS message
 *
 * [In]     p_msg : a non NULL pointer on a WAS message
 *
 * [In/Out] p_len : a pointer on the WAS message length
 *
 * Returns:
 *
 *  - E_WAS_MSG_INVALID_HEADER_TOO_SHORT if the lenght is smaller than the minimal size of a WAS message (= size of the header + size of the footer)
 *
 *  - E_WAS_MSG_INVALID_HEADER_TAG if the SOBM tag is not found at the begining of the message
 *
 *  - E_WAS_MSG_INVALID_HEADER_MPI if the MPI field of the header is equal or greater than E_WAS_MSG_PART_ID_NB 
 *
 *  - E_WAS_MSG_INVALID_HEADER_TOM if the TOM field of the header is equal to E_WAS_MSG_INVALID or equal or greater than E_WAS_MSG_TYPE_NB[/quote] 
 *
 *  - E_WAS_MSG_INVALID_HEADER_LOB if the LOB field of the header is greater than X_WAS_MAX_LOB_SIZE[/quote] 
 *
 *  - E_WAS_MSG_INVALID_TOO_SHORT if the lenght is smaller than the size of the WAS message (= size of the header + lob + size of the footer)
 *
 *  - E_WAS_MSG_INVALID_FOOTER_TAG if EOBM footer tag is not found at the end of the message
 *
 *  - E_WAS_MSG_VALID if all previous tests succeed
 *
 * Remarks:
 *
 *  - In case of invalid message (all failure causes but *_TOO_SHORT), the caller shall call was_resync() to find the next valid message in the buffer
 *
 *  - As of today, the body is not checked by was_is_msg_valid()
 *
 *  - was_is_msg_valid() will set the length of the WAS message if valid, thus the caller shall not pass a pointer on the length of the buffer as the length of the message but a pointer on a copy of the length
 */
EXPORT_MESSAGE E_WAS_MSG_VALIDITY was_is_msg_valid(const tUInt8 * p_msg,
                                           tUInt32 * p_len);

/** \brief Check validity of a WAS message
 *
 * [In]  p_buf : a non NULL pointer on a buffer containing WAS messages
 *
 * [In]  buf_len : buffer length
 *
 * [Out] p_msg_offset : a non NULL pointer that will be filled with the offset of the first valid WAS message
 *
 * [Out] p_msg_len : a non NULL pointer that will be filled with the WAS message length
 *
 * Returns:
 *
 *  - E_WAS_MSG_SYNC if a valid WAS message has been found in the buffer
 *
 *  - E_WAS_MSG_NOT_SYNC_TOO_SHORT if the start of a valid WAS message has been found in the buffer
 *
 *  - E_WAS_MSG_NOT_SYNC if no valid WAS message has been found in the buffer
 *
 * Remark:
 *
 *  - p_msg_offset is set when E_WAS_MSG_SYNC or E_WAS_MSG_NOT_SYNC_TOO_SHORT are returned
 *
 *  - p_msg_len is setw hen E_WAS_MSG_SYNC is returned
 */
EXPORT_MESSAGE E_WAS_MSG_RESYNC was_resync(const tUInt8 * p_buf,
                                   tUInt32 buf_len,
                                   tUInt32 * p_msg_offset,
                                   tUInt32 * p_msg_len);
#ifdef __cplusplus 
}
#endif

#undef EXTERN

#endif
