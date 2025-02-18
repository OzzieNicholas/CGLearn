import { logColor, logName } from '@/constant/log'
import { useI18n } from '@/locales'
import { useNodeGraphStoreApi } from '@/stores/node-graph'
import { AttribInfo, NodeData, NodeInfo as NodeIndo_ } from '@/types/node-graph'
import { cn } from '@/utils/cn'
import { Card, CardBody, CardFooter, CardHeader, Divider, Tooltip } from '@nextui-org/react'
import { useAsync } from 'react-use'

const attribColor = {
  point: '#F06292',
  vertex: '#4FC3F7',
  prim: '#A5D6A7',
  global: '#FFB74D'
}

const clsNames = ['point', 'vertex', 'prim', 'global']

export default function NodeInfo({ data }: { data: NodeData }) {
  const storeApi = useNodeGraphStoreApi()
  const state = useAsync(() => storeApi.getState().getNodeInfo(data.id, 0), [data])
  const { t } = useI18n()

  let nodeInfo: NodeIndo_ = state.value
    ? (state.value as NodeIndo_)
    : {
        node: {
          execution_time: 0
        }
      }

  let attribData: {
    cls: keyof typeof attribColor
    attribs: AttribInfo[]
  }[] = []

  if (nodeInfo.mesh) {
    attribData = [
      nodeInfo.mesh.point,
      nodeInfo.mesh.vertex,
      nodeInfo.mesh.prim,
      nodeInfo.mesh.global
    ]
      .map((attribs, idx) => {
        return {
          cls: clsNames[idx] as keyof typeof attribColor,
          attribs: attribs.sort((a, b) => a.name.toLowerCase().localeCompare(b.name.toLowerCase()))
        }
      })
      .filter((data) => data.attribs.length > 0)
  }

  return (
    <Card className="bg-transparent tracking-wide text-text-primary selection:bg-text-select/40">
      {/* label */}
      <CardHeader className="flex gap-3 rounded-lg text-xl">
        <div className="font-bold text-text-primary/80">{data.label}</div>
        {!data.label.toLowerCase().startsWith(data['type_name'].toLowerCase()) && (
          <div className="badge rounded-md border-0 bg-white/20 text-text-primary">
            {data['type_name']}
          </div>
        )}
        <div className="badge rounded-md border-0 bg-white/20 text-text-primary">
          {`v${data.version}`}
        </div>
      </CardHeader>

      <Divider className="bg-white/5" />

      {/* elemets */}
      {nodeInfo.mesh && (
        <>
          <CardBody className="grid grid-cols-3 gap-4 text-center">
            <div
              style={{
                color: attribColor.point
              }}
            >
              {t('node-graph.node-info.num-points')}
              <span className="pl-1">{nodeInfo.mesh.num_points}</span>
            </div>

            <div
              style={{
                color: attribColor.vertex
              }}
            >
              {t('node-graph.node-info.num-vertices')}
              <span className="pl-1">{nodeInfo.mesh.num_vertices}</span>
            </div>

            <div
              style={{
                color: attribColor.prim
              }}
            >
              {t('node-graph.node-info.num-prims')}
              <span className="pl-1">{nodeInfo.mesh.num_prims}</span>
            </div>
          </CardBody>
          <Divider className="bg-white/5" />
        </>
      )}

      {/* attribs */}
      <CardBody>
        {nodeInfo.mesh ? (
          <>
            <div className="opacity-50">{t('node-graph.node-info.attributes')}</div>
            <div className={cn('grid gap-4', `grid-cols-${attribData.length}`)}>
              {attribData.map((item) => {
                return (
                  <div
                    className="col-span-1"
                    style={{
                      color: attribColor[item.cls]
                    }}
                    key={item.cls}
                  >
                    <div className="text-center font-bold">
                      {t(`node-graph.node-info.${item.cls}` as any)}
                    </div>

                    <Divider className="bg-white/5" />

                    {item.attribs.map((attrib) => {
                      return (
                        <Tooltip
                          className="rounded bg-menu-bg"
                          delay={300}
                          closeDelay={50}
                          content={
                            <div className="flex flex-col  gap-1">
                              <div>
                                {t('node-graph.node-info.major')}: {attrib.major_version}
                              </div>
                              <div>
                                {t('node-graph.node-info.minor')}: {attrib.minor_version}
                              </div>
                              {attrib.compressed && (
                                <div>{t('node-graph.node-info.compressed')}</div>
                              )}
                              {attrib.shared && <div>{t('node-graph.node-info.shared')}</div>}
                            </div>
                          }
                          placement="left"
                          key={attrib.name}
                          isDisabled={true} // TODO: fix nesting tooltips
                        >
                          <div className="text-center">
                            {attrib.name}
                            <span className="pl-0.5 font-extralight text-text-primary/30 ">
                              (
                              <span className="text-text-primary/80">
                                {convertAttribType(attrib.type)}
                              </span>
                              )
                            </span>
                          </div>
                        </Tooltip>
                      )
                    })}
                  </div>
                )
              })}
            </div>
          </>
        ) : (
          <div className=" self-center">{t('node-graph.node-info.no-data')}</div>
        )}
      </CardBody>

      {/* logs */}
      {nodeInfo.node.logs && (
        <>
          <Divider className="bg-white/5" />
          <CardBody>
            <div className="flex flex-col">
              {nodeInfo.node.logs.map((log) => {
                return (
                  <div key={log.level}>
                    <div className="grid grid-cols-4 gap-4">
                      <label
                        className="col-span-1 self-center text-right  font-bold"
                        style={{
                          color: logColor[log.level]
                        }}
                      >
                        {t(logName[log.level] as any)}
                      </label>
                      <div className="col-span-3">
                        {log.message.map((msg, idx) => {
                          return (
                            <li
                              className="max-w-80 text-pretty"
                              style={{
                                color: logColor[log.level]
                              }}
                              key={idx}
                            >
                              {msg}
                            </li>
                          )
                        })}
                      </div>
                    </div>
                  </div>
                )
              })}
            </div>
          </CardBody>
        </>
      )}

      <Divider className="bg-white/5" />

      <CardFooter className="gap-1 rounded-lg text-gray-400">
        {t('node-graph.node-info.execution-time')}:
        <span className="font-bold text-text-primary">{nodeInfo.node.execution_time / 1000.0}</span>
        {t('time.ms')}
      </CardFooter>
    </Card>
  )
}

