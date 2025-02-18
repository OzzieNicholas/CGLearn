import {
  disableLightForPolygon,
  forceShowPointsByDefaultColor,
  // initViewport,
  alwaysShowIsolatePoint as alwaysShowPoint,
  viewportFitView,
  viewportSetIndexSize,
  viewportSetPointColor,
  viewportSetPointIndexColor,
  viewportSetPointNormalColor,
  viewportSetPointNormalLength,
  viewportSetPointSize,
  viewportSetPrimIndexColor,
  viewportSetPrimNormalColor,
  viewportSetPrimNormalLength,
  viewportSetWireframeColor,
  viewportShowPointIndex,
  viewportShowPointNormal,
  viewportShowPrimIndex,
  viewportShowPrimNormal,
  viewportShowWireframe,
  viewportSetBackgroundColor,
  viewportSetNearPlane,
  viewportSetFarPlane,
} from '@/commands'
import { Color3Params, Color4Params } from '@/commands/index.types'
import ColorPicker from '@/components/ColorPicker'
import { Slider, SliderValueType } from '@/components/Slider'
import Tooltip from '@/components/Tooltip'
import { presets } from '@/css/presets'
import { useI18n } from '@/locales'
import { SettingsState, useSettingsStore, useSettingsStoreApi } from '@/stores/settings'
import { cn } from '@/utils/cn'
import { colorResultToRgbaf, colorResultToRgbf, rgbafToHexa } from '@/utils/functions'
import { Popover, PopoverContent, PopoverTrigger } from '@nextui-org/react'
import { Mutex } from 'async-mutex'
import { produce } from 'immer'
import { useCallback } from 'react'
import { TbSettingsBolt } from 'react-icons/tb'
import { BsLightbulbOff } from 'react-icons/bs'
import { VscActivateBreakpoints } from 'react-icons/vsc'
import { useEffectOnce } from 'react-use'
import Toggle from '@/components/Toggle'

async function syncSettings(state: SettingsState) {
  // await initViewport()
  viewportShowWireframe([state.viewport.show_wireframe])
  viewportShowPointIndex([state.viewport.show_point_index])
  viewportShowPointNormal([state.viewport.show_prim_index])
  viewportShowPrimIndex([state.viewport.show_point_normal])
  viewportShowPrimNormal([state.viewport.show_prim_normal])
  viewportSetIndexSize([state.viewport.index_size])
  viewportSetPointNormalLength([state.viewport.point_normal_length])
  viewportSetPrimNormalLength([state.viewport.prim_normal_length])
  viewportSetPointSize([state.viewport.point_size])
  viewportSetPointColor(state.viewport.point_color as Color4Params)
  viewportSetWireframeColor(state.viewport.wireframe_color as Color4Params)
  viewportSetPointIndexColor(state.viewport.point_index_color as Color4Params)
  viewportSetPrimIndexColor(state.viewport.prim_index_color as Color4Params)
  viewportSetPointNormalColor(state.viewport.point_normal_color as Color3Params)
  viewportSetPrimNormalColor(state.viewport.prim_normal_color as Color3Params)
  viewportSetBackgroundColor(state.viewport.background_color as Color3Params)
  viewportSetNearPlane([state.viewport.near_plane])
  viewportSetFarPlane([state.viewport.far_plane])
}

const initMutex = new Mutex()
let hasInit = false

const btnClassName = 'btn btn-square btn-ghost size-7 min-h-0 rounded brightness-125'
const btnActive = 'bg-btn-active/50'

const settingsStoreSelector = (state: SettingsState) => ({
  showWireframe: state.viewport.show_wireframe,
  showPointIndex: state.viewport.show_point_index,
  showPrimIndex: state.viewport.show_prim_index,
  showPointNormal: state.viewport.show_point_normal,
  showPrimNormal: state.viewport.show_prim_normal,
  disableLight: state.viewport.disable_light,
  showIsolatePoint: state.viewport.show_isolate_point
})

