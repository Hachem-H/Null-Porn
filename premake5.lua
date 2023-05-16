workspace "NullPorn"
    configurations { "Debug", "Release", "Distribution" }
    architecture "x86_64"
    startproject "NullPorn-Gui"
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

group "Dependencies"
    project "glfw"
        location "%{wks.location}/NullPorn-GUI/Vendor/%{prj.name}"
        staticruntime "On"
        kind "StaticLib"
        language "C"
    
        targetdir  "%{wks.location}/bin/%{prj.name}"
        objdir "%{wks.location}/bin-int/%{prj.name}-%{cfg.buildcfg}/"
    
        files { "%{prj.location}/src/glfw_config.h",
    	        "%{prj.location}/src/context.c",
    	        "%{prj.location}/src/init.c",
    	        "%{prj.location}/src/input.c",
    	        "%{prj.location}/src/monitor.c",
    	        "%{prj.location}/src/null_init.c",
    	        "%{prj.location}/src/null_joystick.c",
    	        "%{prj.location}/src/null_monitor.c",
    	        "%{prj.location}/src/null_window.c",
    	        "%{prj.location}/src/platform.c",
    	        "%{prj.location}/src/vulkan.c",
    	        "%{prj.location}/src/window.c", }
        includedirs "%{prj.location}/include/"
        
        filter "system:linux"
            pic "On"
            defines "_GLFW_X11"
    
            files { "%{prj.location}/src/x11_init.c",
    		        "%{prj.location}/src/x11_monitor.c",
    		        "%{prj.location}/src/x11_window.c",
    		        "%{prj.location}/src/xkb_unicode.c",
    		        "%{prj.location}/src/posix_time.c",
    		        "%{prj.location}/src/posix_poll.c",
    		        "%{prj.location}/src/posix_thread.c",
    		        "%{prj.location}/src/posix_module.c",
    		        "%{prj.location}/src/glx_context.c",
    		        "%{prj.location}/src/egl_context.c",
    		        "%{prj.location}/src/osmesa_context.c",
    		        "%{prj.location}/src/linux_joystick.c" }

    project "glew"
        location "%{wks.location}/NullPorn-GUI/Vendor/%{prj.name}"
        staticruntime "On"
        kind "StaticLib"
        language "C"

        targetdir  "%{wks.location}/bin/%{prj.name}"
        objdir "%{wks.location}/bin-int/%{prj.name}-%{cfg.buildcfg}/"

        files { "%{prj.location}/src/**.c" }
        includedirs "%{prj.location}/include/"
group ""
        
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
				  "%{wks.location}/NullPorn-Core/src",
                  "%{prj.location}/Vendor/glfw/include",
                  "%{prj.location}/Vendor/glew/include",
                  "%{prj.location}/Vendor/Nuklear",  }
	links { "NullPorn-Core", "glfw", "glew", "GL", "m" }

project "NullPorn-CLI"
    location "%{wks.location}/%{prj.name}"
    kind "ConsoleApp"
    language "C"

    targetdir  "%{wks.location}/bin/"
    objdir "%{wks.location}/bin-int/%{prj.name}-%{cfg.buildcfg}/"

    files { "%{prj.location}/src/**.c",
            "%{prj.location}/src/**.h" }
    includedirs { "%{prj.location}/src",
                  "%{wks.location}/NullPorn-Core/src" }
    links { "NullPorn-Core" }
