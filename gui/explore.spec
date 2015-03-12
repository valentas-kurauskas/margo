# -*- mode: python -*-
#this is a PyInstaller spec file to create a standalone version

a = Analysis(['explore.py'],
             pathex=['c:\\code\\arch\\build\\margo-0.75\\gui'],
             hiddenimports=['scipy.special._ufuncs_cxx', 'sklearn.utils.lgamma', 'sklearn.utils.weight_vector', 
                 'sklearn.utils.arraybuilder', 'Tkinter', 'FixTk', '_tkinter', 'Tkconstants', 'FileDialog', 'Dialog',
 	 	       'mpl_toolkits'],
             hookspath=None,
             runtime_hooks=None)
pyz = PYZ(a.pure)
exe = EXE(pyz,
          a.scripts,
          exclude_binaries=True,
          name='explore.exe',
          debug=False,
          strip=None,
          upx=True,
          console=False,
	     icon='etc\\boa.ico' )
coll = COLLECT(exe,
               a.binaries,
               a.zipfiles,
               a.datas,
               strip=None,
               upx=True,
               name='gui-bin')
