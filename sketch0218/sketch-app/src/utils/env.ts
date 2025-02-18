import {
  Arch,
  OsType,
  arch as arch_,
  locale as locale_,
  type as type_,
  version as version_
} from '@tauri-apps/plugin-os'

type EnvInfo = {
  os: OsType
  os_version: string
  arch: Arch
  locale: string | null
}

let envInfo: EnvInfo = null!

export const setupEnv = async () => {
  envInfo = {
    os: await type_(),
    os_version: await version_(),
    arch: await arch_(),
    locale: await locale_()
  }
  return envInfo
}

export const getEnv = (): EnvInfo => {
  return envInfo
}

export function isMacOS() {
  return getEnv()?.os === 'macos'
}

export function isWindows() {
  return getEnv()?.os === 'windows'
}

export function isLinux() {
  return getEnv()?.os === 'linux'
}
