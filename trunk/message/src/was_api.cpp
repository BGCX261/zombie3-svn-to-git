/******************************************************************************
** Copyright JRD Communication Inc.
*******************************************************************************/
/******************************************************************************
** Name :  was_api.c
**
** Description : Utilities to build/analyze messages encoded with WAS API
**
**
** Created 14/01/09
**
*******************************************************************************/

/*******************************************************************************
| PR/CRQ/EVO| Date   | Author     | Comments                                   |
+-----------+--------+------------+--------------------------------------------+
|           |04/01/09|F.Deschamps | Creation, based on  WAS Protocol v0.2      |
+-----------+--------+------------+--------------------------------------------+
|           |04/02/09|F.Deschamps | Update according to WAS Protocol v0.3      |
|           |        |            |                                            |
*******************************************************************************/

#define WAS_API_C

/* ==========================================================================
 *  Header files
 * ========================================================================== */
#include <string.h>   /* memcpy, ... */
#include <stdarg.h>   /* va_start, ... */
#include <assert.h>   /* assert, ... */
//#include <syslog.h>   /* syslog, ... */

#include "was_api.h"

/* ==========================================================================
 *  Typedef
 * ========================================================================== */
/** \brief tUInt32 to tUInt8[4] conversion */
#define X_WAS_U32_TO_U8A(a, b) \
  if (1) { \
    *(b) = (((a) >> 24) & 0xFF); \
    *((b) + 1)= (((a) >> 16) & 0xFF); \
    *((b) + 2)= (((a) >> 8) & 0xFF); \
    *((b) + 3)= ((a) & 0xFF); \
  } \
  else (void) 0
  
/** \brief tUInt8[4] to tUInt32 conversion */
#define X_WAS_U8A_TO_U32(a) \
  ((*a) << 24) + ((*((a) + 1)) << 16) + ((*((a) + 2)) << 8)+ ((*((a) + 3)))
  
/* Min length of a tag (E_WAS_TAG_TYPE_BYTE_STREAM) */
#define X_WAS_TAG_DATA_MIN_LEN ((2 * X_WAS_TAG_LENGTH) + 1)

/* Tag string table */
#define HEAD(a) static const char * a[E_WAS_TAG_ID_NB] = {
#define ITEM(a, b, c) X_WAS_MAKE_STRING(b),
#define ITEM_LAST(a, b, c) 
#define FOOT(a) };

X_WAS_TAG_MACRO(gc_was_tag_name, unused);

#undef HEAD
#undef ITEM
#undef ITEM_LAST
#undef FOOT

typedef struct {
  E_WAS_TAG_ID id;
  E_WAS_TAG_TYPE type;
  const char * name;
} S_WAS_TAG_LOOKUP_TABLE;

/* Tag info lookup table */
#define HEAD(a) static const S_WAS_TAG_LOOKUP_TABLE a[E_WAS_TAG_ID_NB] = { 
#define ITEM(a, b, c) { E_WAS_TAG_ID_##b, E_WAS_TAG_TYPE_##c, a[E_WAS_TAG_ID_##b] },
#define ITEM_LAST(a, b, c) 
#define FOOT(a) };

X_WAS_TAG_MACRO(gc_was_tag_lookup_table, gc_was_tag_name);

#undef HEAD
#undef ITEM
#undef ITEM_LAST
#undef FOOT

/* Message construction, analysis of va_list: states */
typedef enum {
  E_WAS_VA_LIST_END,
  E_WAS_VA_LIST_WAIT_TAG_ID,
  E_WAS_VA_LIST_WAIT_BYTE_STREAM_LEN,
  E_WAS_VA_LIST_WAIT_BYTE_STREAM,
  E_WAS_VA_LIST_WAIT_KEY_LEN,
  E_WAS_VA_LIST_WAIT_KEY,
  E_WAS_VA_LIST_WAIT_VAL_LEN,
  E_WAS_VA_LIST_WAIT_VAL
} E_WAS_VA_LIST_STATE;

