export type Command = {
  label: string
  action?: () => void | Promise<void>
  info?: string
  shortcut?: string
}
