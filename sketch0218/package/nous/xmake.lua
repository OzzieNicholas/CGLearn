package('nous', function()
  add_urls('ftp://admin:111111@build-server.d5next.com:41657/build/sketch/dependency/nous.zip')
  add_versions('latest', '529beefd8eb2ca3eb16786a040266ee88360ea001253824b0bb0f7720a6dec93')

  on_load(function(package)
    import('core.project.config')
    package:set('installdir', path.absolute(path.join(config.buildir(), '.packages', 'n', 'nous')))
  end)

  add_defines('FMT_HEADER_ONLY') -- fmt in nous is headeronly

  on_install('windows', function(package)
    os.cp('bin/Release/*', package:installdir('bin'))
    os.cp('include/*', package:installdir('include'))
    os.cp('lib/Release/*', package:installdir('lib'))
  end)
end)
