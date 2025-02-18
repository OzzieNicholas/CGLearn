import { logColor } from '@/constant/log'
import { NodeData } from '@/types/node-graph'

export default function NodeMessage({ data }: { data: NodeData }) {
  return (
    <>
      {data.logs ? (
        <div>
          {data.logs.map((log) => {
            return (
              <div key={log.level} className="flex gap-1">
                {log.message.map((msg, idx) => {
                  return (
                    <div
                      className="truncate"
                      style={{
                        color: logColor[log.level]
                      }}
                      key={idx}
                    >
                      {msg}
                    </div>
                  )
                })}
              </div>
            )
          })}
        </div>
      ) : (
        <span>No message</span>
      )}
    </>
  )
}
