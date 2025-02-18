/** @type {import('tailwindcss').Config} */
import { nextui } from '@nextui-org/react'
import typography from '@tailwindcss/typography'
import daisyui from 'daisyui'

export default {
  content: [
    './index.html',
    './image_viewer.html',
    './src/**/*.{js,ts,jsx,tsx}',
    './node_modules/@nextui-org/theme/dist/**/*.{js,ts,jsx,tsx}'
  ],

  theme: {
    extend: {
      height: {
        titlebar: '2rem'
      },
      keyframes: {
        'opacity-fade-in': {
          '0%': { opacity: '0' },
          '100%': { opacity: '100' }
        },
        'opacity-fade-out': {
          '0%': { opacity: '100' },
          '100%': { opacity: '0' }
        },
        'fade-in': {
          '0%': { opacity: '0', transform: 'scale(0.95)' },
          '100%': { opacity: '100', transform: 'scale(1.0)' }
        },
        'fade-out': {
          '0%': { opacity: '100', transform: 'scale(1.0)' },
          '100%': { opacity: '0', transform: 'scale(0.95)' }
        },
        'bounce-in': {
          '0%': { opacity: '0', transform: 'translateY(25%)' },
          '100%': { opacity: '100', transform: 'translateY(0)' }
        },
        'bounce-out': {
          '0%': { opacity: '100', transform: 'translateY(0)' },
          '100%': { opacity: '0', transform: 'translateY(25%)' }
        },
        'bounce-loop': {
          '0%, 100%': {
            transform: 'translateY(0)',
            'animation-timing-function': 'cubic-bezier(0, 0, 0.2, 1)'
          },
          '50%': {
            transform: 'translateY(-25%)',
            'animation-timing-function': ' cubic-bezier(0.8, 0, 1, 1)'
          }
        }
      },
      animation: {
        'opacity-fade-in': 'opacity-fade-in 200ms ease-in',
        'opacity-fade-out': 'opacity-fade-out 1000ms ease-out',
        'fade-in': 'fade-in 100ms ease-in',
        'fade-out': 'fade-out 1000ms ease-out',
        'bounce-in': 'bounce-in 300ms ease-in',
        'bounce-out': 'bounce-out 300ms ease-out',
        'bounce-loop': 'bounce-loop 1s infinite'
      },
      colors: {
        'menu-bg': 'rgba(45, 45, 45, 0.8)',
        'menu-bg-hover': 'rgba(245, 245, 245, 0.2)',
        'badge-active': '#55fad9',
        'text-select': '#55fad9',
        'btn-active': '#55fad9',
        'text-primary': 'rgb(244 244 245)',
        'text-primary-inv': 'rgb(63 63 70)',
        'text-input': 'rgba(50, 50, 50, 0.7)',
        'text-input-active': 'rgba(20, 20, 20, 0.7)'
      }
    }
  },

  plugins: [typography, daisyui, nextui()],

  daisyui: {
    themes: false, // false: only light + dark | true: all themes | array: specific themes like this ["light", "dark", "cupcake"]
    darkTheme: 'dark', // name of one of the included themes for dark mode
    base: true, // applies background color and foreground color for root element by default
    styled: true, // include daisyUI colors and design decisions for all components
    utils: true, // adds responsive and modifier utility classes
    prefix: '', // prefix for daisyUI classnames (components, modifiers and responsive class names. Not colors)
    logs: true, // Shows info about daisyUI version and used config in the console when building your CSS
    themeRoot: ':root' // The element that receives theme color CSS variables
  }
}
