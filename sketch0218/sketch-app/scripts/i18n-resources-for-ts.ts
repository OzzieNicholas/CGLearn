import * as Bun from 'bun'

const src_path = Bun.argv[2]
const dst_path = Bun.argv[3]

async function main() {
  const src = await Bun.file(src_path).text()
  const dst = src
    .replace('export default', 'export default interface Resources')
    .replaceAll(',', '')
  await Bun.write(dst_path, dst)
}

main()
