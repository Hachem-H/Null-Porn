workspace "NullPorn"
    configurations { "Debug", "Release", "Distribution" }
    architecture "x86_64"
    systemversion "latest"

    filter "configurations:Debug"
        defines "NP_DEBUG"
        optimize "Off"
        symbols "On"
    filter "configurations:Release"
        defines "NP_RELEASE"
        optimize "On"
        symbols "On"
    filter "configurations:Distribution"
        defines "NP_DIST"
        optimize "On"
        symbols "Off"

project "NullPorn-Core"
    location "%{wks.location}/%{prj.name}"
    kind "StaticLib"
    language "C"

    targetdir  "%{wks.location}/bin/"
    objdir "%{wks.location}/bin-int/%{prj.name}-%{cfg.buildcfg}/"

    files { "%{prj.location}/src/**.c",
            "%{prj.location}/src/**.h" }
    includedirs { "%{prj.location}/src" }
    
project "NullPorn-GUI"
    location "%{wks.location}/%{prj.name}"
    kind "ConsoleApp"
    language "C"

    targetdir  "%{wks.location}/bin/"
    objdir "%{wks.location}/bin-int/%{prj.name}-%{cfg.buildcfg}/"

    files { "%{prj.location}/src/**.c",
            "%{prj.location}/src/**.h" }
    includedirs { "%{prj.location}/src",

                  "%{wks.location}/NullPorn-Core/src" }
    links { "NullPorn-Core", "curl" }