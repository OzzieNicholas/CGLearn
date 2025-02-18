use std::ffi::*;
use widestring::*;

extern "C" {
    pub fn sketch_core_init();

    pub fn sketch_core_tick();

    fn sketch_core_create_project(path: *const c_ushort, env: *const c_ushort);

    fn sketch_core_active_controller(name: *const c_ushort);
}

pub fn sketch_create_project(path: &str, env: &str) {
    let path_c_wstring = U16CString::from_str(path).unwrap();
    let env_c_wstring = U16CString::from_str(env).unwrap();

    let path_c_wstring_ptr = path_c_wstring.as_ptr();
    let env_c_wstring_ptr = path_c_wstring.as_ptr();

    unsafe {
        sketch_core_create_project(path_c_wstring_ptr, env_c_wstring_ptr);
    }
}

pub fn sketch_active_controller(name: &str) {
    let name_c_wstring = U16CString::from_str(name).unwrap();
    let name_c_wstring_ptr = name_c_wstring.as_ptr();

    unsafe {
        sketch_core_active_controller(name_c_wstring_ptr);
    }
}
