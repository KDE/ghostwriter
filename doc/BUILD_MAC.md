Build ghostwriter on Mac without installing XCode
=================================================


### 1. Acknowledgements & License

This guide is derived from [Qt without Xcode how-to][gist] by Peter Jonas ([shoogle]) / [CC BY 4.0][CC-BY]

[gist]: https://gist.github.com/shoogle/750a330c851bd1a924dfe1346b0b4a08 "GitHub gist"
[shoogle]: https://github.com/shoogle "shoogle's GitHub user profile"

[![Creative Commons License][CC-BY-image]][CC-BY]

This work is licensed under a [Creative Commons Attribution 4.0 International License][CC-BY].

[CC-BY]: http://creativecommons.org/licenses/by/4.0/ "View license text on the Creative Commons website"
[CC-BY-image]: https://i.creativecommons.org/l/by/4.0/88x31.png "CC BY"


### 2. Introduction. Why is this needed.

Older Qt versions (<5.9.1) try to use `xcodebuild` to find out where the utilities are installed.
This fails with the following error message unless Xcode is installed:

> Project ERROR: Could not resolve SDK Path for 'macosx'
> Error while parsing file `<filename.pro>`. Giving up.

You need to tell QMake to use `xcrun` instead.


### 3. Configure Qt 5.5 to use `xcrun` instead of `xcodebuild`

Open a Terminal and change directory to where brew installed target mkspecs files.

  ```bash
  cd /usr/local/Cellar/qt@5.5/5.5.1_1/mkspecs/features/mac

  ```

Now copy and paste these new commands **into the same Terminal window**
to create backup copies of some files:

  ```bash
  function backup_files() {
    for file in "$@"; do
      [[ -f "${file}.backup" ]] || cp "${file}" "${file}.backup"
    done
  }
  backup_files sdk.prf default_pre.prf default_post.prf
  ```
Press the Return key after the final command.

If that all went OK then run these commands to update the files:

  ```bash
  url_base='https://raw.githubusercontent.com/qt/qtbase'
  commit='fa7626713b3a943609453459190e16c49d61dfd3'
  dir='mkspecs/features/mac'
  curl -O "${url_base}/${commit}/${dir}/sdk.prf"
  curl -O "${url_base}/${commit}/${dir}/default_pre.prf"
  old_line='cache(QMAKE_XCODE_VERSION, stash)'
  new_line='!isEmpty(QMAKE_XCODE_VERSION): cache(QMAKE_XCODE_VERSION, stash)'
  sed "s|^${old_line}\$|${new_line}|" <default_post.prf.backup >default_post.prf
  ```

#### All done, please go back to the [main README](../README.md) and continue following the building steps for MacOS.
