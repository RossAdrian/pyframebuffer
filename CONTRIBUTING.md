# Contributing to pyframebuffer

**Thank you for contributing to pyframebuffer!**

**All little pieces of contributions are welcome!**

Please follow the following guidelines to contribute to our project:

## Workspace setup

For development, I use [Visual Studio Code](https://code.visualstudio.com). Clone this project and
open it in vscode as workspace root. Make sure you have installed the *Vscode Dev Containers Extension*.
If installed, you get a notification to reopen this project in a container. If doing so, you'll get a
preconfigured development environment with all required vscode extensions and automatic code formatters
based on the projects coding style.

## Coding conventions

### Code formatting

If opened up with the projects vscode Dev Container, you must not care of code formatting. All is done
automaticly by saving!

If you're not able to use it, you can run the following commands to format the code:

* **For the C sources:**

```sh
# Make sure clang-format is installed
SRC=$(find . -name '*.[ch]')
clang-format -i $SRC
```

This formats all C source code of the project.

* **For Python sources:**

```sh
# Make sure flake8 is installed
# else run: pip install flake8
flake8 . --count --select=E9,F63,F7,F82 --show-source --statistics
flake8 . --count --exit-zero --max-complexity=10 --max-line-length=127 --statistics
```

This shows formatting violations in the Python source code of the project. You self must fix the warnings and errors.

### Naming conventions

* **In the C sources:**

Follow the following steps to generate the function name:

**1. If it is a function, that is defined in `module_pyfb.c` and is a native Python function:**

Please make sure the function is static! Additional, the function name has the prefix `pyfunc_`.

**2. General prefix:**

All functions have the prefix `pyfb_`. If it has allready the prefix `pyfunc_` from the rule above, then concatate the prefix to `pyfunc_pyfb_`.

**3. Additional `s` as prefix?**

If this function also exists as unsafe function not validating the arguments, but this function does checks the arguments and does exactly the same operation, then append to the prefix an little `s`.

For example if it allready exists a unsafe function `pyfb_drawXXX()`, and this new function is the safe version of it, it would be named `pyfb_sdrawXXX()`.

**4. The rest of the name:**

The rest of the name is a name in *camelCase*. For example `drawLine`, appended to the prefix.

**Example:**

We have a function that draws a line and is the safe version of a function that also draws a line but not validates the arguments.

**Step 1:**

It is not a direct Python function wrapper, so dismiss the `pyfunc_` prefix.

**Step 2:**

Add the `pyfb_` prefix.

**Step 3:**

It is the safe version of a function, so add the `s` to the name.

(have `pyfunc_s`)

**Step 4:**

Add a `drawLine` to the function name as it draws a line.

**Result:**

```
pyfunc_sdrawLine();
```

* **In the Python sources:**

Just use the *camelCase* convention.

## Contributing

By contributing to this project, please follow the following rules:

- by bug fixes, commit them directly to the main branch
- for features, create a new branch of the layout `Feature/<feature name>`
- please [sign all your commits with gpg](https://docs.github.com/en/authentication/managing-commit-signature-verification/signing-commits)
