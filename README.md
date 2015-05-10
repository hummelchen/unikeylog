# Unikeylog - multifunctional keylogger for Linux systems

## Features:
 - X11 key logging (doesn't require root)
 - /dev/input key logging (requires root)
 - encryption of log
 - hides itself

## Dependencies
 - C compiler GCC
 - Xlib
 - openssl (library + command line tool)
 - LuaJIT

## Build

Run `luajit build.lua` and follow the instructions.

## Usage

After you run `luajit build.lua`, see file `src/keylogger.exe`.
This application is the keylogger. It runs silently logging to
the logfile specified at the compile time (option
`KEYLOGGER_LOGFILE`). Current date and time are appended to the
name of logfile.

To read logfile, `cd` to directory `src/` and run
`luajit parser.lua < logfile`.

## Example

Build for X11:
```bash
$ KEYLOGGER_MODE=X11 lua build.lua
```

Run:
```bash
$ ./keylogger.exe
```

Parse log:
```bash
$ luajit parser.lua < log.Sun_May_10_08-51-01_2015
ls
<Shift_R>This keylogger runs without rot<BackSpace>ot access,
but in can intercept root or sudo password
su
123456
<Shift_R>Also, we can yo<BackSpace><BackSpace>
use other layouts, for example, <Shift_R>Russian.
<Shift_R>Проверка работы русской раскладки.
```

## Legal

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
