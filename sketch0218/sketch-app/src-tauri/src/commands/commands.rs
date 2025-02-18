use lazy_static::lazy_static;
use log::{debug, error, info, warn};
use path_slash::PathExt;
use serde::Deserialize;
use std::env::current_exe;
use std::fs;
use std::path::Path;
use std::process::Command;
use std::sync::OnceLock;
use std::{
    pin::Pin,
    sync::{Arc, Mutex},
};
use tauri::webview::PlatformWebview;
use tauri::{utils::config::WebviewUrl, webview::WebviewWindowBuilder, Emitter, Manager};
use windows::core::IUnknown;

#[cfg(windows)]
extern crate winapi;

// #[derive(Default)]
// pub struct NousView {
//     // pub buffer: webview::Sha
//     pub buffer: Arc<Mutex<Vec<u8>>>,
//     pub initialized: Arc<Mutex<bool>>,
// }

// impl NousView {
//     pub fn new() -> NousView {
//         #[cfg(target_os = "macos")]
//         {
//             nous::nous_view_init_rs();
//             return NousView {
//                 initialized: Arc::new(Mutex::new(true)),
//                 buffer: Arc::new(Mutex::new(Vec::default())),
//             };
//         }
//
//         NousView {
//             initialized: Arc::new(Mutex::new(false)),
//             buffer: Arc::new(Mutex::new(Vec::default())),
//         }
//     }
// }

// pub static mut PRELUDE: *mut NousPreludeForRust = std::ptr::null_mut();
static APP_HANDLE: OnceLock<tauri::AppHandle> = OnceLock::new();

pub fn setup_prelude() -> bool {
    // if !unsafe { PRELUDE.is_null() } {
    //     return true;
    // }
    //
    // unsafe {
    //     PRELUDE = nous::create_prelude();
    // }
    //
    // if unsafe { PRELUDE.is_null() } {
    //     error!("Failed to create prelude");
    //
    //     return false;
    // }
    //
    // info!("nous prelude init success");
    //
    return true;
}

#[tauri::command]
pub fn finalize_prelude() {
    // if unsafe { PRELUDE.is_null() } {
    //     return;
    // }
    //
    // let prelude_pin = unsafe { Pin::new_unchecked(&mut *PRELUDE) };
    // prelude_pin.finalize();
}

#[tauri::command]
pub async fn invoke(func: String, args: String) -> String {
    debug!("{}:{}", func, args);

    // if client::rpc_connected() {
    //     if let Some(current_client) = client::get_current_backend_client() {
    //         let request = nous_rpc::InvokeRequest { func, args };
    //         let response = current_client.invoke(request).await;
    //         match response {
    //             Ok(response) => return response.into_inner().result,
    //             Err(e) => {
    //                 println!("invoke error {}", e.to_string());
    //             }
    //         }
    //     }
    //     String::default()
    // } else {
    //     let prelude_pin = unsafe { Pin::new_unchecked(&mut *PRELUDE) };
    //     let result = prelude_pin.invoke(func.as_str(), args.as_str());
    //     result
    // }
    String::default()
}

#[tauri::command]
pub fn rust_log(text: &str) {
    println!("{}", text);
}

// #[tauri::command]
// pub async fn init_viewport(state: tauri::State<'_, NousView>) -> Result<(), ()> {
//     if *state.initialized.lock().unwrap() == true {
//         return Ok(());
//     }
//
//     let viewport_default_size = 512;
//
//     reset_renderer(viewport_default_size, viewport_default_size, state).await;
//     Ok(())
// }

#[tauri::command]
pub fn sketch_create_project(path: &str, env: &str) {
    ffi::core::sketch_create_project(path, env);
}

#[tauri::command]
pub fn resize_viewport(window: tauri::Window, width: u32, height: u32) -> Result<(), ()> {
    let webview = window.get_webview_window("main").unwrap();

    unsafe {
        crate::ffi::view::set_resize_surface_event(width, height);
    }

    let width = (width as f32 / 256.0).ceil() as u32 * 256;
    let size = (width * height * 4) as u64;
    crate::commands::viewport::sketch_view_buf_create_or_reset(&webview, size);
    Ok(())
}

