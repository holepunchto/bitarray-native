{
  'targets': [{
    'target_name': 'bitarray',
    'include_dirs': [
      '<!(bare-dev paths compat/napi)',
    ],
    'dependencies': [
      './vendor/libbitarray/libbitarray.gyp:libbitarray',
    ],
    'sources': [
      './binding.c',
    ],
    'configurations': {
      'Debug': {
        'defines': ['DEBUG'],
      },
      'Release': {
        'defines': ['NDEBUG'],
      },
    },
  }]
}
