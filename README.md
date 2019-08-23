# c-demo
Simple demo application written in C

## About
This is just a simple application written in C to bring my skills and knowledge a little bit up-to-date in this language.

The application itself does nothing spectacular: it takes an arbitary number of city-names and querys an external API for weather data for this location(s) and prints them to screen.

I have used raw sockets by intention, because I wanted to do the most by myself. The only Dependency I have included is an small "library" called [jsmn](https://github.com/zserge/jsmn) for JSON parsing and tokenizing.

## What I would change
If this where a real-world application I wuldn't use raw sockets for this case. I rather then would use **libcurl** or at least a system-call to **curl** via `popen()`. This approach is way more maintainable and less bug-prone than dealing with sockets if you don't realy have to.
