import react from '@vitejs/plugin-react'
import million from 'million/compiler'
import { defineConfig } from 'vite'
import { fileURLToPath } from 'url'
import crossOriginIsolation from 'vite-plugin-cross-origin-isolation'

// https://vitejs.dev/config/
export default defineConfig(async () => ({
  plugins: [million.vite({ auto: true }), react(), crossOriginIsolation()],

  // Vite options tailored for Tauri development and only applied in `tauri dev` or `tauri build`
  //
  // 1. prevent vite from obscuring rust errors
  clearScreen: false,
  // 2. tauri expects a fixed port, fail if that port is not available
  server: {
    port: 1420,
    strictPort: true,
    watch: {
      // 3. tell vite to ignore watching `src-tauri`
      ignored: ['**/src-tauri/**']
    }
  },

  build: {
    rollupOptions: {
      input: {
        main: 'index.html',
        second: 'image_viewer.html'
      }
    }
  },

  resolve: {
    alias: {
      '@': fileURLToPath(new URL('./src', import.meta.url))
    }
  }
}))
