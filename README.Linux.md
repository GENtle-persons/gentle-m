# GENtle on Linux

GENtle is available as a package for Debian and Ubuntu since 2010. You can easily install GENtle on your Linux distribution using the package manager. For Debian-based systems, you can find the exact version of GENtle available for your release and backport distribution channels on the [GENtle Debian tracker page](https://tracker.debian.org/pkg/gentle).

## Installation

To install GENtle on Debian or Ubuntu, open a terminal and run the following command:

```bash
sudo apt-get install gentle
```

This command will install the GENtle package along with all its dependencies.

## Running GENtle

After installation, you can start the application by typing `GENtle` in the UNIX command line, or you can find GENtle in the "Science" submenu of your application menu.

## Compiling from Source

If you prefer to compile GENtle from source on Debian or Ubuntu independently from the package maintainers' routines, follow these steps:

1. Install the required packages:

```bash
sudo apt-get install libwxgtk3.2-dev automake1.9 libmysql++-dev libsqlite0-dev libsqlite3-dev
```

2. Perform the usual compilation steps:

```bash
./configure
make
```

## Note for Other Linux Distributions

For users of other Linux distributions, you may need to slightly adjust the package names of the build dependencies mentioned above based on your distribution's package manager. Refer to your distribution's documentation or package repository for appropriate package names.

## Contributing to Packaging

If you are familiar with packaging for Linux distributions and would like to contribute to the packaging of GENtle for any Linux variant, we welcome your assistance. Feel free to reach out to us to discuss how you can help.

We hope you find GENtle valuable for your molecular biology research and experimentation.
If you encounter any issues or have questions, please don't hesitate to contact us for support.
