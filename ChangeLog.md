# Changelog
All notable changes to this project will be documented in this file.

#
## 1.0.0-5**
##### Release: 2019-06-**
##### Added
- Close cemu button combo (L1 + L2 + R1 + R2 + Select)
- Switch tabs using L1 and R1
- Fullscreen cemu option

##### Fixed
- Integrate Cemu doesn't prompt if canceled
- missing directory when attempting to download patch/dlc
- context menu creating empty directories for no reason

##### Changed
- Gamepad start button changed to ButtonA
- TitleInfo::getDirectory() returns existing directory if present

#
## 1.0.0-565
##### Release: 2019-06-10
##### Added
- xinput gamepad support (dpad up & down, start button to launch a game)
- debug log option

##### Changed
- logging info is written to file with more detail
- qt installer framework

##### Removed
- change log tab
- log tab
- verbose log option

##### Fixed
- 

#
## 1.0.0-554
##### Release: 2019-06-07
##### Added
- change log tab
- new game downloads are added to library

##### Fixed
- Downloads going to the wrong directory

#
## 1.0.0-549
##### Release: 2019-06-06
##### Added
- persistent title search when switching region
- export, import, and purge cemu save data

##### Fixed
- Context menu dlc & patch id's were switched causing dlc to download in place of patches and vice versa
- patch/dlc directories being incorrectly created, possibly causing failed decryption

##### Removed
- saved data option under Cemu menu

#
## 1.0.0-542
##### Release: 2019-05-31
#### Added
- Download content menu option (downloads content based on the provided ID)
- Modify and Delete entry from database list
- Filter out eShop titles

#### Fixed
- Missing context menu options

#### Changed
- Title list sorted by name

#
## 1.0.0-533
##### Release: 2019-05-25
### Improved
- Library loading times
- Database loading times

#### Changed
- library sub-directories no longer searched for content

#### Removed
- 

#### Fixed
- At start up, all library entries return 'TitleInfo::init(): id doesn't exist in titlekeys.json'
- race issue causing library entries to load before the database was populated

#
## 1.0.0-529
##### Release: 2019-05-24
#### Added
- fallback to current directory on error GameLibrary::init(QString path): invalid directory

#### Changed
- Context menu disabled during operations
- Context menu displays directory name
- List item no longer auto selected

#### Removed
- config options (All settings are persistent)
- offline toggle option
- download, update, and dlc menu options (replaced by context menu options)
- unified and filtered titlekeys
- reliance on titlekey API (api.tsumes.com)

#### Fixed
- Crash when canceling directory select for compression
- Refresh & Change library returned empty or caused crashes
- All database entries title type returning as Game

#
## 1.0.0-515
##### Release: 2019-05-23
#### Added
- Compress & Decompress option (No use yet, intended for future save file feature)

#### Changed
- Cover art is an optional one-time download
- Cover art package URL

#
## 1.0.0-504
##### Release: 2019-05-22
#### Added
- Client side title list and search input

#### Changed
- Cover art are local instead of downloaded on-demand

#
## 1.0.0-497
##### Release: 2019-05-21
#### Changed
- Context menu decrypt option visiblity based on tmd & cetk existence
- Decrypt skips files that exist and match the supposed file size

#### Fixed
- Unresponsive UI when decrypting large files

#
## 1.0.0-493
##### Release: 2019-05-20
#### Added
- Game, Patch, DLC right click context menu options

#### Changed
- Decrypt FILE using to QFile