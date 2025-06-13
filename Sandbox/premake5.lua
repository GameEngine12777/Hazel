project "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
		"assets/**.glsl",
	}

	includedirs
	{
		"%{wks.location}/Hazel/vendor/spdlog/include",
		"%{wks.location}/Hazel/src",
		"%{wks.location}/Hazel/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
	}

	links
	{
		"Hazel"
	}

	filter "system:windows"
		systemversion "latest"

		-- 构建成功后自动拷贝 VulkanSDK 相关的 dll 文件到当前项目的构建目标路径
		postbuildcommands
        {
            "{COPYDIR} \"%{LibraryDir.VulkanSDK_DLL}\" \"%{cfg.targetdir}\""
        }

	filter "configurations:Debug"
		defines "HZ_DEBUG"
		buildoptions "/utf-8"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "HZ_RELEASE"
		buildoptions "/utf-8"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "HZ_DIST"
		buildoptions "/utf-8"
		runtime "Release"
		optimize "on"
