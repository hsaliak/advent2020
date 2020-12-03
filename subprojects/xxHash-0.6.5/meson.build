project('xxhash', 'c',
    # library files are BSD licensed. xxhsum utility is GPL licensed.
    license : [ 'BSD', 'GPL'],
    version : '0.6.5',
    default_options: ['buildtype=debugoptimized'])

inc = include_directories('.')

libxxhash = library('xxhash',
    ['xxhash.c'],
    include_directories : inc,
    version : meson.project_version(),
    install : true)

xxhash_dep = declare_dependency(
    include_directories : inc,
    link_with : libxxhash)

xxhsum = executable('xxhsum',
    ['xxhash.c', 'xxhsum.c'],
    include_directories : inc,
    install : true)

install_headers('xxhash.h')
install_man('xxhsum.1')

test('internal bench', xxhsum,
    args : ['-bi1'])
