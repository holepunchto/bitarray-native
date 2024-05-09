{
  'targets': [{
    'target_name': 'bitarray',
    'include_dirs': [
      '<!(node -e "require(\'napi-macros\')")',
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