/* ==========================================================================
 *  Const data
 * ========================================================================== */

/* ==========================================================================
 *  Global variables
 * ========================================================================== */

/* ==========================================================================
 *  Static prototypes
 * ========================================================================== */
static void was_write_header(S_WAS_MSG_HEADER * p_hdr, tUInt32 mpi,
                             tUInt32 tom, tUInt32 tid, tUInt32 rspc,
                             tUInt32 lob);
static void was_write_footer(tUInt8 * p_footer);
static void was_write_byte_stream_tag_to_msg(E_WAS_TAG_ID tag_id,
                                             tUInt8 ** pp_msg,
                                             const tUInt8 * p_str,
                                             tUInt32 len);
static void was_write_key_val_tag_to_msg(E_WAS_TAG_ID tag_id, tUInt8 ** pp_msg,
                                         const tUInt8 * p_key, tUInt32 key_len,
                                         const tUInt8 * p_val, tUInt32 val_len);
static E_WAS_TAG_ID was_get_tag_id(const tUInt8 * p_tag,
                                   E_WAS_TAG_TYPE * p_type);
static tUInt32 was_compute_body_length(va_list ap);
static void was_write_msg_body(tUInt8 ** pp_body, va_list ap);
static void * was_malloc0(size_t size);
static void * was_realloc(void * p_in, size_t size);

/* ========================================================================== */
static void was_write_header(S_WAS_MSG_HEADER * p_hdr, tUInt32 mpi,
                             tUInt32 tom, tUInt32 tid, tUInt32 rspc,
                             tUInt32 lob) {

  /* Check parameters */
  assert(p_hdr /*&& E_WAS_MSG_PART_ID_NB > mpi*/ && E_WAS_MSG_TYPE_NB > tom);

  /* Copy header tag */
  memcpy(p_hdr->som, gc_was_tag_name[E_WAS_TAG_ID_SOBM], X_WAS_TAG_LENGTH);

  /* Fill fields */
  p_hdr->mpi = mpi;
  p_hdr->tom = tom;
  p_hdr->tid = tid;
  p_hdr->rspc = rspc;
  p_hdr->lob = lob;
}

/* ========================================================================== */
static void was_write_footer(tUInt8 * p_footer) {

  /* Check parameters */
  assert(p_footer);

  /* Copy footer tag */
  memcpy(p_footer, gc_was_tag_name[E_WAS_TAG_ID_EOBM], X_WAS_TAG_LENGTH);
}

/* ========================================================================== */
static void was_write_byte_stream_tag_to_msg(E_WAS_TAG_ID tag_id,
                                             tUInt8 ** pp_msg,
                                             const tUInt8 * p_str,
                                             tUInt32 len) {

  /* Check parameters */
  assert(E_WAS_TAG_ID_NONE != tag_id && E_WAS_TAG_ID_NB > tag_id &&
         pp_msg && p_str && len);

  /* Copy tag */
  memcpy(*pp_msg, gc_was_tag_name[tag_id], X_WAS_TAG_LENGTH);
  *pp_msg += X_WAS_TAG_LENGTH;

  /* Copy byte stream length */
  X_WAS_U32_TO_U8A(len, *pp_msg);
  *pp_msg += X_WAS_TAG_LENGTH;

  /* Copy byte stream */
  memcpy(*pp_msg, p_str, len);
  *pp_msg += len;
}