// async fn reset_renderer(width: u32, height: u32, state: tauri::State<'_, NousView>) {
// // setup buffer
// let mut buffer: Vec<u8> = vec![0; (width * height * 4).try_into().unwrap()];
// let raw_buffer: &mut &mut [u8] = &mut buffer.as_mut_slice();
//
// unsafe {
//     debug!("nous prelude pointer is {:?}", PRELUDE);
// }
//
// {
//     if client::rpc_connected() {
//         if let Some(current_client) = client::get_current_backend_client() {
//             let request = nous_rpc::ResizeRenderViewRequest { width, height };
//             current_client.resize_render_view(request).await.unwrap();
//         }
//     } else {
//         nous::set_render_buffer_addr_rs(raw_buffer, width, height);
//         if *state.initialized.lock().unwrap() == false {
//             *state.initialized.lock().unwrap() = true;
//             nous::nous_view_init_rs();
//         }
//     }
//     debug!("buffer len is {:?}", buffer.len());
//
//     let mut current_buffer = state.buffer.lock().unwrap();
//     *current_buffer = buffer;
// }
// }

// #[tauri::command]
// pub async fn render_frame(state: tauri::State<'_, NousView>, window: tauri::Window) -> Result<(), ()> {
//     debug!("render frame");
//
//     // if !client::rpc_connected() {
//     //     if *state.initialized.lock().unwrap() == false {
//     //         return Err(());
//     //     }
//     // }
//
//     // clone buffer
//     let src_buffer = state.buffer.lock().unwrap();
//     if src_buffer.len() == 0 {
//         return Ok(());
//     }
//
//     let buffer: Box<Vec<u8>> = Box::new(src_buffer.to_vec());
//
//     let main_webview = window.get_webview_window("main").unwrap();
//     let _ = main_webview.with_webview(move |webview| {
//         #[cfg(target_os = "linux")]
//         {
//             todo!();
//             // see https://docs.rs/webkit2gtk/2.0.0/webkit2gtk/struct.WebView.html
//             // and https://docs.rs/webkit2gtk/2.0.0/webkit2gtk/trait.WebViewExt.html
//             // use webkit2gtk::WebViewExt;
//             // webview.inner().set_zoom_level(4.);
//         }
//
//         #[cfg(windows)]
//         unsafe {
//             use webview2_com::Microsoft::Web::WebView2::Win32::{
//                 ICoreWebView2Environment12, ICoreWebView2_19, COREWEBVIEW2_SHARED_BUFFER_ACCESS_READ_ONLY,
//             };
//             use windows::core::Interface;
//
//             // see https://docs.rs/webview2-com/0.19.1/webview2_com/Microsoft/Web/WebView2/Win32/struct.ICoreWebView2Controller.html
//             let controller = webview.controller();
//             let webview2 = controller.CoreWebView2().unwrap().cast::<ICoreWebView2_19>().unwrap();
//
//             let environment = webview2.Environment().unwrap().cast::<ICoreWebView2Environment12>().unwrap();
//
//             // create shared array buffer
//             let size = buffer.len().try_into().unwrap();
//             let mut ptr = (buffer.to_owned()).as_mut_ptr();
//             let ptr_ptr: *mut *mut u8 = &mut ptr;
//             let shared_buffer = environment.CreateSharedBuffer(size).unwrap();
//             let _ = shared_buffer.Buffer(ptr_ptr);
//             ptr.copy_from(buffer.as_ptr(), buffer.len());
//
//             // post shared buffer
//             let res = webview2.PostSharedBufferToScript(&shared_buffer, COREWEBVIEW2_SHARED_BUFFER_ACCESS_READ_ONLY, None);
//
//             // handle error
//             if res.is_err() {
//                 warn!("post shared array buffer failed: {}", res.err().unwrap())
//             }
//         }
//
//         #[cfg(target_os = "macos")]
//         unsafe {
//             todo!();
//             // let () = msg_send![webview.inner(), setPageZoom: 4.];
//             // let () = msg_send![webview.controller(), removeAllUserScripts];
//             // let bg_color: cocoa::base::id =
//             //     msg_send![class!(NSColor), colorWithDeviceRed:0.5 green:0.2 blue:0.4 alpha:1.];
//             // let () = msg_send![webview.ns_window(), setBackgroundColor: bg_color];
//         }
//
//         #[cfg(target_os = "android")]
//         {
//             todo!();
//             // use jni::objects::JValue;
//             // webview.jni_handle().exec(|env, _, webview| {
//             //     env.call_method(webview, "zoomBy", "(F)V", &[JValue::Float(4.)])
//             //         .unwrap();
//             // })
//         }
//     });
//
//     Ok(())
// }

