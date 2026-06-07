# myshell

A Unix shell written in C.

- Command execution via `fork` and `execvp`
- `cd` builtin with home directory support
- Pipe support (`ls | grep .c`)
- Command history with up/down arrow navigation (via readline)
- Dynamic prompt showing current working directory

## Build
```bash
gcc shell.c -o myshell -lreadline -lncurses
```

## Usage
```bash
./myshell
```