/* ========================================================================== */
static void was_write_key_val_tag_to_msg(E_WAS_TAG_ID tag_id, tUInt8 ** pp_msg,
                                         const tUInt8 * p_key, tUInt32 key_len,
                                         const tUInt8 * p_val, tUInt32 val_len) {

  /* Check parameters */
  assert(E_WAS_TAG_ID_NONE != tag_id && E_WAS_TAG_ID_NB > tag_id && pp_msg &&
         p_key && key_len && p_val && val_len);

  /* Copy tag */
  memcpy(*pp_msg, gc_was_tag_name[tag_id], X_WAS_TAG_LENGTH);
  *pp_msg += X_WAS_TAG_LENGTH;

  /* Copy key length */
  X_WAS_U32_TO_U8A(key_len, *pp_msg);
  *pp_msg += X_WAS_TAG_LENGTH;

  /* Copy key */
  memcpy(*pp_msg, p_key, key_len);
  *pp_msg += key_len;

  /* Copy value length */
  X_WAS_U32_TO_U8A(val_len, *pp_msg);
  *pp_msg += X_WAS_TAG_LENGTH;

  /* Copy value */
  memcpy(*pp_msg, p_val, val_len);
  *pp_msg += val_len;
}

/* ========================================================================== */
static E_WAS_TAG_ID was_get_tag_id(const tUInt8 * p_tag,
                                   E_WAS_TAG_TYPE * p_type) {
  tUInt32 i = 0;

  assert(p_tag);

  /* Find tag string in the lookup table */
  for (i = E_WAS_TAG_ID_NONE; i < E_WAS_TAG_ID_NB; i++) {
    if (! (memcmp(p_tag, gc_was_tag_lookup_table[i].name, X_WAS_TAG_LENGTH))) {
      if (p_type) {
        *p_type = gc_was_tag_lookup_table[i].type;
      }
      return gc_was_tag_lookup_table[i].id;
    }
  }
  /* If not found, set tag type to TYPE_NONE */
  if (p_type) {
    *p_type = E_WAS_TAG_TYPE_NONE;
  }
  return E_WAS_TAG_ID_NONE;
}

/* ========================================================================== */
static tUInt32 was_compute_body_length(va_list ap) {
  tUInt8 * p_bs = NULL;
  tUInt32 body_len = 0;
  tUInt32 bs_len = 0;
  tUInt32 key_len = 0;
  tUInt32 val_len = 0;
  E_WAS_VA_LIST_STATE state = E_WAS_VA_LIST_WAIT_TAG_ID;
  E_WAS_TAG_ID tag_id = E_WAS_TAG_ID_NONE;

  /* First loop: compute the length of the body */
  while (E_WAS_VA_LIST_END != state) {
    switch (state) {
      case E_WAS_VA_LIST_WAIT_TAG_ID:
        bs_len = key_len = val_len = 0;
        tag_id = (E_WAS_TAG_ID) va_arg(ap, tUInt32);
        if (E_WAS_TAG_ID_NONE == tag_id || E_WAS_TAG_ID_NB <= tag_id) {
          state = E_WAS_VA_LIST_END;
        }
        else {
          switch (gc_was_tag_lookup_table[tag_id].type) {
            case E_WAS_TAG_TYPE_BYTE_STREAM:
              state = E_WAS_VA_LIST_WAIT_BYTE_STREAM_LEN;
            break;
            case E_WAS_TAG_TYPE_KEY_VAL:
              state = E_WAS_VA_LIST_WAIT_KEY_LEN;
            break;
            default:
              state = E_WAS_VA_LIST_END;
            break;
          }
        }
      break;

      case E_WAS_VA_LIST_WAIT_BYTE_STREAM_LEN:
        bs_len = va_arg(ap, tUInt32);
        state = E_WAS_VA_LIST_WAIT_BYTE_STREAM;
      break;

      case E_WAS_VA_LIST_WAIT_BYTE_STREAM:
        if ((p_bs = va_arg(ap, tUInt8 *))) {
          body_len += 2 * X_WAS_TAG_LENGTH + bs_len;
          state = E_WAS_VA_LIST_WAIT_TAG_ID;
        }
        else {
          state = E_WAS_VA_LIST_END;
        }
      break;

      case E_WAS_VA_LIST_WAIT_KEY_LEN:
        key_len = va_arg(ap, tUInt32);
        state = E_WAS_VA_LIST_WAIT_KEY;
      break;

      case E_WAS_VA_LIST_WAIT_KEY:
        if ((p_bs = va_arg(ap, tUInt8 *))) {
          state = E_WAS_VA_LIST_WAIT_VAL_LEN;
        }
        else {
          state = E_WAS_VA_LIST_END;
        }
      break;

      case E_WAS_VA_LIST_WAIT_VAL_LEN:
        val_len = va_arg(ap, tUInt32);
        state = E_WAS_VA_LIST_WAIT_VAL;
      break;

      case E_WAS_VA_LIST_WAIT_VAL:
        if ((p_bs = va_arg(ap, tUInt8 *))) {
          body_len += 3 * X_WAS_TAG_LENGTH + key_len + val_len;
          state = E_WAS_VA_LIST_WAIT_TAG_ID;
        }
        else {
          state = E_WAS_VA_LIST_END;
        }
      break;

      case E_WAS_VA_LIST_END:
      break;
    }
  }

  return body_len;
}

