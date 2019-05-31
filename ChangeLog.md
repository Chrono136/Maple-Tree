# Changelog
All notable changes to this project will be documented in this file.

## [Alpha Release]

## [1.0.0-542] - 2019-05-31
### Added
- Download content menu option (downloads content based on the provided ID)
- Modify and Delete entry from database list
- Filter out eShop titles

### Fixed
- Missing context menu options

### Changed
- Title list sorted by name

#
## [1.0.0-533] - 2019-05-25
### Improved
- Library loading times
- Database loading times

### Changed
- library sub-directories no longer searched for content

### Removed
- 

### Fixed
- At start up, all library entries return 'TitleInfo::init(): id doesn't exist in titlekeys.json'
- race issue causing library entries to load before the database was populated

#
## [1.0.0-529] - 2019-05-24
### Added
- fallback to current directory on error GameLibrary::init(QString path): invalid directory

### Changed
- Context menu disabled during operations
- Context menu displays directory name
- List item no longer auto selected

### Removed
- config options (All settings are persistent)
- offline toggle option
- download, update, and dlc menu options (replaced by context menu options)
- unified and filtered titlekeys
- reliance on titlekey API (api.tsumes.com)

### Fixed
- Crash when canceling directory select for compression
- Refresh & Change library returned empty or caused crashes
- All database entries title type returning as Game

#
## [1.0.0-515] - 2019-05-23
### Added
- Compress & Decompress option (No use yet, intended for future save file feature)

### Changed
- Cover art is an optional one-time download
- Cover art package URL

#
## [1.0.0-504] - 2019-05-22
### Added
- Client side title list and search input

### Changed
- Cover art are local instead of downloaded on-demand

#
## [1.0.0-497] - 2019-05-21
### Changed
- Context menu decrypt option visiblity based on tmd & cetk existence
- Decrypt skips files that exist and match the supposed file size

### Fixed
- Unresponsive UI when decrypting large files

#
## [1.0.0-493] - 2019-05-20
### Added
- Game, Patch, DLC right click context menu options

### Changed
- Decrypt FILE using to QFile