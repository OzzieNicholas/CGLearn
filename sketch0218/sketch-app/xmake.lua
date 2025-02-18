target('sketch-app-frontend', function()
  set_kind('phony')

  on_build(function(target)
    import('lib.detect.find_tool')
    import('core.project.config')
    local bun = find_tool('bun')
    local bunx = find_tool('bunx')

    os.cd(os.scriptdir())
    os.execv(bun.program, { 'tsc' })
    os.execv(bunx.program, { '--bun', 'vite', 'build' })
  end)
end)

target('sketch-app', function()
  set_kind('binary')

  on_build(function(target)
    import('lib.detect.find_tool')
    import('core.project.config')
    local bun = find_tool('bun')

    os.cd(os.scriptdir())
    os.setenv('XMAKE_BUILD_TARGET_DIR', path.absolute(target:targetdir()))

    if config.mode() == 'debug' then
      os.execv(bun.program, { 'run', 'tauri', 'build', '--debug' })
      os.mv(path.join(os.scriptdir(), 'src-tauri', 'target', 'debug', 'nous.exe'), path.join(target:targetdir(), 'sketch-app.exe'))
      os.mv(path.join(os.scriptdir(), 'src-tauri', 'target', 'debug', '*.pdb'), path.join(target:targetdir()))
    else
      os.execv(bun.program, { 'run', 'tauri', 'build' })
      os.mv(path.join(os.scriptdir(), 'src-tauri', 'target', 'release', 'nous.exe'), path.join(target:targetdir(), 'sketch-app.exe'))
    end
  end)

  before_run(function(target)
    os.setenv('MIMALLOC_DISABLE_REDIRECT', 1)
  end)

  add_deps('sketch', 'sketch-rs')
  add_packages('nous')
end)
