#include <assert.h>
#include <bare.h>
#include <bitarray.h>
#include <js.h>
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
  err = js_get_arraybuffer_info(env, argv[0], (void **) &bitarray, NULL);
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

static js_value_t *
bitarray_native_insert (js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 3;
  js_value_t *argv[3];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  assert(argc == 3);

  bitarray_native_t *bitarray;
  err = js_get_arraybuffer_info(env, argv[0], (void **) &bitarray, NULL);
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
  err = js_get_arraybuffer_info(env, argv[0], (void **) &bitarray, NULL);
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

static js_value_t *
bitarray_native_get (js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 2;
  js_value_t *argv[2];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  assert(argc == 2);

  bitarray_native_t *bitarray;
  err = js_get_arraybuffer_info(env, argv[0], (void **) &bitarray, NULL);
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
  err = js_get_arraybuffer_info(env, argv[0], (void **) &bitarray, NULL);
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
bitarray_native_fill (js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 4;
  js_value_t *argv[4];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  assert(argc == 4);

  bitarray_native_t *bitarray;
  err = js_get_arraybuffer_info(env, argv[0], (void **) &bitarray, NULL);
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

static js_value_t *
bitarray_native_find_first (js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 3;
  js_value_t *argv[3];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  assert(argc == 3);

  bitarray_native_t *bitarray;
  err = js_get_arraybuffer_info(env, argv[0], (void **) &bitarray, NULL);
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

static js_value_t *
bitarray_native_find_last (js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 3;
  js_value_t *argv[3];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  assert(argc == 3);

  bitarray_native_t *bitarray;
  err = js_get_arraybuffer_info(env, argv[0], (void **) &bitarray, NULL);
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

static js_value_t *
bitarray_native_count (js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 4;
  js_value_t *argv[4];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  assert(argc == 4);

  bitarray_native_t *bitarray;
  err = js_get_arraybuffer_info(env, argv[0], (void **) &bitarray, NULL);
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

#define V(name, fn) \
  { \
    js_value_t *val; \
    err = js_create_function(env, name, -1, fn, NULL, &val); \
    assert(err == 0); \
    err = js_set_named_property(env, exports, name, val); \
    assert(err == 0); \
  }

  V("init", bitarray_native_init)
  V("destroy", bitarray_native_destroy)
  V("insert", bitarray_native_insert)
  V("clear", bitarray_native_clear)
  V("get", bitarray_native_get)
  V("set", bitarray_native_set)
  V("fill", bitarray_native_fill)
  V("findFirst", bitarray_native_find_first)
  V("findLast", bitarray_native_find_last)
  V("count", bitarray_native_count)
#undef V

  return exports;
}

BARE_MODULE(bitarray_native, bitarray_native_exports)
