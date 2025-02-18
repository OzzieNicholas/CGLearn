import ColorPicker from '@/components/ColorPicker'
import {
  Dialog,
  DialogButton,
  DialogContent,
  DialogFooter,
  DialogDescription,
  DialogTitle
} from '@/components/Dialog'
import Select from '@/components/Select'
import { Slider, SliderValueType } from '@/components/Slider'
import Toggle from '@/components/Toggle'
import { EdgeStyle } from '@/types/node-graph'
import { resources as i18nResources, useI18n } from '@/locales'
import {
  SettingsState,
  setAppLanguage,
  useSettingsStore,
  useSettingsStoreApi
} from '@/stores/settings'
import { cn } from '@/utils/cn'
import NiceModal, { useModal } from '@ebay/nice-modal-react'
import { Accordion, AccordionItem, Tab, Tabs } from '@nextui-org/react'
import { BackgroundVariant, SelectionMode } from '@xyflow/react'
import { produce } from 'immer'
import React, { useCallback } from 'react'

function LabeledItem({
  children,
  label,
  className
}: {
  children: React.ReactNode
  label: string
  className?: string
}) {
  const { t } = useI18n()

  return (
    <div className={cn('grid grid-cols-2 items-center gap-4', className)}>
      <div className="text-right align-middle tracking-wide">{t(label as any)}</div>
      <div className="col-span-1 self-center">{children}</div>
    </div>
  )
}

const selectClassname = 'rounded-md border-1 border-white/20 bg-menu-bg w-full h-6 min-h-0'

function Interface() {
  const { t } = useI18n()
  const storeApi = useSettingsStoreApi()
  return (
    <Accordion
      selectionMode="multiple"
      defaultExpandedKeys={['Translation', 'Editor']}
      disableAnimation
      itemClasses={{
        title: 'text-text-primary'
      }}
    >
      <AccordionItem
        key="Translation"
        aria-label={t('settings.translation')}
        title={t('settings.translation')}
      >
        <div className="flex flex-col gap-2">
          <LabeledItem label="settings.language">
            <Select
              className={selectClassname}
              onChange={(lang) => {
                setAppLanguage(lang)
              }}
              defaultValue={storeApi.getState().language}
              options={Object.keys(i18nResources).map((lang) => ({
                value: lang,
                label: t(`settings.languages.${lang}` as any)
              }))}
            />
          </LabeledItem>
        </div>
      </AccordionItem>
      <AccordionItem key="Editor" aria-label={t('settings.editor')} title={t('settings.editor')}>
        <div className="flex flex-col gap-2">
          <LabeledItem label="settings.undo-history-max-size">
            <Slider
              valueType={SliderValueType.Int}
              defaulValue={storeApi.getState().undo_history_max_size}
              rangeMin={1}
              rangeMax={200}
              className="h-full rounded-lg bg-menu-bg/20 align-middle text-sm"
              onValueChange={(v: number) => {
                if (v > 0) {
                  storeApi.setState(() => ({ undo_history_max_size: v }))
                }
              }}
            />
          </LabeledItem>
        </div>
      </AccordionItem>
    </Accordion>
  )
}

const settingsStoreSelector = (state: SettingsState) => ({
  backgroundColor: state.background_color,
  nodeGraphBackgroundColor: state.node_graph.background_color
})

function Appearance() {
  const { backgroundColor, nodeGraphBackgroundColor } = useSettingsStore(settingsStoreSelector)
  const { t } = useI18n()
  const storeApi = useSettingsStoreApi()

  const applyChange = useCallback((key: string, result: any) => {
    if (key === 'settings.background-color') {
      storeApi.setState(
        produce((state) => {
          state.background_color = result.css || result.hexa
        })
      )
    } else if (key === 'settings.node-graph.background-color') {
      storeApi.setState(
        produce((state) => {
          state.node_graph.background_color = result.css || result.hexa
        })
      )
    }
  }, [])

  const colors = [
    ['settings.background-color', backgroundColor],
    ['settings.node-graph.background-color', nodeGraphBackgroundColor]
  ]

  return (
    <Accordion
      selectionMode="multiple"
      defaultExpandedKeys={['Color']}
      disableAnimation
      itemClasses={{
        title: 'text-text-primary'
      }}
    >
      <AccordionItem key="Color" aria-label={t('common.color')} title={t('common.color')}>
        <div className="flex w-full flex-col gap-2">
          {colors.map((item) => {
            return (
              <LabeledItem key={item[0]} label={item[0]} className="h-6">
                <ColorPicker
                  placement="left-start"
                  color={item[1]}
                  onChange={(c) => {
                    applyChange(item[0], c)
                  }}
                  useGradientControls
                >
                  <button
                    className="btn btn-circle btn-ghost m-0 h-6 min-h-0 w-[90%] border-1 border-white/20 p-0"
                    style={{
                      background: item[1]
                    }}
                  />
                </ColorPicker>
              </LabeledItem>
            )
          })}
        </div>
      </AccordionItem>
    </Accordion>
  )
}

