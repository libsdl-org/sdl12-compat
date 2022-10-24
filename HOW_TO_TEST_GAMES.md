# Testing games with sdl12-compat

sdl12-compat is here to make sure older games not only work well, but work
_correctly_, and for that, we need an army of testers.

Here's how to test games with sdl12-compat.


## The general idea

- Build [SDL2](https://github.com/libsdl-org/SDL) or find a prebuilt binary.
- Build sdl12-compat (documentation is in README.md)
- Find a game to test.
- Make sure a game uses sdl12-compat instead of classic SDL 1.2
- See if game works or blows up, report back.


## Find a game to test

We are keeping a spreadsheet of known games that still use SDL 1.2
[here](https://docs.google.com/spreadsheets/d/1u8Rq3LVQYYgu28sBuxrZ371QolbiZu5z_LjENc4ddZs/edit?usp=sharing).

Find something that hasn't been tested, or hasn't been tested recently, and
give it a try! Then update the spreadsheet.

Extra credit if you [file bug reports](https://github.com/libsdl-org/sdl12-compat/issues)
but we're grateful if you just make notes on the spreadsheet, too.


## Make sure the game works with real SDL 1.2 first!

You'd be surprised how many games have bitrotted! If it doesn't work with
real 1.2 anymore, it's not a bug if sdl12-compat doesn't work either. That
being said, lots of games that stopped working _because_ of SDL 1.2 will
now work again with sdl12-compat, which is nice, but just make sure you have
a baseline before you start testing.


## Force it to use sdl12-compat instead.

Either overwrite the copy of SDL-1.2 that the game uses with sdl12-compat,
or (on Linux) export LD_LIBRARY_PATH to point to your copy, so the system will
favor it when loading libraries.

## Watch out for setuid/setgid binaries!

On Linux, if you're testing a binary that's setgid to a "games" group (which
we ran into several times with Debian packages), or setuid root or whatever,
then the system will ignore the LD_LIBRARY_PATH variable, as a security
measure.

The reason some games are packaged like this is usually because they want to
write to a high score list in a global, shared directory. Often times the
games will just carry on if they fail to do so.

There are several ways to bypass this:

- On some distros, you can run `ld.so` directly:
  ```bash
  LD_LIBRARY_PATH=/where/i/can/find/sdl12-compat ld.so /usr/games/mygame
  ```
- You can remove the setgid bit:
  ```bash
  # (it's `u-s` for the setuid bit)
  sudo chmod g-s /usr/games/mygame
  ```
- You can install sdl12-compat system-wide, so the game uses that
  instead of SDL 1.2 by default.
- If you don't have root access at all, you can try to copy the game 
  somewhere else or install a personal copy, or build from source code,
  but these are drastic measures.
  
Definitely read the next section ("Am I actually running sdl12-compat?") in
these scenarios to make sure you ended up with the right library!
  
## Am I actually running sdl12-compat?

The easiest way to know is to set some environment variables:

```bash
export SDL12COMPAT_DEBUG_LOGGING=1
```

If this is set, when loading sdl12-compat, it'll write something like this
to stderr (on Linux and Mac, at least)...

```
INFO: sdl12-compat, built on Sep  2 2022 at 11:27:37, talking to SDL2 2.25.0
```

You can also use this:

```bash
export SDL_EVENT_LOGGING=1
```

Which will report every event SDL 2 sends to stderr:

```
INFO: SDL EVENT: SDL_WINDOWEVENT (timestamp=317 windowid=1 event=SDL_WINDOWEVENT_MOVED data1=1280 data2=674)
INFO: SDL EVENT: SDL_WINDOWEVENT (timestamp=318 windowid=1 event=SDL_WINDOWEVENT_EXPOSED data1=0 data2=0)
INFO: SDL EVENT: SDL_WINDOWEVENT (timestamp=318 windowid=1 event=SDL_WINDOWEVENT_ENTER data1=0 data2=0)
```

Since this is a new feature in SDL2, it'll only show up because sdl12-compat talks
to SDL2 and classic SDL 1.2 doesn't.


## Steam

If testing a Steam game, you'll want to launch the game outside of the Steam
Client, so that Steam doesn't overwrite files you replaced and so you can
easily control environment variables.

Since you'll be using the Steam Runtime, you don't have to find your own copy
of SDL2, as Steam provides it.

On Linux, Steam stores games in ~/.local/share/Steam/steamapps/common, each
in its own usually-well-named subdirectory.

You'll want to add a file named "steam_appid.txt" to the same directory as
the binary, which will encourage Steamworks to _not_ terminate the process
and have the Steam Client relaunch it. This file should just have the appid
for the game in question, which you can find from the store page.

For example, the store page for Braid is:

https://store.steampowered.com/app/26800/Braid/

See that `26800`? That's the appid.

```bash
echo 26800 > steam_appid.txt
```

For Linux, you can make sure that, from the command line, the game still
runs with the Steam Runtime and has the Steam Overlay by launching it with a
little script:

- [steamapp32](https://raw.githubusercontent.com/icculus/twisty-little-utilities/main/steamapp32) for x86 binaries.
- [steamapp64](https://raw.githubusercontent.com/icculus/twisty-little-utilities/main/steamapp64) for x86-64 binaries.

(And make sure you have a 32-bit or 64-bit build of sdl12-compat!)

And then make sure you force it to use _your_ sdl12-compat instead of the
system/Steam Runtime build:

```bash
export LD_LIBRARY_PATH=/where/i/installed/sdl12-compat
```

Putting this all together, you might run [BIT.TRIP Runner2](https://store.steampowered.com/app/218060/)
like this:

```bash
cd ~/.local/share/Steam/steamapps/common/bittriprunner2
export LD_LIBRARY_PATH=/where/i/installed/sdl12-compat
export SDL12COMPAT_DEBUG_LOGGING=1
echo 218060 > steam_appid.txt
steamapp32 ./runner2
```


## Windows

Generally, Windows games just ship with an SDL.dll, and you just need to
overwrite it with an sdl12-compat build, then run as usual.


## macOS, etc.

(write me.)


## Questions?

If something isn't clear, make a note [here](https://github.com/libsdl-org/sdl12-compat/issues/new)
and we'll update this document.

Thanks!

