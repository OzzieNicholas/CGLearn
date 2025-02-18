package('wgpu-native', function()
  set_homepage('https://github.com/45degree/wgpu-native')
  set_description('Native WebGPU implementation based on wgpu-core')
  set_license('MIT')

  add_urls('https://github.com/gfx-rs/wgpu-native.git')
  add_versions('main', '1f0155b317c81117d8414e492f0690db093a7833')

  on_load(function(package)
    import('core.project.config')
    package:set('installdir', path.absolute(path.join(config.buildir(), '.packages', 'w', 'wgpu-native')))
  end)

  on_install('windows', function(package)
    import('lib.detect.find_tool')
    local ret = find_tool('cargo')
    if ret then
      os.execv(ret.program, { 'build', '--release' })
      os.cp('target/release/wgpu_native.dll', package:installdir('bin'))
      os.cp('target/release/wgpu_native.dll.lib', package:installdir('lib'))
      os.cp('ffi/wgpu.h', package:installdir('include'))
      os.cp('ffi/webgpu-headers/webgpu.h', package:installdir('include'))
    end
  end)

  on_test(function(package)
    assert(package:has_cfuncs('wgpuCreateInstance', { includes = { 'wgpu.h', 'webgpu.h' } }))
  end)
end)
