use std::ffi::c_void;

#[repr(C)]
pub struct WGPUTextureDataLayout {
    next_in_chain: *const c_void,
    offset: u64,
    bytes_per_row: u64,
    rows_pre_image: u64,
}

type PresentCallbackFn = unsafe extern "C" fn(*const WGPUTextureDataLayout, *const c_void, u32, u32, u64, *mut c_void);

extern "C" {
    pub fn set_mouse_button_down_event(x: f32, y: f32, button: i32, ctrl: bool, alt: bool, shift: bool);

    pub fn set_mouse_button_up_event(x: f32, y: f32, button: i32, ctrl: bool, alt: bool, shift: bool);

    pub fn set_mouse_move_event(x: f32, y: f32, button: i32, ctrl: bool, alt: bool, shift: bool);

    pub fn set_resize_surface_event(x: u32, y: u32);

    pub fn set_keyboard_event(key: i32, action: i32, mods: i32);

    pub fn sketch_render_init();

    pub fn sketch_get_surface_size(width: &mut u32, height: &mut u32);

    fn sketch_set_callback_after_present(cb: PresentCallbackFn, user_data: *mut c_void);
}

unsafe extern "C" fn call_closure<F>(
    layout: *const WGPUTextureDataLayout,
    data: *const c_void,
    width: u32,
    height: u32,
    size: u64,
    user_data: *mut c_void,
) where
    F: FnMut(*const WGPUTextureDataLayout, *const c_void, u32, u32, u64) + 'static,
{
    let callback_ptr = user_data as *mut F;
    let callback = &mut *callback_ptr;
    callback(layout, data, width, height, size);
}

pub fn sketch_set_present_callback<T>(cb: T)
where
    T: FnMut(*const WGPUTextureDataLayout, *const c_void, u32, u32, u64) + 'static,
{
    let data = Box::into_raw(Box::new(cb));
    unsafe {
        sketch_set_callback_after_present(call_closure::<T>, data as *mut c_void);
    }
    println!("sketch set callback");
}