function Editing() {
  const { t } = useI18n()
  const storeApi = useSettingsStoreApi()

  return (
    <Accordion
      selectionMode="multiple"
      defaultExpandedKeys={['NodeGraph']}
      disableAnimation
      itemClasses={{
        title: 'text-text-primary'
      }}
    >
      <AccordionItem
        key="NodeGraph"
        aria-label={t('window.node-graph')}
        title={t('window.node-graph')}
      >
        <div className="flex flex-col gap-2">
          <LabeledItem label={t('settings.node-graph.parallel-execution-nodes' as any)}>
            <div className="flex justify-items-center">
              <Toggle
                className="checkbox-accent size-4"
                defaultChecked={storeApi.getState().node_graph.parallel_execution_nodes}
                onChange={(checked) => {
                  storeApi.setState(
                    produce((state) => {
                      state.node_graph.parallel_execution_nodes = checked
                    })
                  )
                }}
              />
            </div>
          </LabeledItem>
          <LabeledItem label={t('settings.node-graph.snap-to-grid' as any)}>
            <div className="flex justify-items-center">
              <Toggle
                className="checkbox-accent size-4"
                defaultChecked={storeApi.getState().node_graph.snap_to_grid}
                onChange={(checked) => {
                  storeApi.setState(
                    produce((state) => {
                      state.node_graph.snap_to_grid = checked
                    })
                  )
                }}
              />
            </div>
          </LabeledItem>
          <LabeledItem label={t('settings.node-graph.selection-mode.key' as any)}>
            <Select
              className={selectClassname}
              onChange={(value) => {
                let selection_mode: SelectionMode = SelectionMode.Partial
                switch (value) {
                  case 'partial': {
                    selection_mode = SelectionMode.Partial
                    break
                  }
                  case 'full': {
                    selection_mode = SelectionMode.Full
                    break
                  }
                }
                storeApi.setState(
                  produce((state) => {
                    state.node_graph.selection_mode = selection_mode
                  })
                )
              }}
              defaultValue={storeApi.getState().node_graph.selection_mode}
              options={[
                {
                  value: 'partial',
                  label: t('settings.node-graph.selection-mode.partial' as any)
                },
                {
                  value: 'full',
                  label: t('settings.node-graph.selection-mode.full' as any)
                }
              ]}
            />
          </LabeledItem>
          <LabeledItem label={t('settings.node-graph.edge-style.key' as any)}>
            <Select
              className={selectClassname}
              onChange={(value) => {
                storeApi.setState(
                  produce((state) => {
                    state.node_graph.edge_style = value as EdgeStyle
                  })
                )
              }}
              defaultValue={storeApi.getState().node_graph.edge_style}
              options={[
                {
                  value: EdgeStyle.Straight,
                  label: t('settings.node-graph.edge-style.straight' as any)
                },
                {
                  value: EdgeStyle.Bezier,
                  label: t('settings.node-graph.edge-style.bezier' as any)
                },
                {
                  value: EdgeStyle.Step,
                  label: t('settings.node-graph.edge-style.step' as any)
                },
                {
                  value: EdgeStyle.SmoothStep,
                  label: t('settings.node-graph.edge-style.smooth-step' as any)
                }
              ]}
            />
          </LabeledItem>
          <LabeledItem label={t('settings.node-graph.background.key' as any)}>
            <Select
              className={selectClassname}
              onChange={(value) => {
                let background: BackgroundVariant | undefined = BackgroundVariant.Dots
                switch (value) {
                  case 'lines': {
                    background = BackgroundVariant.Lines
                    break
                  }
                  case 'dots': {
                    background = BackgroundVariant.Dots
                    break
                  }
                  case 'cross': {
                    background = BackgroundVariant.Cross
                    break
                  }
                  case 'none': {
                    background = undefined
                    break
                  }
                }
                storeApi.setState(
                  produce((state) => {
                    state.node_graph.background = background
                  })
                )
              }}
              defaultValue={storeApi.getState().node_graph.background}
              options={[
                {
                  value: 'lines',
                  label: t('settings.node-graph.background.lines' as any)
                },
                {
                  value: 'dots',
                  label: t('settings.node-graph.background.dots' as any)
                },
                {
                  value: 'cross',
                  label: t('settings.node-graph.background.cross' as any)
                },
                {
                  value: 'none',
                  label: t('settings.node-graph.background.none' as any)
                }
              ]}
            />
          </LabeledItem>
        </div>
      </AccordionItem>
    </Accordion>
  )
}

function Settings() {
  const modal = useModal()
  const { t } = useI18n()

  return (
    <Dialog open modal={false}>
      <DialogTitle />
      <DialogContent disableOverlay>
        <DialogDescription />
        <div className="flex min-h-60 w-full flex-col">
          <Tabs
            aria-label="Settings"
            variant="solid"
            radius="md"
            classNames={{
              tabList: 'bg-white/5',
              cursor: 'w-full bg-btn-active/60',
              tabContent: 'text-text-primary/80 select-none'
            }}
            color="secondary"
          >
            <Tab
              key="Interface"
              title={
                <div className="flex items-center space-x-2">
                  <span>{t('settings.interface')}</span>
                </div>
              }
            >
              <Interface />
            </Tab>

            <Tab
              key="Appearance"
              title={
                <div className="flex items-center space-x-2">
                  <span>{t('settings.appearance')}</span>
                </div>
              }
            >
              <Appearance />
            </Tab>

            <Tab
              key="Editing"
              title={
                <div className="flex items-center space-x-2">
                  <span>{t('settings.editing')}</span>
                </div>
              }
            >
              <Editing />
            </Tab>
          </Tabs>
        </div>

        <DialogFooter>
          <DialogButton
            onClick={() => {
              modal.remove()
            }}
          >
            {t('common.ok')}
          </DialogButton>
        </DialogFooter>
      </DialogContent>
    </Dialog>
  )
}

export default NiceModal.create(Settings)
