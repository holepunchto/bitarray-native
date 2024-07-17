#include <assert.h>
#include <bare.h>
#include <bitarray.h>
#include <js.h>
#include <js/ffi.h>
#include <stdint.h>

typedef struct {
  bitarray_t handle;

  js_env_t *env;
  js_ref_t *ctx;
  js_ref_t *on_alloc;
  js_ref_t *on_free;
} bitarray_native_t;

typedef struct {
  uint32_t id;
} bitarray_native_allocation_t;

static void *
bitarray_native__on_alloc (size_t size, bitarray_t *handle) {
  int err;

  bitarray_native_t *bitarray = (bitarray_native_t *) handle->data;

  js_env_t *env = bitarray->env;

  js_escapable_handle_scope_t *scope;
  err = js_open_escapable_handle_scope(env, &scope);
  assert(err == 0);

  js_value_t *ctx;
  err = js_get_reference_value(env, bitarray->ctx, &ctx);
  assert(err == 0);

  js_value_t *cb;
  err = js_get_reference_value(env, bitarray->on_alloc, &cb);
  assert(err == 0);

  js_value_t *args[1];

  err = js_create_uint32(env, sizeof(bitarray_native_allocation_t) + size, &args[0]);
  assert(err == 0);

  js_value_t *result;

  err = js_call_function(env, ctx, cb, 1, args, &result);
  assert(err == 0);

  err = js_escape_handle(env, scope, result, &result);
  assert(err == 0);

  bitarray_native_allocation_t *allocation;
  err = js_get_typedarray_info(env, result, NULL, (void *) &allocation, NULL, NULL, NULL);
  assert(err == 0);

  err = js_close_escapable_handle_scope(env, scope);
  assert(err == 0);

  return ((char *) allocation) + sizeof(bitarray_native_allocation_t);
}

static void
bitarray_native__on_free (void *ptr, bitarray_t *handle) {
  int err;

  bitarray_native_t *bitarray = (bitarray_native_t *) handle->data;

  js_env_t *env = bitarray->env;

  js_handle_scope_t *scope;
  err = js_open_handle_scope(env, &scope);
  assert(err == 0);

  js_value_t *ctx;
  err = js_get_reference_value(env, bitarray->ctx, &ctx);
  assert(err == 0);

  js_value_t *cb;
  err = js_get_reference_value(env, bitarray->on_free, &cb);
  assert(err == 0);

  bitarray_native_allocation_t *allocation = (bitarray_native_allocation_t *) (((char *) ptr) - sizeof(bitarray_native_allocation_t));

  js_value_t *args[1];

  err = js_create_uint32(env, allocation->id, &args[0]);
  assert(err == 0);

  err = js_call_function(env, ctx, cb, 1, args, NULL);
  assert(err == 0);

  err = js_close_handle_scope(env, scope);
  assert(err == 0);
}

static js_value_t *
bitarray_native_init (js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 3;
  js_value_t *argv[3];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  assert(argc == 3);

  js_value_t *handle;

  bitarray_native_t *bitarray;
  err = js_create_arraybuffer(env, sizeof(bitarray_native_t), (void **) &bitarray, &handle);
  assert(err == 0);

  bitarray->env = env;
  bitarray->handle.data = (void *) bitarray;

  err = bitarray_init(&bitarray->handle, bitarray_native__on_alloc, bitarray_native__on_free);
  assert(err == 0);

  err = js_create_reference(env, argv[0], 1, &bitarray->ctx);
  assert(err == 0);

  err = js_create_reference(env, argv[1], 1, &bitarray->on_alloc);
  assert(err == 0);

  err = js_create_reference(env, argv[2], 1, &bitarray->on_free);
  assert(err == 0);

  return handle;
}

