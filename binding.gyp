{
  'targets': [
    {
      'target_name': 'indi-native',
      'sources': [ 'src/indi.cc', 'src/client.cc', 'src/device.cc', 'src/value.cc', 'src/promise.cc', 'src/prop.cc' ],
      'include_dirs': ["<!@(node -p \"require('node-addon-api').include\")"],
      'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
      'cflags!': [ '-fno-exceptions', '-DDEBUG' ],
      'cflags_cc!': [ '-fno-exceptions', '-DDEBUG' ],
      'libraries': [
        '-lindiclient',
      ],
      'xcode_settings': {
        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
        'CLANG_CXX_LIBRARY': 'libc++',
        'MACOSX_DEPLOYMENT_TARGET': '10.7'
      },
      'msvs_settings': {
        'VCCLCompilerTool': { 'ExceptionHandling': 1 },
      }
    }
  ]
}
