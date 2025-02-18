# Nous App React

## Prerequisites

- Install [bun](https://bun.sh)
- Install [rust](https://www.rust-lang.org/tools/install)
- Install [protoc](https://grpc.io/docs/protoc-installation)

## Install frontend dependencies

`bun install`

## Windows

### Set env

1. `$env:NOUS_INSTALL_DIR=[YOUR NOUS INSTALL DIR]`
2. `$env:Path=$env:NOUS_INSTALL_DIR+';'+$env:Path`

### Build

`bun run build`

### Dev

`bun tauri dev`

## MacOS

### Set env

1. `export NOUS_INSTALL_DIR=[YOUR NOUS INSTALL DIR]`
2. `export PATH="[YOUR NOUS INSTALL DIR]:$PATH"`

### Build

`bun build`

### Dev

`bun tauri dev`