static js_value_t *
bitarray_native_destroy (js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 1;
  js_value_t *argv[1];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  assert(argc == 1);

  bitarray_native_t *bitarray;
  err = js_get_typedarray_info(env, argv[0], NULL, (void **) &bitarray, NULL, NULL, NULL);
  assert(err == 0);

  bitarray_destroy(&bitarray->handle);

  err = js_delete_reference(env, bitarray->on_alloc);
  assert(err == 0);

  err = js_delete_reference(env, bitarray->on_free);
  assert(err == 0);

  err = js_delete_reference(env, bitarray->ctx);
  assert(err == 0);

  return NULL;
}

static int64_t
bitarray_native_page_fast (js_ffi_receiver_t *receiver, js_ffi_typedarray_t *handle, uint32_t i) {
  bitarray_native_t *bitarray = (bitarray_native_t *) handle->data.u8;

  bitarray_page_t *page = bitarray_page(&bitarray->handle, i);

  int64_t id = -1;

  if (page) {
    bitarray_native_allocation_t *allocation = (bitarray_native_allocation_t *) (((char *) page) - sizeof(bitarray_native_allocation_t));

    id = allocation->id;
  }

  return id;
}

static js_value_t *
bitarray_native_page (js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 2;
  js_value_t *argv[2];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  assert(argc == 2);

  bitarray_native_t *bitarray;
  err = js_get_typedarray_info(env, argv[0], NULL, (void **) &bitarray, NULL, NULL, NULL);
  assert(err == 0);

  uint32_t i;
  err = js_get_value_uint32(env, argv[1], &i);
  assert(err == 0);

  bitarray_page_t *page = bitarray_page(&bitarray->handle, i);

  int64_t id = -1;

  if (page) {
    bitarray_native_allocation_t *allocation = (bitarray_native_allocation_t *) (((char *) page) - sizeof(bitarray_native_allocation_t));

    id = allocation->id;
  }

  js_value_t *result;
  err = js_create_int64(env, id, &result);
  assert(err == 0);

  return result;
}

static js_value_t *
bitarray_native_insert (js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 3;
  js_value_t *argv[3];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  assert(argc == 3);

  bitarray_native_t *bitarray;
  err = js_get_typedarray_info(env, argv[0], NULL, (void **) &bitarray, NULL, NULL, NULL);
  assert(err == 0);

  uint8_t *data;
  size_t len;
  err = js_get_typedarray_info(env, argv[1], NULL, (void **) &data, (size_t *) &len, NULL, NULL);
  assert(err == 0);

  int64_t start;
  err = js_get_value_int64(env, argv[2], &start);
  assert(err == 0);

  err = bitarray_insert(&bitarray->handle, data, len, start);
  assert(err == 0);

  return NULL;
}

static js_value_t *
bitarray_native_clear (js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 3;
  js_value_t *argv[3];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  assert(argc == 3);

  bitarray_native_t *bitarray;
  err = js_get_typedarray_info(env, argv[0], NULL, (void **) &bitarray, NULL, NULL, NULL);
  assert(err == 0);

  uint8_t *data;
  size_t len;
  err = js_get_typedarray_info(env, argv[1], NULL, (void **) &data, (size_t *) &len, NULL, NULL);
  assert(err == 0);

  int64_t start;
  err = js_get_value_int64(env, argv[2], &start);
  assert(err == 0);

  err = bitarray_clear(&bitarray->handle, data, len, start);
  assert(err == 0);

  return NULL;
}

static bool
bitarray_native_get_fast (js_ffi_receiver_t *receiver, js_ffi_typedarray_t *handle, int64_t bit) {
  bitarray_native_t *bitarray = (bitarray_native_t *) handle->data.u8;

  return bitarray_get(&bitarray->handle, bit);
}

static js_value_t *
bitarray_native_get (js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 2;
  js_value_t *argv[2];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  assert(argc == 2);

  bitarray_native_t *bitarray;
  err = js_get_typedarray_info(env, argv[0], NULL, (void **) &bitarray, NULL, NULL, NULL);
  assert(err == 0);

  int64_t bit;
  err = js_get_value_int64(env, argv[1], &bit);
  assert(err == 0);

  js_value_t *result;
  err = js_get_boolean(env, bitarray_get(&bitarray->handle, bit), &result);
  assert(err == 0);

  return result;
}

