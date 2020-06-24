# cli-typer

CLI typer is a generic scripting tool to allow scripting (and a bit more) on
CLIs that don't accept scripting!

## Usage

CLI typer needs a config file to run:
`./cli-typer <config-file>`

The config file is divided is two section: 1 Generic configurations and the
actions to be executed. For example:
```
cfg.command: sh test.sh
cfg.exitTimeoutS: 3
cfg.actionDelayMs: 200

waitFor: something
sleep: 2
type: list
question:
type: exit
```

### Configuration section
- `cfg.command`: the command to be run
- `cfg.exitTimeoutS`: the amount of time (in seconds) to wait before force quiting the command
- `cfg.actionDelayMs`: the amount of time (in milliseconds) to wait between actions

### Supported actions

Each action consists in a name and parameter separated by `:`.

- `waitFor: <string>`: Wait for the `<string>` on the output of `cfg.command` for 10s (This still needs to be configurable)
- `sleep: <time in seconds>`: Sleep for `<time in seconds>`
- `type: <string>`: Type `<string>` on the input of `cfg.command`
- `question:`: Stop and request user input

## Building

The pre-requisite to build cli-typer is to have boost installed, most versions will work out of the box.

Recommended steps to build:
```
$ mkdir out
$ cd out
$ cmake .. && make
```

## Runnning the example

To run the example, first build cli-typer using the steps above and then:
```
$ cd example
$ ../out/cli-typer file.cfg
```
