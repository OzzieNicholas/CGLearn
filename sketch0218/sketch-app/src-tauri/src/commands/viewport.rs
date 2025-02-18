use lazy_static::lazy_static;
use std::ffi::c_void;
use std::ptr::{null, null_mut, NonNull};
use std::sync::Mutex;
use std::thread;

use tauri::webview::PlatformWebview;
use windows::core::Interface;

#[cfg(windows)]
use webview2_com::Microsoft::Web::WebView2::Win32::{
    ICoreWebView2Environment12, ICoreWebView2SharedBuffer, ICoreWebView2_19, COREWEBVIEW2_SHARED_BUFFER_ACCESS_READ_ONLY,
};

use crate::ffi;
use crate::ffi::view::WGPUTextureDataLayout;

struct SketchViewBuffer {
    #[cfg(windows)]
    pub buffer: Option<ICoreWebView2SharedBuffer>,
    pub size: u64,
}

impl SketchViewBuffer {
    fn new() -> SketchViewBuffer {
        return SketchViewBuffer { buffer: None, size: 0 };
    }

    fn clear(&mut self) {
        self.buffer = None;
        self.size = 0;
    }

    unsafe fn get_buf_ptr(&mut self) -> *mut u8 {
        if self.buffer.is_none() {
            return null_mut();
        }

        let mut ptr: *mut u8 = null_mut();
        match self.buffer.as_ref().unwrap().Buffer(&mut ptr) {
            Ok(_) => return ptr,
            Err(_) => return null_mut(),
        }
    }

    #[cfg(windows)]
    fn reset(&mut self, webview: &PlatformWebview, size: u64) -> Result<(), ()> {
        self.clear();

        unsafe {
            let controller = webview.controller();
            let webview2 = controller.CoreWebView2().unwrap().cast::<ICoreWebView2_19>().unwrap();

            let environment: ICoreWebView2Environment12 = webview2.Environment().unwrap().cast::<ICoreWebView2Environment12>().unwrap();

            if let Ok(buffer) = environment.CreateSharedBuffer(size) {
                self.buffer = Some(buffer);
                self.size = size;
                return Ok(());
            }

            return Err(());
        }
    }
}

unsafe impl Sync for SketchViewBuffer {}
unsafe impl Send for SketchViewBuffer {}

lazy_static! {
    static ref SKETCH_VIEW_BUFFER: Mutex<SketchViewBuffer> = Mutex::new(SketchViewBuffer::new());
}

pub fn sketch_view_buf_create_or_reset(webview: &tauri::WebviewWindow, size: u64) {
    unsafe {
        webview.with_webview(move |native_webview| {
            println!("create sketch view buffer: {}", size);
            let mut buffer = SKETCH_VIEW_BUFFER.lock().unwrap();
            if let Err(()) = buffer.reset(&native_webview, size) {
                println!("Failed to reset sketch view buffer");
            }

            ffi::view::sketch_set_present_callback(move |layout, data, width, height, size| {
                let mut view_buffer = SKETCH_VIEW_BUFFER.lock().unwrap();
                view_buffer.get_buf_ptr().copy_from(data as *const u8, size as usize);

                let webview2 = native_webview.controller().CoreWebView2().unwrap().cast::<ICoreWebView2_19>().unwrap();
                let buffer = view_buffer.buffer.as_ref().unwrap();
                webview2.PostSharedBufferToScript(buffer, COREWEBVIEW2_SHARED_BUFFER_ACCESS_READ_ONLY, None);
            });
        });
    }
}
