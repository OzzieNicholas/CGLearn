import('net.http')
import('utils.archive')

function main(dir)
  local externals = 'ftp://admin:111111@build-server.d5next.com:41657/build/sketch/dependency/externals.zip'
  local tmpdir = os.tmpdir()
  local tmp_download_external_file = path.join(tmpdir, 'externals.zip')
  if not os.isdir(tmp_download_external_file) then
    os.tryrm(tmp_download_external_file)
    http.download(externals, tmp_download_external_file)
  end
  archive.extract(tmp_download_external_file, tmpdir)
  os.cp(path.join(tmpdir, 'externals', 'loong_misc', 'bin', 'Release', 'psh.blob'), path.join(dir, 'psh.blob'))
end
