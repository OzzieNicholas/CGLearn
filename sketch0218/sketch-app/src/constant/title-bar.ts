import About from '@/features/About'
import Settings from '@/features/Settings'
import { emit as emitGlobalAction } from '@/stores/global-actions'
import { redo, undo } from '@/stores/global-undo-redo'
import { setAppLanguage, useSettingsStore } from '@/stores/settings'
import { TitleBarBadge, TitleBarMenu } from '@/types/title-bar'
import NiceModal from '@ebay/nice-modal-react'
import { open as openUrl } from '@tauri-apps/plugin-shell'
import { channelInfo, channelName, channelUrl } from './channel'

let defaultTitleBarMenus: TitleBarMenu[] = [
  {
    label: 'common.file',
    sub_menus: [
      {
        label: 'common.new',
        shortcut: 'Ctrl+N',
        info: 'New project file',
        action: () => emitGlobalAction('new')
      },
      {
        label: 'common.open',
        shortcut: 'Ctrl+O',
        action: () => emitGlobalAction('open')
      },
      {
        label: 'menu.open-recent',
        divider: true,
        action: undefined!,
        sub_menus: useSettingsStore
          .getState()
          .recent_projects.map((url) => ({
            label: url,
            action: () => emitGlobalAction('open', url)
          }))
          .reverse()
      },
      {
        label: 'common.save',
        shortcut: 'Ctrl+S',
        action: () => emitGlobalAction('save')
      },
      {
        label: 'common.save-as',
        shortcut: 'Ctrl+Shift+S',
        divider: true,
        action: () => emitGlobalAction('saveAs')
      },
      {
        label: 'common.quit',
        shortcut: 'Ctrl+Q',
        action: () => emitGlobalAction('quit')
      }
    ]
  },
  {
    label: 'common.edit',
    sub_menus: [
      {
        label: 'common.undo',
        shortcut: 'Ctrl+Z',
        action: undo
      },
      {
        label: 'common.redo',
        shortcut: 'Ctrl+Shift+Z',
        action: redo,
        divider: true
      },
      {
        label: 'common.cut',
        shortcut: 'Ctrl+X',
        action: () => emitGlobalAction('cut')
      },
      {
        label: 'common.copy',
        shortcut: 'Ctrl+C',
        action: () => emitGlobalAction('copy')
      },
      {
        label: 'common.paste',
        shortcut: 'Ctrl+V',
        action: () => emitGlobalAction('paste')
      }
    ]
  },
  {
    label: 'common.settings',
    sub_menus: [],
    action: () => NiceModal.show(Settings)
  },
  {
    label: 'common.help',
    sub_menus: [
      {
        label: 'menu.reset-layout',
        action: () => emitGlobalAction('resetLayout')
      },
      {
        label: 'common.about',
        action: () => NiceModal.show(About)
      }
    ]
  }
]

const defaultTitleBarBadges: TitleBarBadge[] = [
  {
    label: channelName,
    color: '#FFCA28',
    info: channelInfo,
    action: () => {
      if (channelUrl) {
        // window.open(channelUrl)
        openUrl(channelUrl)
      } else {
        const language = useSettingsStore.getState().language === 'en' ? 'zh.cn' : 'en'
        setAppLanguage(language)
      }
    }
  }
]

// subscribe recent projects
useSettingsStore.subscribe(
  (state) => state.recent_projects,
  (urls) => {
    defaultTitleBarMenus[0].sub_menus[2].sub_menus = urls
      .map((url) => ({
        label: url,
        action: () => emitGlobalAction('open', url)
      }))
      .reverse()
  }
)

export { defaultTitleBarBadges, defaultTitleBarMenus }
