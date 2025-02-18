target('sketch-rs', function()
  set_kind('shared')
  add_files('src/**.cpp')
  set_languages("c11", "cxx17")

  add_defines("SKETCH_RS_EXPORT")

  add_deps('sketch', 'sketch-render')
  add_packages('loong-engine', 'nous', 'wgpu-native', 'stringzilla')
  add_includedirs('include/')
end)
