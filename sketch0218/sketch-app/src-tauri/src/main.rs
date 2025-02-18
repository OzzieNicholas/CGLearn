// Prevents additional console window on Windows in release
// #![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]

use std::ffi::c_char;
use std::os::windows::ffi::OsStringExt;

use ffi::view::WGPUTextureDataLayout;
use tauri::RunEvent;
use webview2_com::Microsoft::Web::WebView2::Win32::ICoreWebView2_19;
#[cfg(target_os = "windows")]
use window_vibrancy::apply_acrylic;
#[cfg(target_os = "windows")]
use window_vibrancy::apply_blur;
#[cfg(target_os = "macos")]
use window_vibrancy::{apply_vibrancy, NSVisualEffectMaterial};

// use commands::commands::NousView;
use commands::viewport;

use log::{debug, error};
use tauri::{utils::config::WebviewUrl, webview::WebviewWindowBuilder, Emitter, Listener, Manager, PhysicalSize};
use windows::core::Interface;

// use nous_rs::channel::{get_receiver, EventMsgData};
// use nous_rs::nous_api::nous;
// use nous_rs::rpc::{self, client, server};

mod commands;
mod ffi;
use crate::commands::commands::*;
use crate::commands::image_viewer::open_image_viewer;

static mut EXITS: bool = false;

fn main() {
    env_logger::init();

    // let nous_view_state = NousView::new();
    // let nous_msg_receiver: Arc<Mutex<Receiver<String>>> = get_receiver();

    let mut app = tauri::Builder::default()
        .plugin(tauri_plugin_clipboard_manager::init())
        .plugin(tauri_plugin_store::Builder::new().build())
        .plugin(tauri_plugin_dialog::init())
        .plugin(tauri_plugin_os::init())
        .plugin(tauri_plugin_cli::init())
        .plugin(tauri_plugin_window_state::Builder::default().build())
        // .manage(nous_view_state)
        .setup(|app| {
            close_console_window();

            // let (server_mode, client_mode) = process_cli(app);

            // if !server_mode {
            // let main_window = app.get_webview_window("main").unwrap();

            let window = WebviewWindowBuilder::new(app, "main", WebviewUrl::App("index.html".into()))
                .title("Nous")
                .inner_size(800.0, 600.0)
                .transparent(true)
                .decorations(false)
                .disable_drag_drop_handler()
                .build()
                .unwrap();

            post_init_window_state(&window);

            unsafe {
                let mut width: u32 = 800;
                let mut height: u32 = 600;
                // ffi::view::sketch_get_surface_size(&mut width, &mut height);
                width = (width as f32 / 256.0).ceil() as u32 * 256;
                let main_webview = window.get_webview_window("main").unwrap();
                ffi::view::sketch_render_init();
                viewport::sketch_view_buf_create_or_reset(&main_webview, (width * height * 4) as u64);
                ffi::core::sketch_core_init();
                ffi::core::sketch_create_project("D://test", "");
                ffi::core::sketch_active_controller("Create_Rectangle");
            }

            window.listen("tauri://close-requested", move |_| {
                commands::commands::clear_server_post_frame_callback();
                // client::shutdown_frontend_rpc_server();
                // nous::nous_view_shutdown();
                finalize_prelude();
            });

            #[cfg(target_os = "macos")]
            apply_vibrancy(&window, NSVisualEffectMaterial::HudWindow, None, None).unwrap_or_default();

            #[cfg(target_os = "windows")]
            {
                if let tauri_plugin_os::Version::Semantic(_, _, build_number) = tauri_plugin_os::version() {
                    if build_number < 22000 {
                        apply_blur(&window, None).unwrap_or_default();
                    } else {
                        apply_acrylic(&window, None).unwrap_or_default();
                    }
                } else {
                    apply_acrylic(&window, None).unwrap_or_default();
                }
            }

            Ok(())
        })
        .plugin(tauri_plugin_shell::init())
        .invoke_handler(tauri::generate_handler![
            finalize_prelude,
            invoke,
            // init_viewport,
            resize_viewport,
            rust_log,
            // render_frame,
            write_text_file,
            open_console_window,
            close_console_window,
            toggle_console_window,
            open_profiler,
            open_tracy_profiler,
            open_image_viewer,
            open_devtools,
            get_current_backend_addr,
            connect_to_server,
            disconnect_from_server,
            reload_all,
            get_server_default_port,
            resize_surface_size,
            set_mouse_button_down_event,
            set_mouse_button_up_event,
            set_mouse_move_event,
            set_keyboard_event,
            sketch_core_tick,
        ])
        .build(tauri::generate_context!())
        .expect("error while building tauri application");
    app.run(|_app, _event| {});

    // app.run(|_app, _event| {
    //     sketch_core_tick();
    // });

    // unsafe {
    //     while !EXITS {
    //         app.run_iteration(|_app, _event| match _event {
    //             RunEvent::ExitRequested { code, api, .. } => {
    //                 _app.cleanup_before_exit();
    //                 EXITS = true;
    //                 println!("Exits");
    //             }
    //             _ => {}
    //         });
    //     }
    // }
    // loop {
    //     app.run_iteration(|_app, _event| {});
    //     sketch_core_tick();
    //     if app.webview_windows().is_empty() {
    //         app.cleanup_before_exit();
    //         break;
    //     }
    // }
}

fn post_init_window_state(window: &tauri::WebviewWindow) {
    let min_size = PhysicalSize::new(480, 320);
    let mut new_size = PhysicalSize::new(min_size.width, min_size.height);

    let mut should_update_size = false;
    let mut should_maximize = false;

    if let Ok(maximized) = window.is_maximized() {
        debug!("maximized: {}", maximized);
        if maximized {
            should_update_size = true;
            should_maximize = true;
            let res = window.unmaximize();
            if res.is_err() {
                error!("Failed to unmaximize window");
            }
        }
    } else {
        error!("Failed to get window maximized state");
    }

    let maybe_size = window.outer_size();
    if maybe_size.is_err() {
        error!("Failed to get window size");
    } else {
        let size = maybe_size.unwrap();
        if size.width < min_size.width {
            should_update_size = true;
            new_size.width = min_size.width;
        }
        if size.height < min_size.height {
            should_update_size = true;
            new_size.height = min_size.height;
        }
    }

    if should_update_size {
        let res = window.set_size(new_size);
        if res.is_err() {
            error!("Failed to set window size");
        }
    }

    let maybe_position = window.outer_position();
    if maybe_position.is_err() {
        error!("Failed to get window position");
    } else {
        let position = maybe_position.unwrap();
        if position.x < 0 || position.y < 0 {
            let res = window.set_position(tauri::PhysicalPosition::new(0.0, 0.0));
            if res.is_err() {
                error!("Failed to set window position");
            }
        }
    }

    let res = window.set_min_size(Some(min_size));
    if res.is_err() {
        error!("Failed to set min size");
    }

    if should_maximize {
        let res = window.maximize();
        if res.is_err() {
            error!("Failed to maximize window");
        }
    }
}
