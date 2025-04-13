# IDKBrowser
some browser project i made 3 years ago that i today revived


> [!WARNING]
> This Browser isn't meant to be a professional project nor it being a browser like firefox
> so if you break something just an update won't work


> ## Absurds that have gotten in my way
> - fixed the whole browser by adding downloads.json and history.json because without them the browser doesnt work

## features
it has things like
- downloads manager
- history manager
- tracker blocking (i think)
- just works

### if download history and browsing history doesnt work create files "downlods.json" and "history.json" because they could be deleted or corrupted, create them in the main directory and add '[]' without anything else, the browser will do its thing

# how to compile + run

```sh
cmake CMakeLists.txt && make && QTWEBENGINE_DISABLE_SANDBOX=1 ./WebBrowser
```