/* ========================================================================== */
static void was_write_msg_body(tUInt8 ** pp_body, va_list ap) {
  tUInt8 * p_bs = NULL;
  tUInt8 * p_key = NULL;
  tUInt8 * p_val = NULL;
  tUInt32 bs_len = 0;
  tUInt32 key_len = 0;
  tUInt32 val_len = 0;
  E_WAS_VA_LIST_STATE state = E_WAS_VA_LIST_WAIT_TAG_ID;
  E_WAS_TAG_ID tag_id = E_WAS_TAG_ID_NONE;

  /* Write the message body */
  while (E_WAS_VA_LIST_END != state) {
    switch (state) {
      case E_WAS_VA_LIST_WAIT_TAG_ID:
        bs_len = key_len = val_len = 0;
        p_bs = p_key = p_val = NULL;
        tag_id = (E_WAS_TAG_ID) va_arg(ap, tUInt32);
        if (E_WAS_TAG_ID_NONE == tag_id || E_WAS_TAG_ID_NB <= tag_id) {
          state = E_WAS_VA_LIST_END;
        }
        else {
          switch (gc_was_tag_lookup_table[tag_id].type) {
            case E_WAS_TAG_TYPE_BYTE_STREAM:
              state = E_WAS_VA_LIST_WAIT_BYTE_STREAM_LEN;
            break;
            case E_WAS_TAG_TYPE_KEY_VAL:
              state = E_WAS_VA_LIST_WAIT_KEY_LEN;
            break;
            default:
              state = E_WAS_VA_LIST_END;
            break;
          }
        }
      break;
       case E_WAS_VA_LIST_WAIT_BYTE_STREAM_LEN:
        bs_len = va_arg(ap, tUInt32);
        state = E_WAS_VA_LIST_WAIT_BYTE_STREAM;
      break;

      case E_WAS_VA_LIST_WAIT_BYTE_STREAM:
        if ((p_bs = va_arg(ap, tUInt8 *))) {
          was_write_byte_stream_tag_to_msg(tag_id, pp_body, p_bs, bs_len);
          state = E_WAS_VA_LIST_WAIT_TAG_ID;
        }
        else {
          state = E_WAS_VA_LIST_END;
        }
      break;

      case E_WAS_VA_LIST_WAIT_KEY_LEN:
        key_len = va_arg(ap, tUInt32);
        state = E_WAS_VA_LIST_WAIT_KEY;
      break;

      case E_WAS_VA_LIST_WAIT_KEY:
        if ((p_key = va_arg(ap, tUInt8 *))) {
          state = E_WAS_VA_LIST_WAIT_VAL_LEN;
        }
        else {
          state = E_WAS_VA_LIST_END;
        }
      break;

      case E_WAS_VA_LIST_WAIT_VAL_LEN:
        val_len = va_arg(ap, tUInt32);
        state = E_WAS_VA_LIST_WAIT_VAL;
      break;

      case E_WAS_VA_LIST_WAIT_VAL:
        if ((p_val = va_arg(ap, tUInt8 *))) {
          was_write_key_val_tag_to_msg(tag_id, pp_body, p_key, key_len,
                                       p_val, val_len);
          state = E_WAS_VA_LIST_WAIT_TAG_ID;
        }
        else {
          state = E_WAS_VA_LIST_END;
        }
      break;

      case E_WAS_VA_LIST_END:
      break;
    }
  }
}

