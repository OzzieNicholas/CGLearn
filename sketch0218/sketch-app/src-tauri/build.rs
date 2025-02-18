use std::env;

fn main() {
    #[cfg(target_os = "macos")]
    {
        use std::env;
        use std::path::Path;

        let nous_dir = env::var("NOUS_INSTALL_DIR").expect("you should set NOUS_INSTALL_DIR first");
        let nous_dir_path = Path::new(&nous_dir);

        let abs_nous_dir = {
            if Path::is_absolute(nous_dir_path) {
                nous_dir_path.to_path_buf()
            } else {
                env::current_dir().unwrap().join(nous_dir_path)
            }
        };
        println!("nous dir is {}", abs_nous_dir.display());

        println!("cargo:rerun-if-env-changed=NOUS_INSTALL_DIR");
        // println!("cargo:rustc-link-search=all={}", abs_nous_dir.display());
        println!("cargo:rustc-link-arg=-Wl,-rpath,{}", abs_nous_dir.display());
    }

    // let dst = xmake::build("sketch-rs");
    let build_dir = env::var("XMAKE_BUILD_TARGET_DIR").expect("build from xmake");
    println!("build dir is {}", build_dir);
    println!("cargo:rustc-link-search=native={}", build_dir);
    println!("cargo::rustc-link-lib=static=sketch-rs");

    tauri_build::build()
}
