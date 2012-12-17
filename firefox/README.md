## About

Ancho implementation for hosting Google Chrome extensions in Firefox browser.

## How to use Ancho for Firefox

The target Google Chrome extension is referenced using `chrome-ext` symlink in
this `firefox` directory. Point it to the root of the unpacked extension
directory structure, i.e. where the `manifest.json` file is located.

To use the extension, create a file named `ancho@salsitasoft.com` in
`extensions` directory of your Firefox browser profile with path to this
`firefox` directory as its content. Or, simply create a standard Firefox
extension (the `.xpi` extension file) by zipping content of this `firefox`
directory.  When doing so, make sure the content of referenced `chrome-ext`
directory is included in the package, i.e. that the symbolic links are followed
(this is default behavior of `zip` utility in Linux and OS X, so you can create
the xpi archive using `zip -r ../<name>-<version>.xip *` command).

## Current state

The known limitations of the implementation are:

1. The Google Chrome extension API is far from being implemented completely, so
far we have:
  * `chrome...`,
  * `chrome...`,
  * TODO

We work hard to get the Chrome extension API implemented soon.
