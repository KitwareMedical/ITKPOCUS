# Style and Developer's Guide

## General guidelines
1. Docstrings are in numpy format [https://numpydoc.readthedocs.io/en/latest/format.html](https://numpydoc.readthedocs.io/en/latest/format.html)
2. Every method gets a docstring
2. No human data or other sensitive data may be included in pull requests without explicit permission from the ITKPOCUS authors.
3. Tests are required using `unittest`.  Small tests images may be included in [tests/data](tests/data).

## Github
1. Create your own fork of itkpocus
2. Create a new branch
3. Commit your new branch
4. Open a pull request

## New device support
Contributions for other device manufacturers should follow the template of existing devices.
1. Create an issue on the the Github issue tracker
2. Create a `manufacture_name.py`
3. Write `load_and_preprocess_` methods that return an `itk.Image[itk.F,]` with intensity scaled to 0.0 to 1.0 and physical dimensions set.
4. Adding a `version` parameter to the `load_and_preprocess_` methods for compatibility.

## Commit messages
Write your commit messages using these standard prefixes:

* BUG: Fix for runtime crash or incorrect result
* COMP: Compiler error or warning fix
* DOC: Documentation change
* ENH: New functionality
* PERF: Performance improvement
* STYLE: No logic impact (indentation, comments)
* WIP: Work In Progress not ready for merge

The body of the message should clearly describe the motivation of the commit (what, why, and how). In order to ease the task of reviewing commits, the message body should follow the following guidelines:

* Leave a blank line between the subject and the body. This helps git log and git rebase work nicely, and allows to smooth generation of release notes.
* Try to keep the subject line below 72 characters, ideally 50.
* Capitalize the subject line.
* Do not end the subject line with a period.
* Use the imperative mood in the subject line (e.g. BUG: Fix spacing not being considered.).
* Wrap the body at 80 characters.
* Use semantic line feeds to separate different ideas, which improves the readability.
* Be concise, but honor the change: if significant alternative solutions were available, explain why they were discarded.
* Reference an existing issue by number in the commit (e.g. `Fixes issue #1234`)