const nameTable: Record<string, string> = {
  f32: 'float',
  f64: 'double',
  i32: 'int',
  i64: 'int64',
  u32: 'uint',
  u64: 'uint64',
  str: 'string'
}

const sizeTable: Record<string, string> = {
  '2': 'vec2',
  '3': 'vec3',
  '4': 'vec4',
  '9': 'mat3',
  '16': 'mat4'
}

const vecTable: Record<string, string> = {
  f32: '',
  f64: 'd',
  i32: 'i',
  i64: 'i64',
  u32: 'u',
  u64: 'u64'
}

const cache: Record<string, string> = {}

const convertAttribType = (name: string): string => {
  let result = cache[name]
  if (result !== undefined) {
    return result
  }

  if (name.endsWith('[]')) {
    result = convertAttribType(name.substring(0, name.length - 2)) + '[]'
  } else if (name.endsWith(']')) {
    let offset = name.length - 2
    let vec = sizeTable[name.substring(offset, name.length - 1)]
    if (!vec) {
      --offset
      vec = sizeTable[name.substring(offset, name.length - 1)]
    }
    if (!vec) {
      result = name
    } else {
      const base = vecTable[name.substring(0, offset - 1)]
      if (base === undefined) {
        result = vec + name.substring(0, offset - 1)
      } else {
        result = vec + base
      }
    }
  } else if (name.endsWith('*')) {
    result = convertAttribType(name.substring(0, name.length - 1)) + '*'
  } else {
    result = nameTable[name] || name
  }

  cache[name] = result
  return result
}

// console.log(convertAttribType('f64[9]'))
// console.log(convertAttribType('str[]'))
// console.log(convertAttribType('f32[3][]'))
// console.log(convertAttribType('f32[16][]'))
// console.log(convertAttribType('edge_group'))
// console.log(convertAttribType('f64*'))
