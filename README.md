
# Kilo-Word-Completion

An extension of the Kilo text editor to feature prefix and fuzzy match word completion as well as LaTeX specific editing additions.




## Features

- Prefix match word completion
- Fuzzy match word completion
- Language specific word completion
- Word completion engine visualisation


## Requirements

- The base text editor only requires a Linux terminal
- Tests require CUnit which can be installed using: `sudo apt-get install libcunit1 libcunit1-doc libcunit1-dev`
## Installation

1. Clone the repository and navigate into it.

```bash
  git clone https://github.com/4v0n/kilo-word-completion.git
  cd kilo-word-completion
```

2. Build the project.

```bash
  make main #builds the text editor
  make debug #builds the text editor with debug flag
  make tests #builds tests
```

3. Navigate into the build directory and run the text editor.

```bash
  cd build
  ./kilo
```

To open a file using the text editor:

```bash
  ./kilo [PATH TO FILE]
```
## Running Tests

To run tests, from the main directory, build the tests, navigate to the build folder and run the tests.

```bash
  make tests
  cd build
  ./tests
```

Note: Due to the nature of this program, unit tests alone aren't enough to verifying the functionality of the progam.
## Data Source

This project utilises the ["English Word Frequency"](https://www.kaggle.com/datasets/rtatman/english-word-frequency/data) dataset obtained from Kaggle. The dataset was originally compiled and published by Rachael Tatman.