/* ========================================================================== */
static void * was_malloc0(size_t size) {
  void * p_ptr = NULL;

  if (! size) {
  //  syslog(LOG_ERR, "malloc called with 0 size");
  }
  else if (NULL == (p_ptr = malloc(size))) {
  //  syslog(LOG_ERR, "failed to allocate %u bytes", size);
  }
  memset(p_ptr, 0x00, size);

  return p_ptr;
}

/* ========================================================================== */
static void * was_realloc(void * p_in, size_t size) {
  void * p_out = NULL;

  if (! size) {
  //  syslog(LOG_ERR, "realloc called with 0 size");
  }
  else if (NULL == (p_out = realloc(p_in, size))) {
  //  syslog(LOG_ERR, "failed to reallocate %u bytes", size);
  }

  return p_out;
}

/* ========================================================================== */
#ifdef __cplusplus 
extern "C" {
#endif
/* ========================================================================== */
EXPORT_MESSAGE void was_set_header_field(tUInt8 * p_msg,
                                 E_WAS_MSG_HEADER_FIELD_ID id,
                                 tUInt32 value) {
  S_WAS_MSG_HEADER * p_hdr = (S_WAS_MSG_HEADER *)p_msg;

  /* Check parameters */
  assert(p_hdr && ! memcmp(p_hdr->som,
                           gc_was_tag_lookup_table[E_WAS_TAG_ID_SOBM].name,
                           X_WAS_TAG_LENGTH));

  /* Set the desired field with the given value */
  switch (id) {
    case E_WAS_MSG_HEADER_FIELD_MPI:
      assert(E_WAS_MSG_PART_ID_NB > value);
      p_hdr->mpi = value;
    break;
    case E_WAS_MSG_HEADER_FIELD_TOM:
      assert(E_WAS_MSG_TYPE_NB > value);
      p_hdr->tom = value;
    break;
    case E_WAS_MSG_HEADER_FIELD_TID:
      p_hdr->tid = value;
    break;
    case E_WAS_MSG_HEADER_FIELD_RSPC:
      p_hdr->rspc = value;
    break;
    case E_WAS_MSG_HEADER_FIELD_LOB:
      p_hdr->lob = value;
    break;
    default:
      assert(0);
    break;
  }
}

/* ========================================================================== */
EXPORT_MESSAGE tUInt32 was_get_header_field(tUInt8 * p_msg,
                                     E_WAS_MSG_HEADER_FIELD_ID id) {
  S_WAS_MSG_HEADER * p_hdr = (S_WAS_MSG_HEADER *)p_msg;
  tUInt32 value = 0;

  /* Check parameters */
  assert(p_hdr && ! memcmp(p_hdr->som,
                           gc_was_tag_lookup_table[E_WAS_TAG_ID_SOBM].name,
                           X_WAS_TAG_LENGTH));

  /* Get the desired field value */
  switch (id) {
    case E_WAS_MSG_HEADER_FIELD_MPI:
      value = p_hdr->mpi;
    break;
    case E_WAS_MSG_HEADER_FIELD_TOM:
      value = p_hdr->tom;
    break;
    case E_WAS_MSG_HEADER_FIELD_TID:
      value = p_hdr->tid;
    break;
    case E_WAS_MSG_HEADER_FIELD_RSPC:
      value = p_hdr->rspc;
    break;
    case E_WAS_MSG_HEADER_FIELD_LOB:
      value = p_hdr->lob;
    break;
    default:
      assert(0);
    break;
  }

  return value;
}

/* ========================================================================== */
EXPORT_MESSAGE tUInt8 * was_write_msg(tUInt32 * p_len, tUInt32 mpi, tUInt32 tom,
                               tUInt32 tid, tUInt32 rspc, ...) {
  tUInt8 * p_msg = NULL;
  va_list ap;

  /* Check parameters */
  assert(p_len);

  /* Get the va_list */
  va_start(ap, rspc);

  /* Call was_write_msgv */
  p_msg = was_write_msgv(p_len, mpi, tom, tid, rspc, ap);

  /* Release the va_list */
  va_end(ap);

  return p_msg;
}

/* ========================================================================== */
EXPORT_MESSAGE tUInt8 * was_write_msgv(tUInt32 * p_len, tUInt32 mpi, tUInt32 tom,
                                tUInt32 tid, tUInt32 rspc, va_list ap) {
  va_list ap2 = NULL;
  tUInt8 * p_msg = NULL;
  tUInt8 * p_body = NULL;
  tUInt32 body_len = 0;

  /* Check parameters */
  assert(p_len);

  /* Copy the va_list */
  //va_copy(ap2, ap);

  /* Compute the body length */
  body_len = was_compute_body_length(ap);

  /* Compute the message length */
  *p_len = sizeof(S_WAS_MSG_HEADER) + body_len + sizeof(S_WAS_MSG_FOOTER);

  /* Allocate message */
  if ((p_msg = (tUInt8 *) was_malloc0(*p_len))) {

    /* Write header */
    was_write_header((S_WAS_MSG_HEADER *) p_msg, mpi, tom, tid, rspc, body_len);

    /* Write body */
    p_body = p_msg + sizeof(S_WAS_MSG_HEADER);
    was_write_msg_body(&p_body, ap);

    /* Write footer */
    was_write_footer(p_body);
  }
  else {
    *p_len = 0;
  }

  /* Release the va_list */
  va_end(ap);

  return p_msg;
}

/* ========================================================================== */
EXPORT_MESSAGE tUInt8 * was_append_to_msg(tUInt32 * p_len, tUInt8 * p_in, ...) {
  tUInt8 * p_msg = NULL;
  va_list ap;

  /* Check parameters */
  assert(p_len && p_in);

  /* Get the va_list */
  va_start(ap, p_in);

  /* Call was_append_to_msgv */
  p_msg = was_append_to_msgv(p_len, p_in, ap);

  /* Release the va_list */
  va_end(ap);

  return p_msg;
}


/* ========================================================================== */
EXPORT_MESSAGE tUInt8 * was_append_to_msgv(tUInt32 * p_len,
                                    tUInt8 * p_in,
                                    va_list ap) {
  va_list ap2 = NULL;
  tUInt8 * p_msg = NULL;
  tUInt8 * p_body = NULL;
  tUInt32 in_body_len = 0;
  tUInt32 body_len = 0;

  /* Check parameters */
  assert(p_len && p_in);

  /* Initialize the body length */
  in_body_len = body_len = ((S_WAS_MSG_HEADER *)p_in)->lob;
  
  /* Copy the va_list */
  //va_copy(ap2, ap);

  /* Compute the body length */
  body_len += was_compute_body_length(ap);

  /* Compute the message length */
  *p_len = sizeof(S_WAS_MSG_HEADER) + body_len + sizeof(S_WAS_MSG_FOOTER);

  /* Allocate message */
  if ((p_msg = (tUInt8 *) was_realloc(p_in, *p_len))) {

    /* Update header: length of body */
    ((S_WAS_MSG_HEADER *)p_msg)->lob = body_len;
    
    /* Append to body */
    p_body = p_msg + sizeof(S_WAS_MSG_HEADER) + in_body_len;
    was_write_msg_body(&p_body, ap);

    /* Write footer */
    was_write_footer(p_body);
  }
  else {
    *p_len = 0;
  }

  /* Release the va_list */
  va_end(ap);

  return p_msg;
}

/* ========================================================================== */
EXPORT_MESSAGE E_WAS_TAG_ID was_get_first_tag_data(const tUInt8 * p_msg, tUInt32 len,
                                           S_WAS_TAG_DATA * p_td) {

  /* Check parameters */
  assert(p_msg && p_td && len);

  /* Reset offset */
  p_td->offset = 0;

  /* Call was_get_next_tag_data */
  return was_get_next_tag_data(p_msg, len, p_td);
}

/* ========================================================================== */
EXPORT_MESSAGE E_WAS_TAG_ID was_get_next_tag_data(const tUInt8 * p_msg, tUInt32 len,
                                          S_WAS_TAG_DATA * p_td) {
  const tUInt8 * p_cur = p_msg;
  const tUInt8 * p_ptr = NULL;
  tUInt32 remaining_len = len;
  tUInt32 offset = 0;
  tUInt32 len1 = 0;
  tUInt32 len2 = 0;
  E_WAS_TAG_ID tag_id = E_WAS_TAG_ID_NONE;
  E_WAS_TAG_TYPE tag_type = E_WAS_TAG_TYPE_NONE;

  /* Check parameters */
  assert(p_msg && p_td && len);

  /* Get the offset and set current pointer position and remaining length */
  if (p_td->offset) {
    offset = p_td->offset;
  }
  else {
    offset = sizeof(S_WAS_MSG_HEADER);
  }
  p_cur += offset;
  remaining_len -= offset;

  /* Initialize the S_WAS_TAG_DATA structure */
  memset(p_td, 0x00, sizeof(S_WAS_TAG_DATA));
  p_td->tag_type = E_WAS_TAG_TYPE_NONE;
  p_td->tag_id = E_WAS_TAG_ID_NONE;

  /* Check that at least a E_WAS_TAG_TYPE_BYTE_STREAM */
  /* can fit in the remaining length */
  if (X_WAS_TAG_DATA_MIN_LEN <= remaining_len) {
    /* Check that tag id is valid */
    if (E_WAS_TAG_ID_NONE != (tag_id = was_get_tag_id(p_cur, &tag_type)) &&
        E_WAS_TAG_TYPE_NONE != tag_type) {
      p_cur += X_WAS_TAG_LENGTH;
      len1 = X_WAS_U8A_TO_U32(p_cur);
      p_cur += X_WAS_TAG_LENGTH;
      remaining_len -= 2 * X_WAS_TAG_LENGTH;
      /* Check that length is not 0 and that the remaining length */
      /* is enough to store the byte stream */
      if (len1 && remaining_len >= len1) {
        p_ptr = p_cur;
        switch (tag_type) {
          case E_WAS_TAG_TYPE_BYTE_STREAM:
            p_td->tag_type = tag_type;
            p_td->tag_id = tag_id;
            p_td->data.bs.byte_stream_len = len1;
            p_td->data.bs.byte_stream = p_cur;
            p_td->offset = p_cur + len1 - p_msg;
          break;

          case E_WAS_TAG_TYPE_KEY_VAL:
            p_cur += len1;
            remaining_len -= len1;
            if (X_WAS_TAG_LENGTH <= remaining_len) {
              len2 = X_WAS_U8A_TO_U32(p_cur);
              p_cur += X_WAS_TAG_LENGTH;
              remaining_len -= X_WAS_TAG_LENGTH;
              if (len2 && remaining_len >= len2) {
                p_td->tag_type = tag_type;
                p_td->tag_id = tag_id;
                p_td->data.kv.key_len = len1;
                p_td->data.kv.key = p_ptr;
                p_td->data.kv.val_len = len2;
                p_td->data.kv.val = p_cur;
                p_td->offset = p_cur + len2 - p_msg;
              }
            }
          break;

          default:
          break;
        }
      }
    }
  }

  return p_td->tag_id;
}

/* ========================================================================== */
EXPORT_MESSAGE void was_free_msg(void ** pp_ptr) {

  //if (! pp_ptr) {
  //  syslog(LOG_WARNING, "free: NULL parameter");
  //}
  //else if (! *pp_ptr) {
  //  syslog(LOG_WARNING, "free of a NULL pointer");
  //}
  //else {
    free(*pp_ptr);
    *pp_ptr = NULL;
  //}
}

/* ========================================================================== */
EXPORT_MESSAGE E_WAS_MSG_VALIDITY was_is_msg_valid(const tUInt8 * p_msg,
                                           tUInt32 * p_len) {
  S_WAS_MSG_HEADER * p_hdr = (S_WAS_MSG_HEADER *)p_msg;

  assert(p_msg && p_len);

  /* A valid message shall  contain at least the header and footer */
  if (X_WAS_MSG_MIN_SIZE > *p_len) {
     return E_WAS_MSG_INVALID_HEADER_TOO_SHORT;
  }

  if (memcmp(p_hdr->som, gc_was_tag_lookup_table[E_WAS_TAG_ID_SOBM].name,
             X_WAS_TAG_LENGTH)) {
    return E_WAS_MSG_INVALID_HEADER_TAG;
  }
      
  //if (E_WAS_MSG_PART_ID_NB <= p_hdr->mpi) {
  //  return E_WAS_MSG_INVALID_HEADER_MPI;
  //}
      
  if (E_WAS_MSG_TYPE_NB <= p_hdr->tom) {
    return E_WAS_MSG_INVALID_HEADER_TOM;
  }
      
  if (X_WAS_MAX_LOB_SIZE <= p_hdr->lob) {
    return E_WAS_MSG_INVALID_HEADER_LOB;
  }

  if ((X_WAS_MSG_MIN_SIZE + p_hdr->lob) > *p_len) {
    return E_WAS_MSG_INVALID_TOO_SHORT;
  }
  
  if (memcmp(p_msg + sizeof(S_WAS_MSG_HEADER) + p_hdr->lob,
             gc_was_tag_lookup_table[E_WAS_TAG_ID_EOBM].name,
             X_WAS_TAG_LENGTH)) {
    return E_WAS_MSG_INVALID_FOOTER_TAG;
  }

  *p_len = X_WAS_MSG_MIN_SIZE + p_hdr->lob;

  return E_WAS_MSG_VALID;
}

/* ========================================================================== */
EXPORT_MESSAGE E_WAS_MSG_RESYNC was_resync(const tUInt8 * p_buf,
                                   tUInt32 buf_len,
                                   tUInt32 * p_msg_offset,
                                   tUInt32 * p_msg_len) {
  const tUInt8 * p_sync = p_buf;
  const tUInt8 * p_first = p_buf;
  tUInt32 rem_len  = buf_len;
  E_WAS_MSG_VALIDITY valid = E_WAS_MSG_INVALID_HEADER_TAG;
  char sync_char = *gc_was_tag_lookup_table[E_WAS_TAG_ID_SOBM].name;

  assert(p_buf && buf_len && p_msg_offset && p_msg_len);
  *p_msg_offset = *p_msg_len = 0;

  while (rem_len &&
         (p_first = (const tUInt8 *)memchr(p_sync, sync_char, rem_len))) {
    *p_msg_len = rem_len = buf_len - (tUInt32) (p_first - p_sync);
    valid = was_is_msg_valid(p_first, p_msg_len);
    if (E_WAS_MSG_VALID == valid || E_WAS_MSG_INVALID_TOO_SHORT == valid) {
      *p_msg_offset = p_first - p_buf;
      if (E_WAS_MSG_VALID == valid) {
        return E_WAS_MSG_SYNC;
      }
      else if (E_WAS_MSG_INVALID_TOO_SHORT) {
        return E_WAS_MSG_NOT_SYNC_TOO_SHORT;
      }
    }
    p_sync = p_first + 1;
  }

  return E_WAS_MSG_NOT_SYNC;
}
  
#ifdef __cplusplus 
} /* extern "C" */
#endif

#undef WAS_API_C