static js_value_t *
bitarray_native_set (js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 3;
  js_value_t *argv[3];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  assert(argc == 3);

  bitarray_native_t *bitarray;
  err = js_get_typedarray_info(env, argv[0], NULL, (void **) &bitarray, NULL, NULL, NULL);
  assert(err == 0);

  int64_t bit;
  err = js_get_value_int64(env, argv[1], &bit);
  assert(err == 0);

  bool value;
  err = js_get_value_bool(env, argv[2], &value);
  assert(err == 0);

  js_value_t *result;
  err = js_get_boolean(env, bitarray_set(&bitarray->handle, bit, value), &result);
  assert(err == 0);

  return result;
}

static js_value_t *
bitarray_native_set_batch (js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 3;
  js_value_t *argv[3];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  assert(argc == 3);

  bitarray_native_t *bitarray;
  err = js_get_typedarray_info(env, argv[0], NULL, (void **) &bitarray, NULL, NULL, NULL);
  assert(err == 0);

  uint32_t len;
  err = js_get_array_length(env, argv[1], &len);
  assert(err == 0);

  bool value;
  err = js_get_value_bool(env, argv[2], &value);
  assert(err == 0);

  bool changed = false;

  for (uint32_t i = 0, n = len; i < n; i++) {
    js_value_t *element;
    err = js_get_element(env, argv[1], i, &element);
    assert(err == 0);

    int64_t bit;
    err = js_get_value_int64(env, element, &bit);
    assert(err == 0);

    changed = bitarray_set(&bitarray->handle, bit, value) || changed;
  }

  js_value_t *result;
  err = js_get_boolean(env, changed, &result);
  assert(err == 0);

  return result;
}

static js_value_t *
bitarray_native_fill (js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 4;
  js_value_t *argv[4];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  assert(argc == 4);

  bitarray_native_t *bitarray;
  err = js_get_typedarray_info(env, argv[0], NULL, (void **) &bitarray, NULL, NULL, NULL);
  assert(err == 0);

  bool value;
  err = js_get_value_bool(env, argv[1], &value);
  assert(err == 0);

  int64_t start;
  err = js_get_value_int64(env, argv[2], &start);
  assert(err == 0);

  int64_t end;
  err = js_get_value_int64(env, argv[3], &end);
  assert(err == 0);

  bitarray_fill(&bitarray->handle, value, start, end);

  return NULL;
}

static int64_t
bitarray_native_find_first_fast (js_ffi_receiver_t *receiver, js_ffi_typedarray_t *handle, bool value, int64_t pos) {
  bitarray_native_t *bitarray = (bitarray_native_t *) handle->data.u8;

  return bitarray_find_first(&bitarray->handle, value, pos);
}

static js_value_t *
bitarray_native_find_first (js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 3;
  js_value_t *argv[3];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  assert(argc == 3);

  bitarray_native_t *bitarray;
  err = js_get_typedarray_info(env, argv[0], NULL, (void **) &bitarray, NULL, NULL, NULL);
  assert(err == 0);

  bool value;
  err = js_get_value_bool(env, argv[1], &value);
  assert(err == 0);

  int64_t pos;
  err = js_get_value_int64(env, argv[2], &pos);
  assert(err == 0);

  js_value_t *result;
  err = js_create_int64(env, bitarray_find_first(&bitarray->handle, value, pos), &result);
  assert(err == 0);

  return result;
}

static int64_t
bitarray_native_find_last_fast (js_ffi_receiver_t *receiver, js_ffi_typedarray_t *handle, bool value, int64_t pos) {
  bitarray_native_t *bitarray = (bitarray_native_t *) handle->data.u8;

  return bitarray_find_last(&bitarray->handle, value, pos);
}

static js_value_t *
bitarray_native_find_last (js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 3;
  js_value_t *argv[3];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  assert(argc == 3);

  bitarray_native_t *bitarray;
  err = js_get_typedarray_info(env, argv[0], NULL, (void **) &bitarray, NULL, NULL, NULL);
  assert(err == 0);

  bool value;
  err = js_get_value_bool(env, argv[1], &value);
  assert(err == 0);

  int64_t pos;
  err = js_get_value_int64(env, argv[2], &pos);
  assert(err == 0);

  js_value_t *result;
  err = js_create_int64(env, bitarray_find_last(&bitarray->handle, value, pos), &result);
  assert(err == 0);

  return result;
}

