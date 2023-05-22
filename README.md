![logo](Branding/Logo.png)

# Null-Porn
Null-Porn is a project born out of frustration with the increasing presence of pornographic and explicit content on platforms such as [YouTube](https://youtube.com)/[YouTube Shorts](https://shorts.youtube.com) and [TikTok](https://tiktok.com). These platforms have been inundated with spam bots that not only disrupt the user experience but also display offensive and inappropriate material. In response to this issue, I have taken the initiative to create a set of scripts that aim to mitigate the problem, including a DDoS flood script and various DNS tracker scripts.

## Project Structure

_* The project uses [premake](https://github.premake.io) as a build system_.

### Directory Hierarchy
```sh
.
├── Branding      # Resources used for the github repository
├── Vendor        # External Dependencies
├── NullPorn-Core # Core Files and API, contains the attacks
├── NullPorn-CLI  # CLI frontend for <NullPorn-Core>
├── NullPorn-GUI  # GUI frontend for <NullPorn-Core>
├── LICENSE
├── README.md
└── premake5.lua  # Build script and Project configuration
```

### Compatibility

This project is primarily optimized for Unix-based systems. It leverages the `netinet` C standard socket library, which is commonly available on Unix systems. While it may also work on other operating systems, it is recommended to use a Unix system for the best compatibility and performance.

### External Dependencies

The command line interface is implemented using [newt](https://pagure.io/newt), a popular library for creating text-based user interfaces. It provides a rich set of functionalities for creating interactive command line interfaces. Some core functionality is facilitated by the use of the [stb](https://github.com/nothings/stb) header.

The graphical user interface is built using OpenGL/[GLEW](https://glew.sourceforge.net/) and [GLFW](https://glfw.org) as a windowing system. It provides a visually appealing and interactive user interface. Additionally, the UI components are implemented using [Nuklear](https://immediate-mode-ui.github.io/Nuklear/doc/index.html), a lightweight immediate mode GUI library.

## Screenshots



![cli](Branding/Screenshot-CLI.png)

*The CLI frontend*

## Disclaimer

Although Null-Porn is an open-source project and its repository is publicly available, it is crucial to use this software responsibly and ethically. The purpose of this project is solely educational, intended to shed light on the techniques involved and the potential vulnerabilities of certain systems. **Under no circumstances should this project be used for any malicious or harmful activities.** Any misuse or unethical use of the scripts is strictly prohibited.

Please respect the principles of responsible usage and ensure that you adhere to legal and ethical standards when engaging with this project.
