nullporn_gui = executable('NullPorn-GUI',
    sources: [
        'src/Application.c',
        'src/ApplicationState.c',
    ],
    
    link_args: ['-lm'],
    link_with: nullporn_core,
    dependencies: [
        nullporn_core_dependencies,
        dependency('raylib'),
    ],

    include_directories: [
        include_directories('src'),
        nullporn_core_includes,
        vendor_includes,
    ]
)