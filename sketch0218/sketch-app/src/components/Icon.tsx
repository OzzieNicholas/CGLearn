import { IconBaseProps, IconType } from 'react-icons'

const Icon = ({
  content,
  className,
  ...props
}: { content: string; className?: string } & IconBaseProps) => {
  return (
    <img
      src={content}
      style={{ width: props.size || props.width, height: props.size || props.height }}
      className={className}
    />
  )
}

export default Icon

export function MakeIcon({ content }: { content: string }): IconType {
  const GeneratedIcon = (props: IconBaseProps) => {
    return <Icon content={content} {...props} />
  }
  GeneratedIcon.displayName = 'GeneratedIcon'
  return GeneratedIcon
}
