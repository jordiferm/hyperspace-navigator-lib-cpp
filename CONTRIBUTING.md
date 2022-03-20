# Contributing

Thanks for getting involved and contributing to the Space Navigator library Below are a few things to setup when submitting a PR.

## Code comments

If adding new code, be sure to include relevant code comments. Code comments are a great way for others to learn from your code. 

## Update Documentation

Be sure to update any documentation relevant to your change. This includes updating the [CHANGELOG.md](./CHANGELOG.md).

## Code Formatting

We use [this script](/scripts/format.sh#L20) to install a consistent version of [`clang-format`](https://clang.llvm.org/docs/ClangFormat.html) to format the code base. The format is automatically checked via a Travis CI build as well. Run the following script locally to ensure formatting is ready to merge:

    make format

We also use [`clang-tidy`](https://clang.llvm.org/extra/clang-tidy/) as a C++ linter. Run the following command to lint and ensure your code is ready to merge:

	make tidy

These commands are set from within [the Makefile](./Makefile).