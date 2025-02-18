import('net.http')
import('utils.archive')

function main(dir)
  local url = 'ftp://admin:111111@build-server.d5next.com:41657/build/sketch/dependency/config.zip'
  local config_file_path = path.join(dir, 'config')

  if not os.isdir(config_file_path) then
    os.tryrm(config_file_path)
    http.download(url, path.join(dir, 'config.zip'))
  end
  archive.extract(path.join(dir, 'config.zip'), dir)
end
