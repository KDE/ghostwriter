## Introduction

Welcome to *ghostwriter*!  This quick reference guide gives examples of writing in Markdown, a plain text markup format created by John Gruber.  For more information, please visit John Gruber's website at <http://www.daringfireball.net>.

*ghostwriter* has built-in support for the cmark-gfm processor.  However, it also can auto-detect Pandoc, MultiMarkdown, or cmark processors.  Simply install any of these processors and ensure that their installation locations are added to your system's `PATH` environment variable.  *ghostwriter* will auto-detect their installation on startup, and give you live HTML preview and export options accordingly.

## Headings

    # Level 1 Heading
    ## Level 2 Heading ##
    ###### Level 6 Heading
    
    Level 1 Heading
    ===============
    some text
    
    Level 2 Heading
    ---------------
    some more text

## Paragraphs

    This is a paragraph.
    This is another line in the same paragraph using a manual line break.
    
    This is a new paragraph.

## Inline Formatting

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

## Links

You can link to various websites:

    Click [here](http://url.com "Title") to visit a website.

You can also use reference links:

    [This][id] is a reference link which is defined below.
    
    [id]: http://url.com "Title"

## Images

You can embed images like so:

    ![alternate text](./image.jpg "Title")

You can also use the same syntax as with reference links:

    ![alternate text][id] for a reference defined below.
    
    [id]: ./image.jpg "Title"

## Lists

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

## Code Blocks

You can indent with four or more spaces or a tab character to create a code block:

    This is a normal paragraph, followed by a code block.
        
        for (int i = 0; i < 10; i++) {
            System.out.println("i = " + i);
        }
        
    The above will be displayed as a preformatted block of code.

In some Markdown converters, such as Pandoc or GitHub, you can use GitHub-Flavored Markdown's backticks to delimit code blocks instead of indenting the block, like so:

    ```
    for (int i = 0; i < 10; i++) {
        System.out.println("i = " + i);
    }
    ```

The above will likewise be displayed as a preformatted block of code.

## Code Spans

You can use backticks in the middle of a paragraph to delimit code, as with this `` `<html>` `` tag.  You can use literal backticks by using more than one backtick, such as with this example:  ``` `` `escaped backtick` `` ```.

## Blockquotes

You can use e-mail-style angle brackets to delimit block quotes, as follows:

    > This is a block quote.
    >
    >> This is a nested block quote.

## Horizontal Rules

Use three or more dashes (\-), underscores (\_), or asterisks (\*) for horizontal rules, like so:

    ---
    
    ***
    ___

You can even have spaces between each character:

    - - -

## Markdown Extensions

Some Markdown processors add syntax for tables, footnotes, and so forth to extend the original John Gruber Markdown specification.  Below is a footnote usage example for Pandoc, MultiMarkdown, and other processors that support the extension:

    Here is the body text that has a footnote.[^footnoteid]
    
    [^footnoteid]:  Here is the footnote text.

Below is a sample Pandoc table:

    Column One     Column Two
    ----------     ----------
    Row Text 1     Row Text 2
    Row Text 1     Row Text 2
    
    Table:  Pandoc sample table.

Below is a sample cmark-gfm or MultiMarkdown table:

    Column One
:    Column Two
    ----------
:    ----------
    Row Text 1
:    Row Text 2
    Row Text 1
:    Row Text 2

There are even more table syntaxes that these Markdown processors can support.  There are also many more extensions.  For more information on these processors, please visit the following websites:

+ Pandoc: <http://johnmacfarlane.net/pandoc/>
+ MultiMarkdown: <http://fletcherpenney.net/multimarkdown/>
+ cmark: <http://github.com/jgm/cmark/>
+ cmark-gfm: <http://github.com/github/cmark>

The original John Gruber Markdown specification can be found at <http://daringfireball.net/projects/markdown/>.

## Shortcuts #

*ghostwriter* supports the following shortcuts for editing Markdown:


<kbd>Ctrl</kbd>`+`<kbd>B</kbd>
:   Applies bold (strong) formatting.

<kbd>Ctrl</kbd>`+`<kbd>I</kbd>
:   Applies italic (emphasis) formatting.

<kbd>Ctrl</kbd>`+`<kbd>K</kbd>
:   Applies strikethrough formatting.

<kbd>Ctrl</kbd>`+`<kbd>/</kbd>
:   Inserts an HTML comment at the cursor position if no text is selected, or else will turn selected text into an HTML comment.

<kbd>Tab</kbd>
:   Indents the selected lines of text.

<kbd>Shift</kbd>`+`<kbd>Tab</kbd>
:   Unindents the selected lines of text.

<kbd>Shift</kbd>`+`<kbd>Enter</kbd>
:   Inserts a Markdown-style line break.

<kbd>Ctrl</kbd>`+`<kbd>Enter</kbd>
:   Inserts a line break as usual, except the current indentation will not be honored.  In other words, if you are typing on an indented line and press `Ctrl+Enter`, no indentation will be added to the new line.  Note that the Ctrl modifier can be combined with the Shift modifier to produce both an unindented line and a Markdown-style line break.

<kbd>Ctrl</kbd>`+`<kbd>.</kbd>
:   Turns the current line or the selected lines into a block quote.

<kbd>Ctrl</kbd>`+`<kbd>,</kbd>
:   Removes any block quote formatting from the current line or the selected lines of text.

<kbd>Ctrl</kbd>`+`<kbd>8</kbd>
:   Turns the current line or the selected lines into a bullet list with the `*` marker.  For example, if you have selected text within the following lines:

        List item one.
        List item two.
        
    Pressing this shortcut key will turn the selected lines into a bullet list like so:

        * List item one.
        * List item two.

<kbd>Ctrl</kbd>`+`<kbd>Shift</kbd>`+`<kbd>-</kbd>
:   Turns the current line or the selected lines into a bullet list with the `-` marker.  For example, if you have selected text within the following lines:

        List item one.
        List item two.
        
    Pressing this shortcut key will turn the selected lines into a bullet list like so:

        - List item one.
        - List item two.

<kbd>Ctrl</kbd>`+`<kbd>Shift</kbd>`+`<kbd>=</kbd>
:   Pressing `Ctrl` and `Shift` with the `=` character turns the current line or the selected lines into a bullet list with the `+` marker.  For example, if you have selected text within the following lines:

        List item one.
        List item two.
        
    Pressing this shortcut key will turn the selected lines into a bullet list like so:

        + List item one.
        + List item two.

<kbd>Ctrl</kbd>`+`<kbd>1</kbd>
:   Turns the current line or the selected lines into a numbered list with the `.` marker.  For example, if you have selected text within the following lines:

        List item one.
        List item two.
        
    Pressing this shortcut key will turn the selected lines into a numbered list like so:

        1. List item one.
        2. List item two.

<kbd>Ctrl</kbd>`+`<kbd>0</kbd>
:   Turns the current line or the selected lines into a numbered list with the `)` marker.  For example, if you have selected text within the following lines:

        List item one.
        List item two.
        
    Pressing this shortcut key will turn the selected lines into a numbered list like so:

        1) List item one.
        2) List item two.

