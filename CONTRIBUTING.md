# Contributing

Interested in making *ghostwriter* even better? Please read the sections below to see how you can contribute.

## Bugs Reports

Found a bug? Please report it on the GitHub issue tracker [here](https://github.com/wereturtle/ghostwriter/issues). Please be sure to fill out all answers to the questions in the bug report template.

**Warning:** Failing to provide the information required in the bug report template will result in the bug report being automatically closed.

## New Features and Bug Fixes

Do you know how to code? Follow these steps:

1. Submit an issue for your feature or bug on the GitHub issue tracker [here](https://github.com/wereturtle/ghostwriter/issues).  
   **Note:** This step is necessary to ensure that the new feature meets the project goals or that the bug isn't really a feature. You will also want to coordinate what areas of code you will be updating to avoid merge conflicts in case someone else is working on that same area of code as well.
2. After discussion for your new issue, code your feature or bug fix. Please follow the [KDE Frameworks coding style guide](https://community.kde.org/Policies/Frameworks_Coding_Style).
3. Submit a pull request against the master branch with your code updates.  
   **Important:** Please squash your commits in git before submitting the pull request!
4. Wait for a brief code review and at least two other volunteers to test your feature or bug fix.
5. Make updates to your pull request as you receive feedback.
6. Once your pull request is approved by at least two other volunteer testers, your pull request will be merged.

## Code Reviews

Do you know how to code and have an eye for detail? Volunteer to peer review new pull requests!

1. Look for a [new pull request](https://github.com/wereturtle/ghostwriter/pulls), and add a comment that you are volunteering to code review it.
2. Submit code review comments.
3. Wait for updates to the pull request (if needed).
4. Continue providing feedback until you are confident the code is sound.
5. Leave a comment with your approval of the pull request.
6. Once at least two volunteer testers submit their approval, the pull request will be merged.

Here are some tips of what to look for in code reviews:

* Does the submitted code follow the [KDE Frameworks coding style guide](https://community.kde.org/Policies/Frameworks_Coding_Style)?
* Is the code readable, with comments explaining any non-obvious lines?
* Is the code "pessimistic"? In other words, does it check for invalid values before using them and handle possible error conditions?
* Are multiple comparisons or statements wrapped in parentheses within `if` statements to prevent accidents with order of operations?  Examples:

    ```cpp
    // Wrong
    if (a == b || c > d);

    // Correct
    if ((a == b) || (c > d));
    ```
* Are literals on the left side of the `==` operator to prevent a typo with the `=` operator from escaping the compiler's notice? Examples:

    ```cpp
    // Wrong
    if (variable == 1);

    // Reason: What if there's a typo?
    //         The compiler will not catch it!
    if (variable = 1); // Oops!

    // Correct
    if (1 == variable);

    // Reason: This time the compiler will catch
    //         the typo.
    if (1 = variable); // Oops! 
    ```

## Testing

Volunteer to review code and test new features and bug fixes! Follow these steps:

1. Look for a [new pull request](https://github.com/wereturtle/ghostwriter/pulls), and add a comment that you are volunteering to test it.
2. Build the pull request on your platform.
3. Test the feature/bug fix and try to break it!
4. Provide feedback on test results in the pull request.
5. Wait for updates to the pull request (if needed).
6. Continue testing and providing feedback until you are confident the code is working.
7. Leave a comment with your approval of the pull request.
8. Once at least on other volunteer tester submits their approval, the pull request will be merged.

## Translations

With [Qt Linguist](https://doc.qt.io/qt-5/qtlinguist-index.html), you can translate ghostwriter into other languages, or update an existing translation file. The `translations/` directory within the [source code repository](https://github.com/wereturtle/ghostwriter) contains all the `.ts` files you need to get started.

## Wiki Documentation

Found a workaround for a common issue? Have any useful tips? Please document your insights on the project's [wiki](https://github.com/wereturtle/ghostwriter/wiki).

## Tech Support

Knowledgeable about Linux or building for MacOS? Know how to fix that one issue others keep running into? Your answering user questions and troubleshooting in the [GitHub issue tracker](https://github.com/wereturtle/ghostwriter/issues) would be much appreciated!

## Automated Builds

The ghostwriter project uses continuous integration for its Windows and Linux builds to provide regular releases. Sadly, this capability has not been implemented for MacOS.

If you are familiar with MacOS, please consider creating automated builds. These builds will ideally provide an installer for the application.

Also, the current Windows build does not provide an installer (`setup.exe` or `.msi`). If you are familiar with making Windows installers for Qt apps in AppVeyor, please consider contributing to the project's appveyor.yml file.