fn server_post_frame_callback(buffer: Vec<u8>) {
    // let handle = APP_HANDLE.get().expect("APP_HANDLE is not set");
    // let state: tauri::State<'_, NousView> = handle.state();
    // let mut src_buffer = state.buffer.lock().unwrap();
    //
    // if src_buffer.len() == buffer.len() {
    //     *src_buffer = buffer;
    //     if handle.emit("frame", ()).is_err() {
    //         error!("Failed to emit frame event");
    //     }
    // } else {
    //     println!("Server post frame size mismatch");
    // }
}

pub fn set_server_post_frame_callback() {
    // client::set_post_frame_func_ptr(server_post_frame_callback);
}

pub fn clear_server_post_frame_callback() {
    // client::clear_post_frame_func_ptr();
}

pub fn store_app_handle(handle: tauri::AppHandle) {
    APP_HANDLE.set(handle).expect("Failed to set APP_HANDLE");
}

#[derive(Deserialize)]
struct LoadGraphResult {
    result: bool,
}

pub fn load_graph(file_path: &str) {
    let path = Path::new(file_path);
    if !path.exists() {
        println!("Project file not exists: {}", file_path);
        return;
    }
    let file_path = String::from(path.to_slash().unwrap());
    println!("{}", file_path.as_str());

    let load_graph_args = format!("{}\"args\":[0, \"{}\"]{}", "{", file_path, "}");
    let (handle, _) = get_current_runtime();

    handle.block_on(async move {
        let load_graph_result = invoke("load_graph".to_string(), load_graph_args).await;
        if let Ok(result) = serde_json::from_str::<LoadGraphResult>(&load_graph_result) {
            if result.result {
                return;
            }
        }
        println!("Failed to load project file: {}", file_path);
    });
}

pub fn switch_graph_type(graph_type: String) {
    let switch_graph_type_args = format!("{}\"args\":[\"{}\"]{}", "{", graph_type, "}");
    let (handle, _) = get_current_runtime();

    handle.block_on(async move {
        let load_graph_result = invoke("switch_graph_type".to_string(), switch_graph_type_args).await;
        if let Ok(result) = serde_json::from_str::<LoadGraphResult>(&load_graph_result) {
            if result.result {
                return;
            }
        }
        println!("Failed to switch graph type: {}", graph_type);
    });
}

#[tauri::command]
pub async fn write_text_file(path: &str, content: &str) -> Result<(), ()> {
    fs::write(path, content).unwrap_or_default();
    Ok(())
}

lazy_static! {
    static ref CONSOLE_WINDOW_OPENED: Mutex<bool> = Mutex::new(true);
}

#[tauri::command]
pub fn open_console_window() {
    let mut opened = CONSOLE_WINDOW_OPENED.lock().unwrap();
    if *opened {
        return;
    }
    #[cfg(windows)]
    {
        use winapi::um::wincon::GetConsoleWindow;
        use winapi::um::winuser::{ShowWindow, SW_SHOWNORMAL};

        let window = unsafe { GetConsoleWindow() };
        if !window.is_null() {
            unsafe {
                ShowWindow(window, SW_SHOWNORMAL);
                *opened = true;
            }
        }
    }
}

#[tauri::command]
pub fn close_console_window() {
    let mut opened = CONSOLE_WINDOW_OPENED.lock().unwrap();
    if !*opened {
        return;
    }
    #[cfg(windows)]
    {
        use winapi::um::wincon::GetConsoleWindow;
        use winapi::um::winuser::{ShowWindow, SW_HIDE};

        let window = unsafe { GetConsoleWindow() };
        if !window.is_null() {
            unsafe {
                ShowWindow(window, SW_HIDE);
                *opened = false;
            }
        }
    }
}

#[tauri::command]
pub fn toggle_console_window() {
    let mut opened = CONSOLE_WINDOW_OPENED.lock().unwrap();
    #[cfg(windows)]
    {
        use winapi::um::wincon::GetConsoleWindow;
        use winapi::um::winuser::{ShowWindow, SW_HIDE, SW_SHOWNORMAL};

        let window = unsafe { GetConsoleWindow() };
        // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-showwindow
        if !window.is_null() {
            unsafe {
                if *opened {
                    ShowWindow(window, SW_HIDE);
                    *opened = false;
                } else {
                    ShowWindow(window, SW_SHOWNORMAL);
                    *opened = true;
                }
            }
        }
    }
}