<kbd>Ctrl</kbd>`+`<kbd>T</kbd>
:   Turns the current line or the selected lines into a GitHub-Flavored Markdown task list item.  For example, if you have selected text within the following lines:

        Important thing to do.
        Another important thing to do.
        
    Pressing this shortcut key will turn the selected lines into a task list like so:

        - [ ] Important thing to do.
        - [ ] Another important thing to do.

<kbd>Ctrl</kbd>`+`<kbd>D</kbd>
:   When the current line of text is a GitHub-Flavored Markdown task list item, pressing this shortcut key will toggle the completion status of the task, placing an `x` in the checkbox if the task is complete, or removing the `x` if it is incomplete.  For example:

        - [ ] This is an incomplete task.
        - [x] This task as done!

Matched Characters
:   When text within a single line is selected, pressing any of the above characters will enclose the selected text with that character and its matching closing character.  For example, if you select the word `dog` and then enter the `(` character, the word `dog` will be enclosed in parentheses and become `(dog)`.  Below is a table showing the typed character and the matching closing character:
    
    Opening | Closing
    --------|--------
    `"`     | `"`
    `'`     | `'`
    `(`     | `)`
    `[`     | `]`
    `{`     | `}`
    `*`     | `*`
    `_`     | `_`
    `` ` `` | `` ` ``
    `<`     | `>`
    
    To enable auto-matching of characters as you type, check "Automatically match characters while typing" from the Editor tab in the Preferences dialog, available from the menu bar under the `Settings->Preferences...` option.  You can also customize which characters are matched by selecting the "Customize matched characters..." button.

Drag and Drop Image Files
:   An alternative to selecting the `Edit->Insert Image...` option from the menu bar is to drag and drop any image file from your file system into the editor window.  The application will insert inline image text with your image's file path relative to the currently opened document's location, or the full path if the document is untitled.  For example:

        ![](../../relative/path/to/your/file.png)

    or if the document is untitled:

        ![](file:///full/path/to/your/file.png)