# shsubst

Alternative to `envsubst` with support for running arbitrary shell commands.

## Syntax

`${MY_VAR}` is replaced by the contents of the environment variable `MY_VAR`.

`$(echo "Hello")` is replaced by the output of the command `echo "Hello"`.

`$$` is replaced by `$`, but escaping stray `$`s isn't required.

Inside of commands and variable names `\` will escape the followed character.

## Build

`make`

### Test

`make test`

### Install

`sudo make install`

## License

MIT
