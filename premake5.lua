workspace "RayTracing"
    architecture "x64"
    startproject "RayTracing"

    configurations{
        "Debug", 
        "Release", 
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "Walnut/WalnutExternal.lua"
include "RayTracing"