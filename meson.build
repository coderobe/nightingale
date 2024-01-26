project('nightingale', 'cpp', default_options: ['cpp_std=c++17'])

add_project_arguments(
#  '-fsanitize=address',
  '-O0',
  '-g',
  '-fno-omit-frame-pointer',
  language: 'cpp'
)
add_project_link_arguments(
#  '-fsanitize=address',
  language: 'cpp'
)

executable('nightingale',
  sources: run_command('find', '.', '-type', 'f', '-maxdepth', '1', '-name', '*.cpp', '-print').stdout().strip().split('\n'),
  dependencies: [
    dependency('threads')
  ]
)
