import { Maybe } from '@/types/maybe'
import { ColorResult, hexToRgba, rgbaToHexa } from '@uiw/color-convert'

export function delay(ms: number) {
  return new Promise((resolve) => setTimeout(resolve, ms))
}

export function* sequence(n: number): Generator<number> {
  for (var i = 0; i < n; i++) yield i
}

export const deepCopy = <T>(target: T): T => {
  if (target === null) {
    return target
  }
  if (target instanceof Array) {
    const cp = [] as any[]
    ;(target as any[]).forEach((v) => {
      cp.push(v)
    })
    return cp.map((n: any) => deepCopy<any>(n)) as any
  }
  if (typeof target === 'object' && target) {
    const cp = { ...(target as { [key: string]: any }) } as { [key: string]: any }
    Object.keys(cp).forEach((k) => {
      cp[k] = deepCopy<any>(cp[k])
    })
    return cp as T
  }
  return target
}

export const rgbafToHexa = (color: number[]) => {
  for (let c of color) {
    if (c > 1) {
      return '#FFF'
    }
  }
  return rgbaToHexa({
    r: Math.floor(color[0] * 255.0),
    g: Math.floor(color[1] * 255.0),
    b: Math.floor(color[2] * 255.0),
    a: color.length == 4 ? color[3] : 1.0
  })
}

export const colorResultToRgbaf = (color: ColorResult) => {
  const c = color.rgba
  return [c.r / 255.0, c.g / 255.0, c.b / 255.0, c.a]
}

export const colorResultToRgbf = (color: ColorResult) => {
  const c = color.rgb
  return [c.r / 255.0, c.g / 255.0, c.b / 255.0]
}

export const hexToRgbaf = (color: string) => {
  const c = hexToRgba(color)
  return [c.r / 255.0, c.g / 255.0, c.b / 255.0, c.a]
}

export const isNumberSame = (a: number, b: number) => {
  return Math.abs(a - b) < 1e-6
}

export const isEmptyString = (str: Maybe<string>): boolean => {
  return str === undefined || str === null || str === ''
}

export const isNotEmptyString = (str: Maybe<string>) => {
  return !isEmptyString(str)
}
