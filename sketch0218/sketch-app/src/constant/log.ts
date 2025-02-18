import { LogLevel } from '@/commands/definer'

export const logColor: Record<LogLevel, string> = {
  [LogLevel.Trace]: '#E0E0E0',
  [LogLevel.Debug]: '#E0E0E0',
  [LogLevel.Info]: '#E0E0E0',
  [LogLevel.Warn]: '#FFF176',
  [LogLevel.Error]: '#FF1744',
  [LogLevel.Critical]: '#FF1744',
  [LogLevel.Off]: '#E0E0E0'
}

export const logName: Record<LogLevel, string> = {
  [LogLevel.Trace]: 'Trace',
  [LogLevel.Debug]: 'Debug',
  [LogLevel.Info]: 'Info',
  [LogLevel.Warn]: 'Warning',
  [LogLevel.Error]: 'Error',
  [LogLevel.Critical]: 'Critical',
  [LogLevel.Off]: 'Off'
}
