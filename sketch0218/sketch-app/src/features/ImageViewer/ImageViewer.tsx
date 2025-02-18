import React from 'react'
import ReactDOM from 'react-dom/client'

import '@/css/image-viewer.css'

import { useEffectOnce } from 'react-use'
import { useState } from 'react'
import { listen } from '@tauri-apps/api/event'

const ImageViewer = () => {
  const [base64Img, setBase64ImgImg] = useState<string | null>(null)
  useEffectOnce(() => {
    const unlisten = listen<string>('UpdateImageEvent', (event) => {
      if (event.payload.length > 0) {
        setBase64ImgImg(event.payload)
      } else {
        setBase64ImgImg(null)
      }
    })
    return () => {
      unlisten.then((fn) => fn())
    }
  })
  return (
    <div className="h-screen w-screen bg-black" data-theme="dark">
      {base64Img !== null && <img src={`data:image/png;base64,${base64Img}`} alt="Base64 Image" />}
    </div>
  )
}

ReactDOM.createRoot(document.getElementById('root') as HTMLElement).render(
  <React.StrictMode>
    <ImageViewer />
  </React.StrictMode>
)
