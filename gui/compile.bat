C:\Python27\Scripts\pyinstaller.exe explore.spec
xcopy etc dist\gui-bin\etc /e /i /y
xcopy classifiers dist\gui-bin\classifiers /e /i /y
copy defaults.cfg.bck dist\gui-bin\defaults.cfg /y
copy layout.bck.ini dist\gui-bin\layout.ini /y
