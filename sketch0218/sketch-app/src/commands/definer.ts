export enum ParamType {
  Bool = 0, // checkbox
  Int, // input with max/min value -> slider + input
  Float, // input with max/min value -> slider + input
  String, // input
  Color, // color picker in the future
  Vec2, // vector2 float
  Vec3, // vector3 float
  Vec4, // vector4 float
  Vec2i, // vector2 int
  Vec3i, // vector3 int
  Vec4i, // vector4 int
  Mat3, // matrix3 float
  Mat4, // matrix4 float
  Mesh, // string
  Group, // string
  Enum, // select
  Button, // bool
  Code, // multiline string
  ScalarRamp,
  ColorRamp,
  List
}

export type ParamTypeStr =
  | 'bool'
  | 'int'
  | 'float'
  | 'string'
  | 'color'
  | 'vec2'
  | 'vec3'
  | 'vec4'
  | 'vec2i'
  | 'vec3i'
  | 'vec4i'
  | 'mat3'
  | 'mat4'
  | 'mesh'
  | 'group'
  | 'enum'
  | 'button'
  | 'code'
  | 'scalar_ramp'
  | 'color_ramp'
  | 'list'

export const ParamTypeStrMap: Record<ParamType, ParamTypeStr> = {
  [ParamType.Bool]: 'bool',
  [ParamType.Int]: 'int',
  [ParamType.Float]: 'float',
  [ParamType.String]: 'string',
  [ParamType.Color]: 'color',
  [ParamType.Vec2]: 'vec2',
  [ParamType.Vec3]: 'vec3',
  [ParamType.Vec4]: 'vec4',
  [ParamType.Vec2i]: 'vec2i',
  [ParamType.Vec3i]: 'vec3i',
  [ParamType.Vec4i]: 'vec4i',
  [ParamType.Mat3]: 'mat3',
  [ParamType.Mat4]: 'mat4',
  [ParamType.Mesh]: 'mesh',
  [ParamType.Group]: 'group',
  [ParamType.Enum]: 'enum',
  [ParamType.Button]: 'button',
  [ParamType.Code]: 'code',
  [ParamType.List]: 'list',
  [ParamType.ScalarRamp]: 'scalar_ramp',
  [ParamType.ColorRamp]: 'color_ramp'
}

export const ParamStrTypeMap: Record<ParamTypeStr, ParamType> = {
  bool: ParamType.Bool,
  int: ParamType.Int,
  float: ParamType.Float,
  string: ParamType.String,
  color: ParamType.Color,
  vec2: ParamType.Vec2,
  vec3: ParamType.Vec3,
  vec4: ParamType.Vec4,
  vec2i: ParamType.Vec2i,
  vec3i: ParamType.Vec3i,
  vec4i: ParamType.Vec4i,
  mat3: ParamType.Mat3,
  mat4: ParamType.Mat4,
  mesh: ParamType.Mesh,
  group: ParamType.Group,
  enum: ParamType.Enum,
  button: ParamType.Button,
  code: ParamType.Code,
  list: ParamType.List,
  scalar_ramp: ParamType.ScalarRamp,
  color_ramp: ParamType.ColorRamp
}

export enum ParamActionType {
  Value = 0,
  Checked,
  Editable,
  Visible,
  Expression
}

export enum LayoutType {
  Vertical = 0,
  Horizontal,
  Stack,
  Spacer
}

export type LayoutTypeStr = 'v' | 'h' | 'stack' | 'spacer'

export const LayoutTypeStrMap: Record<LayoutType, LayoutTypeStr> = {
  [LayoutType.Vertical]: 'v',
  [LayoutType.Horizontal]: 'h',
  [LayoutType.Stack]: 'stack',
  [LayoutType.Spacer]: 'spacer'
}

export const LayoutStrTypeMap: Record<LayoutTypeStr, LayoutType> = {
  v: LayoutType.Vertical,
  h: LayoutType.Horizontal,
  stack: LayoutType.Stack,
  spacer: LayoutType.Spacer
}

export enum Port {
  Input = 0,
  Output
}

export enum LogLevel {
  Trace = 0,
  Debug,
  Info,
  Warn,
  Error,
  Critical,
  Off
}
