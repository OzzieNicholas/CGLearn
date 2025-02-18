import {
  collectMemory,
  toggleConsoleWindow,
  openProfiler,
  setMemoryProfiling,
  openTracyProfiler,
  openImageViewer,
  openDevtools,
  connectToServer,
  disconnectFromServer,
  getServerDefaultPort
} from '@/commands'
import { Command } from '@/types/command'
import { exportGraphDebugInfo } from '@/utils/debug'

export const defaultCommands: Command[] = [
  {
    label: 'command.export-graph-debug-info',
    action: exportGraphDebugInfo
  },
  {
    label: 'command.collect-memory',
    action: () => collectMemory(true)
  },
  {
    label: 'command.toggle-console-window',
    action: () => toggleConsoleWindow()
  },
  {
    label: 'command.open-performance-profiler',
    action: () => openProfiler()
  },
  {
    label: 'command.open-tracy-profiler',
    action: () => openTracyProfiler()
  },
  {
    label: 'command.enable-memory-profiling',
    action: () => {
      setMemoryProfiling(true)
    }
  },
  {
    label: 'command.disable-memory-profiling',
    action: () => {
      setMemoryProfiling(false)
    }
  },
  {
    label: 'command.open-image-viewer',
    action: () => openImageViewer()
  },
  {
    label: 'command.open-devtools',
    action: () => openDevtools()
  },
  {
    label: 'command.connect-to-server',
    action: async () => {
      const result = await connectToServer(await getServerDefaultPort())
      console.log(`connect to server result: ${result}`)
    }
  },
  {
    label: 'command.disconnect-from-server',
    action: async () => {
      const result = await disconnectFromServer()
      console.log(`disconnect from server result: ${result}`)
    }
  }
]