static int64_t
bitarray_native_count_fast (js_ffi_receiver_t *receiver, js_ffi_typedarray_t *handle, bool value, int64_t start, int64_t end) {
  bitarray_native_t *bitarray = (bitarray_native_t *) handle->data.u8;

  return bitarray_count(&bitarray->handle, value, start, end);
}

static js_value_t *
bitarray_native_count (js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 4;
  js_value_t *argv[4];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  assert(argc == 4);

  bitarray_native_t *bitarray;
  err = js_get_typedarray_info(env, argv[0], NULL, (void **) &bitarray, NULL, NULL, NULL);
  assert(err == 0);

  bool value;
  err = js_get_value_bool(env, argv[1], &value);
  assert(err == 0);

  int64_t start;
  err = js_get_value_int64(env, argv[2], &start);
  assert(err == 0);

  int64_t end;
  err = js_get_value_int64(env, argv[3], &end);
  assert(err == 0);

  js_value_t *result;
  err = js_create_int64(env, bitarray_count(&bitarray->handle, value, start, end), &result);
  assert(err == 0);

  return result;
}

static js_value_t *
bitarray_native_exports (js_env_t *env, js_value_t *exports) {
  int err;

#define V(name, fn, ffi) \
  { \
    js_value_t *val; \
    if (ffi) { \
      err = js_create_function_with_ffi(env, name, -1, fn, NULL, ffi, &val); \
    } else { \
      err = js_create_function(env, name, -1, fn, NULL, &val); \
    } \
    assert(err == 0); \
    err = js_set_named_property(env, exports, name, val); \
    assert(err == 0); \
  }

  V("init", bitarray_native_init, NULL)
  V("destroy", bitarray_native_destroy, NULL)

  {
    js_ffi_type_info_t *return_info;
    err = js_ffi_create_type_info(js_ffi_int64, &return_info);
    assert(err == 0);

    js_ffi_type_info_t *arg_info[3];

    err = js_ffi_create_type_info(js_ffi_receiver, &arg_info[0]);
    assert(err == 0);

    err = js_ffi_create_type_info(js_ffi_uint8array, &arg_info[1]);
    assert(err == 0);

    err = js_ffi_create_type_info(js_ffi_uint32, &arg_info[2]);
    assert(err == 0);

    js_ffi_function_info_t *function_info;
    err = js_ffi_create_function_info(return_info, arg_info, 3, &function_info);
    assert(err == 0);

    js_ffi_function_t *ffi;
    err = js_ffi_create_function(bitarray_native_page_fast, function_info, &ffi);
    assert(err == 0);

    V("page", bitarray_native_page, ffi)
  }

  V("insert", bitarray_native_insert, NULL)
  V("clear", bitarray_native_clear, NULL)

  {
    js_ffi_type_info_t *return_info;
    err = js_ffi_create_type_info(js_ffi_bool, &return_info);
    assert(err == 0);

    js_ffi_type_info_t *arg_info[3];

    err = js_ffi_create_type_info(js_ffi_receiver, &arg_info[0]);
    assert(err == 0);

    err = js_ffi_create_type_info(js_ffi_uint8array, &arg_info[1]);
    assert(err == 0);

    err = js_ffi_create_type_info(js_ffi_int64, &arg_info[2]);
    assert(err == 0);

    js_ffi_function_info_t *function_info;
    err = js_ffi_create_function_info(return_info, arg_info, 3, &function_info);
    assert(err == 0);

    js_ffi_function_t *ffi;
    err = js_ffi_create_function(bitarray_native_get_fast, function_info, &ffi);
    assert(err == 0);

    V("get", bitarray_native_get, ffi)
  }

  V("set", bitarray_native_set, NULL)
  V("setBatch", bitarray_native_set_batch, NULL)
  V("fill", bitarray_native_fill, NULL)

  {
    js_ffi_type_info_t *return_info;
    err = js_ffi_create_type_info(js_ffi_int64, &return_info);
    assert(err == 0);

    js_ffi_type_info_t *arg_info[4];

    err = js_ffi_create_type_info(js_ffi_receiver, &arg_info[0]);
    assert(err == 0);

    err = js_ffi_create_type_info(js_ffi_uint8array, &arg_info[1]);
    assert(err == 0);

    err = js_ffi_create_type_info(js_ffi_bool, &arg_info[2]);
    assert(err == 0);

    err = js_ffi_create_type_info(js_ffi_int64, &arg_info[3]);
    assert(err == 0);

    js_ffi_function_info_t *function_info;
    err = js_ffi_create_function_info(return_info, arg_info, 4, &function_info);
    assert(err == 0);

    js_ffi_function_t *ffi;
    err = js_ffi_create_function(bitarray_native_find_first_fast, function_info, &ffi);
    assert(err == 0);

    V("findFirst", bitarray_native_find_first, ffi)
  }

  {
    js_ffi_type_info_t *return_info;
    err = js_ffi_create_type_info(js_ffi_int64, &return_info);
    assert(err == 0);

    js_ffi_type_info_t *arg_info[4];

    err = js_ffi_create_type_info(js_ffi_receiver, &arg_info[0]);
    assert(err == 0);

    err = js_ffi_create_type_info(js_ffi_uint8array, &arg_info[1]);
    assert(err == 0);

    err = js_ffi_create_type_info(js_ffi_bool, &arg_info[2]);
    assert(err == 0);

    err = js_ffi_create_type_info(js_ffi_int64, &arg_info[3]);
    assert(err == 0);

    js_ffi_function_info_t *function_info;
    err = js_ffi_create_function_info(return_info, arg_info, 4, &function_info);
    assert(err == 0);

    js_ffi_function_t *ffi;
    err = js_ffi_create_function(bitarray_native_find_last_fast, function_info, &ffi);
    assert(err == 0);

    V("findLast", bitarray_native_find_last, ffi)
  }

  {
    js_ffi_type_info_t *return_info;
    err = js_ffi_create_type_info(js_ffi_int64, &return_info);
    assert(err == 0);

    js_ffi_type_info_t *arg_info[5];

    err = js_ffi_create_type_info(js_ffi_receiver, &arg_info[0]);
    assert(err == 0);

    err = js_ffi_create_type_info(js_ffi_uint8array, &arg_info[1]);
    assert(err == 0);

    err = js_ffi_create_type_info(js_ffi_bool, &arg_info[2]);
    assert(err == 0);

    err = js_ffi_create_type_info(js_ffi_int64, &arg_info[3]);
    assert(err == 0);

    err = js_ffi_create_type_info(js_ffi_int64, &arg_info[4]);
    assert(err == 0);

    js_ffi_function_info_t *function_info;
    err = js_ffi_create_function_info(return_info, arg_info, 5, &function_info);
    assert(err == 0);

    js_ffi_function_t *ffi;
    err = js_ffi_create_function(bitarray_native_count_fast, function_info, &ffi);
    assert(err == 0);

    V("count", bitarray_native_count, ffi)
  }
#undef V

  js_value_t *constants;
  err = js_create_object(env, &constants);
  assert(err == 0);

  err = js_set_named_property(env, exports, "constants", constants);
  assert(err == 0);

#define V(name, n) \
  { \
    js_value_t *val; \
    err = js_create_uint32(env, n, &val); \
    assert(err == 0); \
    err = js_set_named_property(env, constants, name, val); \
    assert(err == 0); \
  }

  V("BYTES_PER_PAGE", BITARRAY_BYTES_PER_PAGE)

  V("PAGE_BITFIELD_OFFSET", offsetof(bitarray_page_t, bitfield))
#undef V

  return exports;
}

BARE_MODULE(bitarray_native, bitarray_native_exports)
