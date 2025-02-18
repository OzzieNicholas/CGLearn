use actix_web::middleware::Logger;
use actix_web::web::JsonConfig;
use actix_web::{web, App, HttpResponse, HttpServer, Responder};
use lazy_static::lazy_static;
use serde::Deserialize;
use std::sync::{Arc, Mutex};
use tauri::{utils::config::WebviewUrl, webview::WebviewWindowBuilder, Emitter, Listener, Manager, WebviewWindow};

lazy_static! {
    static ref WINDOW: Mutex<Option<WebviewWindow>> = Mutex::new(None);
}

#[derive(Deserialize)]
struct ImageQuery {
    base64: String,
}

#[tauri::command]
pub async fn open_image_viewer(window: tauri::Window) -> Result<(), ()> {
    let mut window_ref = WINDOW.lock().unwrap();
    if let Some(existing_window) = window_ref.as_ref() {
        if !existing_window.is_visible().unwrap_or(true) {
            existing_window.show().unwrap_or_default();
        }

        if existing_window.is_minimized().unwrap_or(false) {
            existing_window.unminimize().unwrap_or_default();
        }
        return Ok(());
    }

    let main_webview = window.get_webview_window("main").unwrap();
    let window = WebviewWindowBuilder::new(window.app_handle(), "image_viewer", WebviewUrl::App("image_viewer.html".into()))
        .title("Nous Image Viewer")
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

    // 使用 Arc 和 Mutex 来共享 Server 句柄
    let server_handle = Arc::new(Mutex::new(None));

    // 克隆句柄以在异步任务中使用
    let server_handle_clone = Arc::clone(&server_handle);

    // 启动一个异步任务来运行 HTTP 服务器
    tauri::async_runtime::spawn(async move {
        let server = HttpServer::new(move || {
            App::new()
                .wrap(Logger::default())
                .app_data(JsonConfig::default().limit(100 * 1024 * 1024))
                .route("/image", web::post().to(handle_image))
        })
        .bind(("127.0.0.1", 30111))
        .expect("Failed to bind address")
        .run();

        // 存储 Server 句柄
        *server_handle_clone.lock().unwrap() = Some(server.handle());
        println!("open server");
        // 运行服务器
        server.await
    });

    window.listen("tauri://close-requested", move |_| {
        let mut window_ref = WINDOW.lock().unwrap();
        if (*window_ref).is_some() {
            *window_ref = None;
        }
        let server_handle = server_handle.lock().unwrap();
        if let Some(server) = &*server_handle {
            println!("close server");
            #[allow(unused_variables)]
            let _future = server.stop(true);
        }
    });

    *window_ref = Some(window);
    Ok(())
}

async fn handle_image(query: web::Json<ImageQuery>) -> impl Responder {
    let window_ref = WINDOW.lock().unwrap();
    if let Some(existing_window) = window_ref.as_ref() {
        existing_window
            .app_handle()
            .emit("UpdateImageEvent", query.base64.as_str())
            .unwrap_or_default();
    }
    HttpResponse::Ok()
}