export function ViewportToolBar({ className }: { className?: string }) {
  const {
    showWireframe,
    showPointIndex,
    showPrimIndex,
    showPointNormal,
    showPrimNormal,
    disableLight,
    showIsolatePoint
  } = useSettingsStore(settingsStoreSelector)
  const settingsApi = useSettingsStoreApi()

  useEffectOnce(() => {
    if (!hasInit && !initMutex.isLocked()) {
      initMutex.runExclusive(() => {
        hasInit = true
        syncSettings(settingsApi.getState())
      })
    }
    return useSettingsStore.persist.onFinishHydration(syncSettings)
  })

  return (
    <div className={cn('flex flex-col gap-1', className)}>
      {/* fitview */}
      <Tooltip placement="left" content="tooltip.viewport.fitview">
        <div className={btnClassName} onClick={viewportFitView}>
          <img src="/fitview.png" className="p-1 brightness-125" />
        </div>
      </Tooltip>

      {/* disable light for polygon */}
      <Tooltip placement="left" content="tooltip.viewport.toggle-light-for-polygon">
        <div
          className={cn(btnClassName, disableLight ? btnActive : '')}
          onClick={() => {
            disableLightForPolygon([!disableLight])
            settingsApi.setState(
              produce((state) => {
                state.viewport.disable_light = !disableLight
              })
            )
          }}
        >
          <BsLightbulbOff className="p-0 brightness-125" />
        </div>
      </Tooltip>

      {/* toggle wireframe */}
      <Tooltip placement="left" content="tooltip.viewport.toggle-wireframe">
        <div
          className={cn(btnClassName, showWireframe ? btnActive : '')}
          onClick={() => {
            viewportShowWireframe([!showWireframe])
            settingsApi.setState(
              produce((state) => {
                state.viewport.show_wireframe = !showWireframe
              })
            )
          }}
        >
          <img
            src="/wireframe.png"
            className={cn('p-1', showWireframe ? 'brightness-200' : 'brightness-125')}
          />
        </div>
      </Tooltip>

      {/* toggle point index */}
      <Tooltip placement="left" content="tooltip.viewport.toggle-point-index">
        <div
          className={cn(btnClassName, showPointIndex ? btnActive : '')}
          onClick={() => {
            viewportShowPointIndex([!showPointIndex])
            settingsApi.setState(
              produce((state) => {
                state.viewport.show_point_index = !showPointIndex
              })
            )
          }}
        >
          <img
            src="/point-index.png"
            className={showPointIndex ? 'brightness-150' : 'brightness-125'}
          />
        </div>
      </Tooltip>

      {/* always show point */}
      <Tooltip placement="left" content="tooltip.viewport.toggle-show-point">
        <div
          className={cn(btnClassName, showIsolatePoint ? btnActive : '')}
          onClick={() => {
            alwaysShowPoint([!showIsolatePoint])
            settingsApi.setState(
              produce((state) => {
                state.viewport.show_isolate_point = !showIsolatePoint
              })
            )
          }}
        >
          <VscActivateBreakpoints className="p-0 brightness-125" />
        </div>
      </Tooltip>

      {/* toggle prim index */}
      <Tooltip placement="left" content="tooltip.viewport.toggle-prim-index">
        <div
          className={cn(btnClassName, showPrimIndex ? btnActive : '')}
          onClick={() => {
            viewportShowPrimIndex([!showPrimIndex])
            settingsApi.setState(
              produce((state) => {
                state.viewport.show_prim_index = !showPrimIndex
              })
            )
          }}
        >
          <img
            src="/prim-index.png"
            className={showPrimIndex ? 'brightness-150' : 'brightness-125'}
          />
        </div>
      </Tooltip>

      {/* toggle point normal */}
      <Tooltip placement="left" content="tooltip.viewport.toggle-point-normal">
        <div
          className={cn(btnClassName, showPointNormal ? btnActive : '')}
          onClick={() => {
            viewportShowPointNormal([!showPointNormal])
            settingsApi.setState(
              produce((state) => {
                state.viewport.show_point_normal = !showPointNormal
              })
            )
          }}
        >
          <img
            src="/point-normal.png"
            className={showPointNormal ? 'brightness-150' : 'brightness-125'}
          />
        </div>
      </Tooltip>

      {/* toggle prim normal */}
      <Tooltip placement="left" content="tooltip.viewport.toggle-prim-normal">
        <div
          className={cn(btnClassName, showPrimNormal ? btnActive : '')}
          onClick={() => {
            viewportShowPrimNormal([!showPrimNormal])
            settingsApi.setState(
              produce((state) => {
                state.viewport.show_prim_normal = !showPrimNormal
              })
            )
          }}
        >
          <img
            src="/prim-normal.png"
            className={showPrimNormal ? 'brightness-150' : 'brightness-125'}
          />
        </div>
      </Tooltip>

      {/* settings */}
      <Popover offset={10} placement="left" backdrop="transparent">
        <PopoverTrigger>
          <button className={cn(btnClassName, 'brightness-75 outline-none')}>
            {/* <Tooltip placement="left" content="tooltip.viewport.more-settings"> */}
            <TbSettingsBolt className="size-full p-0.5" />
          </button>
        </PopoverTrigger>
        <ViewportSettings />
      </Popover>
    </div>
  )
}

