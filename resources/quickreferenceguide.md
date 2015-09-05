# Introduction

Welcome to *ghostwriter*!  This quick reference guide gives examples of writing in Markdown, a plain text markup format created by John Gruber.  For more information and help, please visit John Gruber's website at <http://www.daringfireball.net>.  The official *ghostwriter* website can be found at <http://www.github.com/wereturtle/ghostwriter>.

*ghostwriter* has built-in support for the Sundown processor.  However, it also can auto-detect Pandoc, MultiMarkdown, or Discount processors.  To use any or all of the latter three, simply install them and ensure that their installation locations are added to your system's `PATH` environment variable.  *ghostwriter* will auto-detect their installation on startup, and give you live HTML preview and export options accordingly.

# Headings

    # Level 1 Heading
    ## Level 2 Heading ##
    ###### Level 6 Heading
    
    Level 1 Heading
    ===============
    some text
    
    Level 2 Heading
    ---------------
    some more text

# Paragraphs

    This is a paragraph.
    This is another line in the same paragraph using a manual line break.
    
    This is a new paragraph.

# Inline Formatting

You can do *italics* or even **bold** in two ways.  The first is by surrounding the text with asterisks (\*), while the second is with underscores (\_).

    This is *italics* with asterisks.
    This is _italics_ with underscores.
    
    This is **bold** with asterisks.
    This is __bold__ with underscores.

You can also do ***bold italics*** by mixing and matching, like so:

    This is ***bold italics*** with asterisks.
    This is **_bold italics_** mixing.
    This is another *__bold italics__*.
    etc.

# Links

You can link to various websites:

    Click [here](http://url.com "Title") to visit a website.

You can also use reference links:

    [This][id] is a reference link which is defined below.
    
    [id]: http://url.com "Title"

# Images

You can embed images like so:

    ![alternate text](./image.jpg "Title")

You can also use the same syntax as with reference links:

    ![alternate text][id] for a reference defined below.
    
    [id]: ./image.jpg "Title"

# Lists

You can use ordered lists:

    1. Item one.
    2. Item two.
    3. Item three.

Or you can use unordered lists:

    * Item one.
    + Item two.
    - Item three.

You can mix and match with nested lists:

    1. Item one.
        * Subitem one.
        * Subitem two.
    2. Item two.
        - Subitem one.
        - Subitem two.
    3. Item three.
        1. Subitem three.

# Code Blocks

You can indent with four or more spaces or a tab character to create a code block:

    This is a normal paragraph, followed by a code block.
        
        for (int i = 0; i < 10; i++) {
            System.out.println("i = " + i);
        }
        
    The above will be displayed as a preformatted block of code.

In some Markdown converters, such as Pandoc or Github, you can use Github-Flavored Markdown's backticks to delimit code blocks instead of indenting the block, like so:

    ```
    for (int i = 0; i < 10; i++) {
        System.out.println("i = " + i);
    }
    ```

The above will likewise be displayed as a preformatted block of code.

# Code Spans

You can use backticks in the middle of a paragraph to delimit code, as with this `` `<html>` `` tag.  You can use literal backticks by using more than one backtick, such as with this example:  ``` `` `escaped backtick` `` ```.

# Blockquotes

You can use e-mail-style angle brackets to delimit block quotes, as follows:

    > This is a block quote.
    >
    >> This is a nested block quote.

# Horizontal Rules

Use three or more dashes (\-), underscores (\_), or asterisks (\*) for horizontal rules, like so:

    ---
    
    ***
    ___

You can even have spaces between each character:

    - - -

# Markdown Extensions

Some Markdown processors add syntax for tables, footnotes, etc. to extend the original John Gruber Markdown specification.  Below is a footnote usage example for Pandoc, MultiMarkdown, and other processors that support the extension:

    Here is the body text that has a footnote.[^footnoteid]
    
    [^footnoteid]:  Here is the footnote text.

Below is a sample Pandoc table:

    Column One     Column Two
    ----------     ----------
    Row Text 1     Row Text 2
    Row Text 1     Row Text 2
    
    Table:  Pandoc sample table.

Below is a sample Sundown or MultiMarkdown table:

    Column One  |  Column Two
    ----------  |  ----------
    Row Text 1  |  Row Text 2
    Row Text 1  |  Row Text 2

There are even more table syntaxes that these Markdown processors can support.  There are also many more extensions.  For more information on these processors, please visit the following websites:

+ Sundown: <http://github.com/vmg/sundown/>
+ Pandoc: <http://johnmacfarlane.net/pandoc/>
+ MultiMarkdown: <http://fletcherpenney.net/multimarkdown/>
+ Discount: <http://www.pell.portland.or.us/~orc/Code/discount/>

The original John Gruber Markdown specification can be found at <http://daringfireball.net/projects/markdown/>.

# Ghostwriter Shortcuts #

*ghostwriter* supports several shortcut keys for editing Markdown:

`Tab`
:   When text is selected in the editor, pressing the tab key will indent the selected lines of text.

`Shift+Tab`
:   When text is selected in the editor, pressing the Shift and Tab keys together will unindent the selected lines of text.

`Shift+Enter`
:   Inserts a Markdown-style line break.

`>`
:   When text is selected in the editor, pressing the `>` character (`Shift+.`) will create a block quote from the selected lines of text.

`Ctrl+>`
:   When text is selected in the editor, pressing `Ctrl` with the `>` character (`Ctrl+Shift+.`) will remove any block quote formatting from the selected lines of text.

`Ctrl+D`
:   When the current line of text is a Github-Flavored Markdown task list item, pressing `Ctrl+D` will toggle the completion status of the task, placing an `x` in the checkbox if the task is complete, or removing the `x` if it is incomplete.  For example:

        - [ ] This is an incomplete task.
        - [x] `Ctrl+D` marks this task as done!

`"`, `'`, `(`, `[`, `{`, `*`, `_`, ` ` `
:   When text within a single line is selected, pressing any of the above characters will enclose the selected text with that character and its matching closing character.  For example, if you select the word `dog` and then enter the `(` character, the word `dog` will be enclosed in parentheses and become `(dog)`.

- - - -

Copyright (c) 2014, 2015 [wereturtle](http://www.github.com/wereturtle)