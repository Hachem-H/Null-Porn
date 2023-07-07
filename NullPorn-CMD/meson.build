nullporn_cmd = executable('NullPorn-CMD',
    sources: [
        'src/Application.c'
    ],

    link_with: nullporn_core,
    dependencies: nullporn_core_dependencies,

    include_directories: [
        include_directories('src'),
        nullporn_core_includes,
        vendor_includes,
    ]
)