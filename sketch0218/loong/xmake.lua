add_requires('loong-engine')
add_requires('geos', { configs = { shared = true } })
add_requires('opengl')

target('sketch', function()
  set_languages('c11', 'cxx17')
  set_kind('shared')

  add_cxxflags('cl::/DUNICODE', 'cl::/utf-8')

  add_files('sketch/src/**.cpp')
  add_includedirs('sketch/include/', { public = true })
  add_includedirs('sketch/src/')

  add_defines('CORE_DLL_EXPORTS', 'NOMINMAX')
  add_packages('nous', 'loong-engine', 'geos', 'opengl')

  -- after load
  -- @see https://github.com/xmake-io/xmake/blob/dev/xmake/rules/winsdk/xmake.lua
  after_load(function(target)
    -- add links
    target:add('syslinks', 'kernel32', 'user32', 'gdi32', 'winspool', 'comdlg32', 'advapi32')
    target:add('syslinks', 'shell32', 'ole32', 'oleaut32', 'uuid', 'odbc32', 'odbccp32', 'comctl32')
    target:add('syslinks', 'comdlg32', 'setupapi', 'shlwapi')
    if not is_plat('mingw') then
      target:add('syslinks', 'strsafe')
    end

    import('core.project.config')
    local download_dir = path.join(config.buildir(), 'sketch-dependencies')
    target:set('sketch-dependencies-dir', download_dir)

    if not os.isdir(download_dir) then
      if os.exists(download_dir) then
        os.rm(download_dir)
      end
      import('script.download-config-dir', { rootdir = os.projectdir() })(download_dir)
      import('script.download-psh-blob-file', { rootdir = os.projectdir() })(download_dir)
    end
  end)

  ---@param target Target
  after_build(function(target)
    local dependencies_dir = target:get('sketch-dependencies-dir')
    if os.isdir(path.join(dependencies_dir, 'config')) then
      os.cp(path.join(dependencies_dir, 'config'), target:targetdir())
    end

    if os.isfile(path.join(dependencies_dir, 'psh.blob')) then
      os.cp(path.join(dependencies_dir, 'psh.blob'), target:targetdir())
    end
  end)

  ---@param target Target
  after_install(function(target)
    os.cp(path.join(target:pkg('loong-engine'):installdir('bin'), '**.dll'), target:installdir('bin'))
    os.cp(path.join(target:pkg('loong-engine'):installdir('bin'), '**.tx'), target:installdir('bin'))
    os.cp(path.join(target:pkg('nous'):installdir('bin'), '*.dll'), target:installdir('bin'))

    local dependencies_dir = target:get('sketch-dependencies-dir')

    if os.isdir(path.join(dependencies_dir, 'config')) then
      os.cp(path.join(dependencies_dir, 'config'), target:installdir('bin'))
    end

    if os.isfile(path.join(dependencies_dir, 'psh.blob')) then
      os.cp(path.join(dependencies_dir, 'psh.blob'), target:installdir('bin'))
    end
  end)
end)
