nullporn_core_includes = include_directories('src')
nullporn_core_dependencies = dependency('libcurl')

nullporn_core = library('NullPorn-Core',
    sources: [
        'src/DDoS.c',
        'src/Networking.c',
        'src/Utils.c',
    ],

    dependencies: nullporn_core_dependencies,

    include_directories: [
        nullporn_core_includes,
        vendor_includes
    ]
)