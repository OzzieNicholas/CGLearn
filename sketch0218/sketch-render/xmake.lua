add_requires('wgpu-native main')
add_requires('glm')
add_requires('stringzilla v3.10.5')
add_requires('nous')
add_requires('stb 2024.06.01')

target('sketch-render', function()
  set_languages('c11', 'cxx17')
  set_kind('shared')

  add_cxxflags('cl::/utf-8', 'cl::/Zc:__cplusplus')
  add_includedirs('src')
  add_includedirs('include', { public = true })

  if is_plat('windows') then
    add_defines('NOMINMAX')
  end
  add_defines("SKETCH_RENDER_EXPORT")
  add_defines('GLM_FORCE_DEPTH_ZERO_TO_ONE')

  add_files("src/**.cpp")
  add_deps('sketch')

  add_packages("wgpu-native", "glm", "stringzilla", 'nous', 'loong-engine')

  after_build(function(target)
    os.cp(path.join(os.scriptdir(), 'shaders'), target:targetdir())
  end)

  after_install(function(target)
    os.cp(path.join(os.scriptdir(), 'shaders'), target:installdir('bin'))
  end)
end)