#[tauri::command]
pub async fn open_profiler(window: tauri::Window) -> Result<(), ()> {
    let main_webview = window.get_webview_window("main").unwrap();
    WebviewWindowBuilder::new(window.app_handle(), "profiler", WebviewUrl::App("profiler/profiler.html".into()))
        .title("Nous Performance Profiler")
        .parent(&main_webview)
        .unwrap()
        .on_web_resource_request(|request, response| {
            if request.uri().scheme_str() == Some("tauri") {
                response
                    .headers_mut()
                    .insert("Cross-Origin-Opener-Policy", "same-origin".try_into().unwrap());
                response
                    .headers_mut()
                    .insert("Cross-Origin-Embedder-Policy", "require-corp".try_into().unwrap());
            }
        })
        .build()
        .unwrap();
    Ok(())
}

#[tauri::command]
pub async fn open_tracy_profiler() -> Result<(), ()> {
    let cwd = current_exe().unwrap();
    let cwd = cwd.parent().unwrap();
    let tracy_names = ["tracy-profiler.exe", "tracy.exe", "tracy-profiler", "tracy"];
    for name in tracy_names.iter() {
        let path = cwd.join(name);
        if path.exists() {
            Command::new(path).arg("-a").arg("127.0.0.1").arg("-p").arg("12113").spawn().unwrap();
            return Ok(());
        }
    }
    Ok(())
}

#[tauri::command]
pub async fn open_devtools(window: tauri::Window) -> Result<(), ()> {
    let main_webview = window.get_webview_window("main").unwrap();
    main_webview.open_devtools();
    Ok(())
}

#[tauri::command]
pub fn get_current_backend_addr() -> Option<String> {
    // client::get_current_backend_addr()
    return None;
}

fn reload_webview(window: tauri::Window) {
    if let Some(main_webview) = window.get_webview_window("main") {
        main_webview.eval("location.reload();").unwrap();
    }
}

#[tauri::command]
pub fn connect_to_server(window: tauri::Window, port: u16) -> bool {
    // if client::rpc_connected_check() {
    //     return false;
    // }

    // if client::connect_to_backend_server(port, 51099) {
    //     if APP_HANDLE.get().is_none() {
    //         store_app_handle(window.app_handle().clone());
    //     }
    //     nous::clear_render_buffer_addr_rs();
    //     set_server_post_frame_callback();
    //     reload_webview(window);
    //     return true;
    // }
    false
}

#[tauri::command]
pub fn disconnect_from_server(window: tauri::Window) -> bool {
    // if client::rpc_connected() || get_current_backend_addr().is_some() {
    //     setup_prelude();
    //     client::disconnect_from_backend_server();
    //     clear_server_post_frame_callback();
    //     reload_webview(window);
    //     return true;
    // }
    false
}

#[tauri::command]
pub fn get_server_default_port() -> i32 {
    // rpc::common::default_backend_server_port() as i32
    return 0;
}

#[tauri::command]
pub fn reload_all(window: tauri::Window) {
    let mut reloaded = false;

    // if !client::rpc_connected_check() {
    //     reloaded = disconnect_from_server(window.clone());
    // }

    if !reloaded {
        reload_webview(window);
    }
}

fn get_current_runtime() -> (tokio::runtime::Handle, Option<tokio::runtime::Runtime>) {
    match tokio::runtime::Handle::try_current() {
        Ok(h) => (h, None),
        Err(_) => {
            let rt = tokio::runtime::Runtime::new().unwrap();
            (rt.handle().clone(), Some(rt))
        }
    }
}

use crate::commands::viewport::sketch_view_buf_create_or_reset;
use crate::ffi;

#[tauri::command]
pub fn set_mouse_move_event(x: f32, y: f32, button: i32, ctrl: bool, alt: bool, shift: bool) {
    unsafe {
        ffi::view::set_mouse_move_event(x, y, button, ctrl, alt, shift);
    }
}

#[tauri::command]
pub fn set_mouse_button_down_event(x: f32, y: f32, button: i32, ctrl: bool, alt: bool, shift: bool) {
    unsafe {
        ffi::view::set_mouse_button_down_event(x, y, button, ctrl, alt, shift);
    };
}

#[tauri::command]
pub fn set_mouse_button_up_event(x: f32, y: f32, button: i32, ctrl: bool, alt: bool, shift: bool) {
    unsafe {
        ffi::view::set_mouse_button_up_event(x, y, button, ctrl, alt, shift);
    };
}

#[tauri::command]
pub fn set_keyboard_event(key: i32, action: i32, mods: i32) {
    unsafe {
        ffi::view::set_keyboard_event(key, action, mods);
    };
}

#[tauri::command]
pub fn resize_surface_size(x: u32, y: u32) {
    unsafe {
        ffi::view::set_resize_surface_event(x, y);
    };
}

#[tauri::command]
pub fn sketch_core_tick() {
    unsafe {
        ffi::core::sketch_core_tick();
    };
}
