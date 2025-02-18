struct VertexIn {
  @location(0) position: vec2f,
  @location(1) uv: vec2f,
  @location(2) color: vec4f
};

struct VertexOut {
  @builtin(position) position: vec4f,
  @location(0) uv: vec2f,
  @location(1) color: vec4f
};

struct Uniforms {
    mvp: mat4x4f,
    gamma: f32
};

@group(0) @binding(0)
var<uniform> data: Uniforms;

@vertex
fn vs_main(in: VertexIn) -> VertexOut {
    var out: VertexOut;
    out.position = data.mvp * vec4f(in.position, 0.0, 1.0);
    out.uv = in.uv;
    out.color = in.color;
    return out;
}

@fragment
fn fs_main(in: VertexOut) -> @location(0) vec4<f32> {
    return vec4<f32>(pow(in.color.rgb, vec3<f32>(data.gamma)), in.color.a);
}
