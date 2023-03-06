-- premake5.lua
workspace "WalnutRaytracer"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "WalnutRaytracer"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
include "Walnut/WalnutExternal.lua"

include "WalnutRaytracer"