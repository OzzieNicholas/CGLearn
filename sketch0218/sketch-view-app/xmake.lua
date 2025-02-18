add_requires('glfw')

target('sketch-view-app', function()
  set_languages('c11', 'cxx17')

  add_cxxflags('cl::/utf-8', 'cl::/Zc:__cplusplus')

  set_kind('binary')
  add_files('src/**.cpp')
  add_includedirs('src')

  if is_plat('windows') then
    add_defines('NOMINMAX')
  end

  add_deps('sketch', 'sketch-render')
  add_packages('loong-engine', 'nous', 'wgpu-native', 'stringzilla')
  add_packages('glfw', 'stb')

  before_run(function(target)
    os.setenv('MIMALLOC_DISABLE_REDIRECT', 1)
  end)
end)
