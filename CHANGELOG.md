# 📜 Changelog

## [1.0.0] - 2025-09-22
### Add
- Load `.env` files securely and easily
- recovery of environment variables (any type): int, float, std::string, etc.
- Header-only : just include and use
- Simple API for accessing environment variables

## [1.1.0] - 2025-08-03
### Add
- Added string operator overload

## [1.2.0] - 2025-10-18
### Add

- added operator overloading [] for all types
- added string_to_bool function

## [1.2.1] - 2025-11-02
### Fixed

- (Windows, Linux) Correct handling of Unicode filenames in `load_from_file`
  - Fixed compatibility with MinGW and MSVC using `_wfopen` on Windows.
  - Ensures `.env` files with names like `文件名.env` load correctly.