const settingsStoreSelector2 = (state: SettingsState) => ({
  wireframeColor: rgbafToHexa(state.viewport.wireframe_color),
  pointSize: state.viewport.point_size,
  pointColor: rgbafToHexa(state.viewport.point_color),
  indexSize: state.viewport.index_size,
  pointIndexColor: rgbafToHexa(state.viewport.point_index_color),
  pointNormalColor: rgbafToHexa(state.viewport.point_normal_color),
  pointNormalLength: state.viewport.point_normal_length,
  primIndexColor: rgbafToHexa(state.viewport.prim_index_color),
  primNormalColor: rgbafToHexa(state.viewport.prim_normal_color),
  primNormalLength: state.viewport.prim_normal_length,
  disableLight: state.viewport.disable_light,
  backgroundColor: rgbafToHexa(state.viewport.background_color),
  nearPlane: state.viewport.near_plane,
  farPlane: state.viewport.far_plane
})

function ViewportSettings() {
  const {
    wireframeColor,
    pointSize,
    pointColor,
    indexSize,
    pointIndexColor,
    pointNormalColor,
    pointNormalLength,
    primIndexColor,
    primNormalColor,
    primNormalLength,
    backgroundColor,
    nearPlane,
    farPlane
  } = useSettingsStore(settingsStoreSelector2)
  const settingsApi = useSettingsStoreApi()
  const { t } = useI18n('viewport')

  const applyChange = useCallback((key: string, data: any) => {
    if (key === 'background-color') {
      const color = colorResultToRgbaf(data)
      settingsApi.setState(
        produce((state) => {
          state.viewport.background_color = color
        })
      )
      viewportSetBackgroundColor(color as Color4Params)
    } else if (key === 'wireframe-color') {
      const color = colorResultToRgbaf(data)
      settingsApi.setState(
        produce((state) => {
          state.viewport.wireframe_color = color
        })
      )
      viewportSetWireframeColor(color as Color4Params)
    } else if (key === 'point-color') {
      const color = colorResultToRgbf(data)
      settingsApi.setState(
        produce((state) => {
          state.viewport.point_color = color
        })
      )
      viewportSetPointColor(color as Color4Params)
    } else if (key === 'point-index-color') {
      const color = colorResultToRgbaf(data)
      settingsApi.setState(
        produce((state) => {
          state.viewport.point_index_color = color
        })
      )
      viewportSetPointIndexColor(color as Color4Params)
    } else if (key === 'point-normal-color') {
      const color = colorResultToRgbf(data)
      settingsApi.setState(
        produce((state) => {
          state.viewport.point_normal_color = color
        })
      )
      viewportSetPointNormalColor(color as Color3Params)
    } else if (key === 'prim-index-color') {
      const color = colorResultToRgbaf(data)
      settingsApi.setState(
        produce((state) => {
          state.viewport.prim_index_color = color
        })
      )
      viewportSetPrimIndexColor(color as Color3Params)
    } else if (key === 'prim-normal-color') {
      const color = colorResultToRgbf(data)
      settingsApi.setState(
        produce((state) => {
          state.viewport.prim_normal_color = color
        })
      )
      viewportSetPrimNormalColor(color as Color3Params)
    } else if (key === 'point-size') {
      settingsApi.setState(
        produce((state) => {
          state.viewport.point_size = data
        })
      )
      viewportSetPointSize([data])
    } else if (key === 'index-size') {
      settingsApi.setState(
        produce((state) => {
          state.viewport.index_size = data
        })
      )
      viewportSetIndexSize([data])
    } else if (key === 'point-normal-length') {
      settingsApi.setState(
        produce((state) => {
          state.viewport.point_normal_length = data
        })
      )
      viewportSetPointNormalLength([data])
    } else if (key === 'prim-normal-length') {
      settingsApi.setState(
        produce((state) => {
          state.viewport.prim_normal_length = data
        })
      )
      viewportSetPrimNormalLength([data])
    } else if(key == 'near-plane') {
      let set_success = false
      settingsApi.setState(produce((state) => {
        if(data < state.viewport.far_plane) {
          state.viewport.near_plane = data
          set_success = true
        }
      }))
      if(set_success) {
        viewportSetNearPlane([data])
      }
    } else if(key == 'far-plane') {
        let set_success = false
        settingsApi.setState(produce((state) => {
          if(data > state.viewport.near_plane) {
            state.viewport.far_plane = data
            set_success = true
          }
        })
      )
      if(set_success) {
        viewportSetFarPlane([data])
      }
    }
  }, [])

  const colors = [
    ['background-color', backgroundColor, true],
    ['wireframe-color', wireframeColor, true],
    ['point-color', pointColor, false],
    ['point-index-color', pointIndexColor, true],
    ['point-normal-color', pointNormalColor, false],
    ['prim-index-color', primIndexColor, true],
    ['prim-normal-color', primNormalColor, false]
  ]

  const values = [
    ['point-size', pointSize, 20.0],
    ['index-size', indexSize, 50.0],
    ['point-normal-length', pointNormalLength, 2.0],
    ['prim-normal-length', primNormalLength, 2.0],
    ['near-plane', nearPlane, 0.01],
    ['far-plane', farPlane, 1000.0]
  ]

  return (
    <PopoverContent className={cn(presets['menu-primary'], 'w-[400px]')}>
      <div className="flex w-full flex-col gap-1 p-1">
        {values.map((src) => {
          return (
            <div className="grid h-6 grid-cols-2 items-center gap-4" key={src[0]}>
              <label className="col-span-1 text-right align-middle tracking-wide">
                {t(src[0] as any)}
              </label>
              <div className="h-full">
                <Slider
                  valueType={SliderValueType.Float}
                  defaulValue={src[1] as number}
                  rangeMin={0}
                  rangeMax={src[2] as number}
                  className="h-full align-middle text-sm"
                  onValueChange={(v: number) => {
                    applyChange(src[0] as string, v)
                  }}
                />
              </div>
            </div>
          )
        })}
        {colors.map((src) => {
          return (
            <div className="grid h-6 grid-cols-2 items-center gap-4" key={src[0] as string}>
              {src[0] === 'point-color' && (
                <Toggle
                  className="absolute left-4 size-4"
                  checked={settingsApi.getState().viewport.force_show_point_default_color}
                  onChange={(checked: boolean) => {
                    forceShowPointsByDefaultColor([checked])
                    settingsApi.setState(
                      produce((state) => {
                        state.viewport.force_show_point_default_color = checked
                      })
                    )
                  }}
                ></Toggle>
              )}

              <label className="col-span-1 text-right align-middle tracking-wide">
                {t(src[0] as any)}
              </label>
              <div className="h-full grid-cols-2">
                <ColorPicker
                  placement="left-start"
                  color={src[1] as string}
                  onChange={(c) => {
                    applyChange(src[0] as string, c)
                  }}
                  hideOpacity={!src[2] as boolean}
                >
                  <div
                    className="btn btn-circle btn-ghost m-0 size-full min-h-0 p-0"
                    style={{
                      backgroundColor: src[1] as string
                    }}
                  />
                </ColorPicker>
              </div>
            </div>
          )
        })}
      </div>
    </PopoverContent>
  )
}
