import { ClassValue, clsx } from 'clsx'
import { twJoin, twMerge } from 'tailwind-merge'

export function cn(...inputs: ClassValue[]) {
  return twMerge(clsx(inputs))
}

export function join(...inputs: ClassValue[]) {
  return twJoin(clsx(inputs))
}
