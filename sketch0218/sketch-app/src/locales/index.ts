import { useSettingsStore } from '@/stores/settings'
import i18n, { KeyPrefix } from 'i18next'
import { initReactI18next, useTranslation, UseTranslationResponse } from 'react-i18next'

import en from './en'
import zh_cn from './zh.cn'

// load translations
export const resources = {
  'zh.cn': {
    translation: zh_cn
  },
  en: {
    translation: en
  }
}

// setup i18n
i18n
  // pass the i18n instance to react-i18next.
  .use(initReactI18next)
  // init i18n
  // for all options read: https://www.i18next.com/overview/configuration-options
  .init({
    resources,
    lng: useSettingsStore.getState().language,
    fallbackLng: 'en',
    nsSeparator: false,
    interpolation: {
      escapeValue: false // not needed for react as it escapes by default
    }
  })

// subscribe settings store
useSettingsStore.subscribe(
  (state) => state.language,
  (language) => i18n.changeLanguage(language)
)

// i18n.on('languageChanged', (language) => {
//   useSettingsStore.setState({ language })
// })

export function useI18n<KPrefix extends KeyPrefix<'translation'> = undefined>(
  prefix?: KPrefix
): UseTranslationResponse<'translation', KPrefix> {
  return prefix ? useTranslation('translation', { keyPrefix: prefix }) : useTranslation()
}

export default i18n
